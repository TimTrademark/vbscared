# VBScared Tool 👻

VBScared is an executable, disguised as a .jpg file, that executes a VBScript in-memory reverse shell upon double clicking. The executable can be delivered to victims via social engineering tactics.

## Compile resources

```
windres resources\resources.rc -O coff --target pe-i386 -o resources\resources.res
```

## Compile executable

```
cl runscript.c /DEBUG:NONE /link /subsystem:windows /entry:mainCRTStartup
```

## Automated build

Or you can use the `generate.py` script under `scripts` to automate the build process

## Run c2

Wait for a connection ;).

## Features
* Manage multiple connected clients
* C2 detection evasion, the C2 detects client requests from investigation/other requests and will answer with 404 if request is not coming from client
* Exfilatration macro available
