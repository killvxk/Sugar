// PPTXml2BinConverter.h : Declaration of the CPPTXml2BinConverter

#pragma once
#include "resource.h"       // main symbols


#include "PowerPointConverter_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CPPTXml2BinConverter

class ATL_NO_VTABLE CPPTXml2BinConverter :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPPTXml2BinConverter, &CLSID_PPTXml2BinConverter>,
	public IDispatchImpl<IPPTConverter, &IID_IPPTConverter, &LIBID_PowerPointConverterLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CPPTXml2BinConverter()
	{
	}

DECLARE_REGISTRY_RESOURCEID(106)


BEGIN_COM_MAP(CPPTXml2BinConverter)
	COM_INTERFACE_ENTRY(IPPTConverter)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	STDMETHOD(Convert_PPTX_2_PPT)(ConvertData * name);


};

OBJECT_ENTRY_AUTO(__uuidof(PPTXml2BinConverter), CPPTXml2BinConverter)
