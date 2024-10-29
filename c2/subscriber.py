from client import Client

class ServerSubscriber:
    def on_client_register(self, client: Client):
        pass
    
    def on_client_cmd_output(self, client: Client, output: str):
        pass

    def on_client_exfiltration(self, client: Client):
        pass