// PowerPointConverter.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "PowerPointConverter_i.h"
#include "xdlldata.h"


using namespace ATL;


class CPowerPointConverterModule : public ATL::CAtlExeModuleT< CPowerPointConverterModule >
{
public :
	DECLARE_LIBID(LIBID_PowerPointConverterLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_POWERPOINTCONVERTER, "{013e0d26-1ae2-43a9-85d7-ad1e0a32172d}")
};

CPowerPointConverterModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/,
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

