// PPTXml2BinConverter.cpp : Implementation of CPPTXml2BinConverter

#include "stdafx.h"
#include "PPTXml2BinConverter.h"


// CPPTXml2BinConverter

STDMETHODIMP CPPTXml2BinConverter::Convert_PPTX_2_PPT(ConvertData * name, wchar_t pwzBuffer[260]) {
	pwzBuffer[0] = 'A';
	pwzBuffer[1] = 'B';
	//CComBSTR sResult(L"123");
	//*pVal = sResult.Copy();
	//pVal[0] = '1';

	return S_OK;
}