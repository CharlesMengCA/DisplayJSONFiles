// WinMergeScript.cpp : Implementation of CWinMergeScript
#include "stdafx.h"
#include "DisplayJSONFiles.h"
#include "WinMergeScript.h"
#include "rapidjson/include/rapidjson/prettywriter.h"
#include "rapidjson/include/rapidjson/filereadstream.h"
#include "rapidjson/include/rapidjson/filewritestream.h"
#include "rapidjson/include/rapidjson/document.h"

#include <algorithm>

using namespace rapidjson;

/////////////////////////////////////////////////////////////////////////////
// CWinMergeScript


STDMETHODIMP CWinMergeScript::get_PluginEvent(BSTR* pVal)
{
	*pVal = SysAllocString(L"FILE_PACK_UNPACK");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginDescription(BSTR* pVal)
{
	*pVal = SysAllocString(L"Prettify JSON files so WinMerge can compare them");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginFileFilters(BSTR* pVal)
{
	*pVal = SysAllocString(L"\\.json$");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginIsAutomatic(VARIANT_BOOL* pVal)
{
	*pVal = VARIANT_TRUE;
	return S_OK;
}


STDMETHODIMP CWinMergeScript::UnpackBufferA(SAFEARRAY** pBuffer, INT* pSize, VARIANT_BOOL* pbChanged, INT* pSubcode, VARIANT_BOOL* pbSuccess)
{
	// We don't need it
	return S_OK;
}

STDMETHODIMP CWinMergeScript::PackBufferA(SAFEARRAY** pBuffer, INT* pSize, VARIANT_BOOL* pbChanged, INT subcode, VARIANT_BOOL* pbSuccess)
{
	// We don't need it
	return S_OK;
}

template<typename Criteria>
void SortPropertiesOfObject(rapidjson::Value& v, const Criteria& criteria)
{
	std::sort(v.MemberBegin(), v.MemberEnd(), criteria);
}

template<typename Criteria>
void SortItemsOfArray(rapidjson::Value& v, const Criteria& criteria)
{
	std::sort(v.Begin(), v.End(), criteria);
}

auto criteriaObject = [](const Value::Member& left, const Value::Member& right) -> bool
{
	const char* leftStr = left.name.GetString();
	const char* rightStr = right.name.GetString();

	return strcmp(leftStr, rightStr) < 0;
};

auto criteriaArrayObject = [](const Value& left, const Value& right) -> bool
{
	const char* leftStr = left.MemberBegin()->name.GetString();
	const char* rightStr = right.MemberBegin()->name.GetString();

	return strcmp(leftStr, rightStr) < 0;
};

auto criteriaNumber = [](const Value& left, const Value& right)
{
	return left.GetDouble() < right.GetDouble();
};

auto criteriaString = [](const Value& left, const Value& right)
{
	return strcmp(left.GetString(), right.GetString()) < 0;
};

auto criteriaType = [](const Value& left, const Value& right)
{
	Type leftType = left.GetType();
	Type rightType = right.GetType();

	return leftType < rightType;
};

void SortValue(rapidjson::Value& d) {

	if (d.IsObject()) {

		for (Value::MemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr)
		{
			if (itr->value.IsArray()) SortValue(itr->value);
		}

		SortPropertiesOfObject(d, criteriaObject);
	}
	else if (d.IsArray()) {

		if (d[0].IsNumber()) {
			SortItemsOfArray(d, criteriaNumber);
		}
		else if (d[0].IsString()) {
			SortItemsOfArray(d, criteriaString);
		}
		else if (d[0].IsObject()) {

			for (SizeType i = 0; i < d.Size(); i++)
			{
				SortValue(d[i]);
			}

			SortItemsOfArray(d, criteriaArrayObject);
		}

		SortItemsOfArray(d, criteriaType);
	}
}

STDMETHODIMP CWinMergeScript::UnpackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL* pbChanged, INT* pSubcode, VARIANT_BOOL* pbSuccess)
{
	// Prepare reader and input stream.
	FILE* fi, * fo;
	char* readBuffer, * writeBuffer;

	_wfopen_s(&fi, fileSrc, L"rb");
	readBuffer = (char*)malloc(65536);
	FileReadStream is(fi, readBuffer, sizeof(readBuffer));

	// Prepare writer and output stream.
	_wfopen_s(&fo, fileDst, L"wb");
	writeBuffer = (char*)malloc(65536);
	FileWriteStream os(fo, writeBuffer, sizeof(writeBuffer));
	PrettyWriter<FileWriteStream> writer(os);

	Document d;

	if (d.ParseStream<kParseCommentsFlag>(is).HasParseError())
	{
		*pbChanged = VARIANT_FALSE;
		*pbSuccess = VARIANT_FALSE;
	}
	else
	{
		SortValue(d);

		d.Accept(writer);

		*pbChanged = VARIANT_TRUE;
		*pbSuccess = VARIANT_TRUE;
	}

	free(readBuffer);
	free(writeBuffer);

	if (fi) fclose(fi);
	if (fo) fclose(fo);

	return S_OK;
}



STDMETHODIMP CWinMergeScript::PackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL* pbChanged, INT pSubcode, VARIANT_BOOL* pbSuccess)
{
	// Prepare reader and input stream.
	FILE* fi, * fo;
	char* readBuffer, * writeBuffer;

	_wfopen_s(&fi, fileSrc, L"rb");
	readBuffer = (char*)malloc(65536);
	FileReadStream is(fi, readBuffer, sizeof(readBuffer));

	// Prepare writer and output stream.
	_wfopen_s(&fo, fileDst, L"wb");
	writeBuffer = (char*)malloc(65536);
	FileWriteStream os(fo, writeBuffer, sizeof(writeBuffer));
	Writer<FileWriteStream> writer(os);

	Document d;

	if (d.ParseStream<kParseCommentsFlag>(is).HasParseError())
	{
		*pbChanged = VARIANT_FALSE;
		*pbSuccess = VARIANT_FALSE;
	}
	else
	{
		d.Accept(writer);

		*pbChanged = VARIANT_TRUE;
		*pbSuccess = VARIANT_TRUE;
	}

	free(readBuffer);
	free(writeBuffer);

	if (fi) fclose(fi);
	if (fo) fclose(fo);

	return S_OK;
}

STDMETHODIMP CWinMergeScript::ShowSettingsDialog(VARIANT_BOOL* pbHandled)
{
	*pbHandled = VARIANT_FALSE;
	return E_NOTIMPL;
}
