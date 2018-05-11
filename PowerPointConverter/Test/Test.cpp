// Test.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#import "../PowerPointConverter/x64/Debug/PowerPointConverter.tlb" no_namespace named_guids 

int main()
{
	CoInitialize(NULL);

	IPPTConverter *pIConverter = NULL;
	HRESULT result = CoCreateInstance(CLSID_PPTXml2BinConverter, NULL, CLSCTX_LOCAL_SERVER, IID_IPPTConverter, (void **)&pIConverter);
	ConvertData data = {13};
	result = pIConverter->Convert_PPTX_2_PPT(&data);

	CoUninitialize();

    return 0;
}

