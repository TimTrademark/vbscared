import uuid
import friendlywords as fw
import time

class Client:
    def __init__(self, ip: str):
        self.ip = ip
        self.friendly_name = fw.generate('po', separator='-')
        self.cmd = "NOP"
        self.first_ping_timestamp = time.time()
    
    def read_cmd_and_clear(self):   
        cmd = self.cmd
        self.cmd = "NOP"
        return cmd
    
    def set_cmd(self, cmd:str):
        self.cmd = cmd
    
    def set_ip(self, ip: str):
        self.ip = ip