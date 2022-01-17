// WinMergeScript.cpp : Implementation of CWinMergeScript
#include "stdafx.h"
#include "DisplayJSONFiles.h"
#include "WinMergeScript.h"
#include "rapidjson/reader.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"

using namespace rapidjson;

/////////////////////////////////////////////////////////////////////////////
// CWinMergeScript


STDMETHODIMP CWinMergeScript::get_PluginEvent(BSTR *pVal)
{
	*pVal = SysAllocString(L"FILE_PACK_UNPACK");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginDescription(BSTR *pVal)
{
	*pVal = SysAllocString(L"This plugins hides the first non-space character");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginFileFilters(BSTR *pVal)
{
	*pVal = SysAllocString(L"\\.json$");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginIsAutomatic(VARIANT_BOOL *pVal)
{
	*pVal = VARIANT_TRUE;
	return S_OK;
}


STDMETHODIMP CWinMergeScript::UnpackBufferA(SAFEARRAY **pBuffer, INT *pSize, VARIANT_BOOL *pbChanged, INT *pSubcode, VARIANT_BOOL *pbSuccess)
{
	// We don't need it
	return S_OK;
}

STDMETHODIMP CWinMergeScript::PackBufferA(SAFEARRAY **pBuffer, INT *pSize, VARIANT_BOOL *pbChanged, INT subcode, VARIANT_BOOL *pbSuccess)
{
	// We don't need it
	return S_OK;
}

STDMETHODIMP CWinMergeScript::UnpackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL *pbChanged, INT *pSubcode, VARIANT_BOOL *pbSuccess)
{
	// Prepare reader and input stream.
	FILE* fi, * fo;
	char* readBuffer, * writeBuffer;

	_wfopen_s(&fi, fileSrc, L"rb");
	Reader reader;
	readBuffer = (char*)malloc(65536);
	FileReadStream is(fi, readBuffer, sizeof(readBuffer));

	// Prepare writer and output stream.
	_wfopen_s(&fo, fileDst, L"wb");
	writeBuffer = (char*)malloc(65536);
	FileWriteStream os(fo, writeBuffer, sizeof(writeBuffer));
	PrettyWriter<FileWriteStream> writer(os);

	// JSON reader parse from the input stream and let writer generate the output.
	if (reader.Parse<kParseValidateEncodingFlag>(is, writer)) {
		*pbChanged = VARIANT_TRUE;
		*pbSuccess = VARIANT_TRUE;
	}
	else
	{
		*pbChanged = VARIANT_FALSE;
		*pbSuccess = VARIANT_FALSE;
	}

	free(readBuffer);
	free(writeBuffer);

	if (fi) fclose(fi);
	if (fo) fclose(fo);

	return S_OK;
}



STDMETHODIMP CWinMergeScript::PackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL *pbChanged, INT pSubcode, VARIANT_BOOL *pbSuccess)
{
	// Prepare reader and input stream.
	FILE* fi, * fo;
	char* readBuffer, * writeBuffer;

	_wfopen_s(&fi, fileSrc, L"rb");
	Reader reader;
	readBuffer = (char*)malloc(65536);
	FileReadStream is(fi, readBuffer, sizeof(readBuffer));

	// Prepare writer and output stream.
	_wfopen_s(&fo, fileDst, L"wb");
	writeBuffer = (char*)malloc(65536);
	FileWriteStream os(fo, writeBuffer, sizeof(writeBuffer));
	Writer<FileWriteStream> writer(os);

	// JSON reader parse from the input stream and let writer generate the output.
	if (reader.Parse<kParseValidateEncodingFlag>(is, writer)) {
		*pbChanged = VARIANT_TRUE;
		*pbSuccess = VARIANT_TRUE;
	}
	else
	{
		*pbChanged = VARIANT_FALSE;
		*pbSuccess = VARIANT_FALSE;
	}

	free(readBuffer);
	free(writeBuffer);

	if (fi) fclose(fi);
	if (fo) fclose(fo);

	return S_OK;

}

STDMETHODIMP CWinMergeScript::ShowSettingsDialog(VARIANT_BOOL *pbHandled)
{
	*pbHandled = VARIANT_FALSE;
	return E_NOTIMPL;
}
