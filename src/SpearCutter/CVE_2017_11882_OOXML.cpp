#include "CVE_2017_11882_OOXML.h"
#include "SampleFileInfo.h"

CCVE_2017_11882_OOXML::CCVE_2017_11882_OOXML()
{
}

CCVE_2017_11882_OOXML::~CCVE_2017_11882_OOXML()
{
}

ECODE CCVE_2017_11882_OOXML::Analyze(const std::vector<std::tstring> vecExtractFiles, ST_ANALYZE_RESULT* output)
{
	if (Sample()->m_pDocumentParser->GetType() != DOCUMENT_OOXML) return EC_SUCCESS;
	COoxmlParser* pParser = (COoxmlParser*)(Sample()->m_pDocumentParser);
	ECODE nRet = EC_SYSTEM_ERROR;
	
	// sheet[N].xml이 존재하지 않으면 해당사항 없음
	std::vector<std::tstring> vecSheet;
	pParser->QueryStreamPath(TEXT("*/worksheets/*.xml"), vecSheet);
	if (vecSheet.empty())
		return EC_SUCCESS;

	// OOXML 포맷 내 존재하는 OleObjectFile 추출
	std::vector<std::tstring> vecOleObjectFile;
	for (std::tstring& strSheet : vecSheet)
	{
		try
		{
			// worksheet를 구조체에 담기
			std::vector<BYTE> vecSheetBinary;
			nRet = pParser->QueryData(strSheet, vecSheetBinary);
			if (EC_SUCCESS != nRet)
				throw exception_format(TEXT("QueryData(%s) Failure, %d"), strSheet.c_str(), nRet);

			ST_WORKSHEET_XML stSheet;
			if (!ReadXmlFromString(&stSheet, TCSFromUTF8((LPCSTR)vecSheetBinary.data(), vecSheetBinary.size())))
				throw exception_format(TEXT("ReadXmlFromString() in %s Failure"), strSheet.c_str());

			// worksheet에 EmbeddedObject가 없으면 해당사항 없음
			if (stSheet.vecEmbeddedObject.empty())
				continue;

			// EmbeddedObject에 r:id 항목이 있는지 검사
			std::vector<std::tstring> vecRid;
			for (int i = 0; i < stSheet.vecEmbeddedObject.size(); i++)
			{
				for (int j = 0; j < stSheet.vecEmbeddedObject[i].vecOleObject.size(); j++)
					vecRid.push_back(stSheet.vecEmbeddedObject[i].vecOleObject[j].strId);
			}
			
			// r:id 항목이 없다면 해당사항 없음
			if (vecRid.empty())
				continue;

			// 있다면 반드시 .rels파일이 존재함
			std::tstring strRels = Format(TEXT("%s/_rels/%s.rels"), ExtractDirectory(strSheet).c_str(), ExtractFileName(strSheet).c_str());
			std::vector<BYTE> vecRelsBinary;
			nRet = pParser->QueryData(strRels, vecRelsBinary);
			if (EC_SUCCESS != nRet)
				throw exception_format(TEXT("QueryData(%s) failure, %d"), strRels.c_str(), nRet);

			ST_RELS stRels;
			if (!ReadXmlFromString(&stRels, TCSFromUTF8((LPSTR)vecRelsBinary.data(), vecRelsBinary.size())))
				throw exception_format(TEXT("ReadXmlFromString() in %s Failure"), strRels.c_str());

			std::tstring strOleObjectFile;
			for (std::tstring& strRid : vecRid)
			{
				for (int i = 0; i < stRels.vecRelationship.size(); i++)
				{
					if (strRid != stRels.vecRelationship[i].strId)
						continue;

					strOleObjectFile = Format(TEXT("%s/%s"), ExtractDirectory(strSheet).c_str(), stRels.vecRelationship[i].strTarget.c_str());
					vecOleObjectFile.push_back(MakeFormalPath(strOleObjectFile));
				}
			}
		}
		catch (const std::exception& e)
		{
			Log_Warn("%s", e.what());
			continue;
		}
	}

	if (vecOleObjectFile.empty())
		return EC_SUCCESS;

	ST_BEHAVIOR detection;
	detection.strUrl.append("Local OleObject");
	detection.strName.append("Insert OleObject in DocumentFile");
	detection.strDesc.append("문서파일 내부에 Ole Object File이 포함되어있다.");
	detection.Severity = 0;
	output->vecBehaviors.push_back(detection);
	
	// 발견된 OleObjects 파일을 추출하기
	HANDLE hFile = nullptr;
	std::vector<BYTE> vecOleObjectBinary;
	for (std::tstring& strOleObjectFile : vecOleObjectFile)
	{
		try
		{
			nRet = pParser->QueryData(strOleObjectFile, vecOleObjectBinary);
			if (EC_SUCCESS != nRet)
				throw exception_format(TEXT("QueryData(%s) Failure, %d"), strOleObjectFile.c_str(), nRet);

			if (vecOleObjectBinary.empty())
				throw exception_format(TEXT("/*%s is 0 size*/"), strOleObjectFile.c_str());

			std::tstring strExtractedFile = Format(TEXT("../temp/Extracted/%s.spearcutter"), ExtractFileName(strOleObjectFile).c_str());

			if (!IsFileExist(ExtractDirectory(strExtractedFile)))
			{
				if (CreateDirectoryRecursively(ExtractDirectory(strExtractedFile)))
					throw exception_format(TEXT("CreateDirectoryRecursively(%s) Failure, %d"), ExtractDirectory(strExtractedFile).c_str(), GetLastError());
			}

			hFile = CreateFile(strExtractedFile.c_str(), GENERIC_WRITE_, CREATE_ALWAYS_, FILE_ATTRIBUTE_NORMAL_);
			if (nullptr == hFile)
				throw exception_format(TEXT("CreateFile(%s) Failure, %d"), strExtractedFile.c_str(), GetLastError());

			nRet = WriteFileContents(strExtractedFile, vecOleObjectBinary);
			if (EC_SUCCESS != nRet)
				throw exception_format(TEXT("WriteFileContents(%s) Failure"), strExtractedFile.c_str());

			CloseFile(hFile);
			output->vecExtractedFiles.push_back(std::make_pair(strExtractedFile, ASF));
		}
		catch (const std::exception& e)
		{
			if (nullptr != hFile)
				CloseFile(hFile);

			Log_Warn("%s", e.what());
			continue;
		}
	}
	return EC_SUCCESS;
}
