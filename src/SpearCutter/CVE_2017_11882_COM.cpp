#include "CVE_2017_11882_COM.h"
#include "SampleFileInfo.h"

CCVE_2017_11882_COM::CCVE_2017_11882_COM()
{
}

CCVE_2017_11882_COM::~CCVE_2017_11882_COM()
{
}

//TODO : COM 내부에 존재하는 또다른 문서파일 처리 필요 -> 여기서 해야하는가?
ECODE CCVE_2017_11882_COM::Analyze(const std::vector<std::tstring> vecExtractFiles, ST_ANALYZE_RESULT* output)
{
	if (Sample()->m_pDocumentParser->GetType() != DOCUMENT_COM) return EC_SUCCESS;
	CCompoundParser* pParser = (CCompoundParser*)(Sample()->m_pDocumentParser);
	ECODE nRet = EC_SYSTEM_ERROR;
	try
	{
		std::map<std::tstring, ST_OLE_DIRECTORY_ENTRY> mapDirectoryEntry;
		pParser->QueryStoragePath(TEXT("*"), mapDirectoryEntry);

		// Directory가 0이면 RootDirectory도 없다는 뜻이므로, 포맷에 맞지 않음
		if (mapDirectoryEntry.empty())
			throw exception_format(TEXT("Can not find Root Directory at (%s)"), Sample()->m_strFile);

		BYTE pbEqnedtClsid[16] = { 0x02, 0xCE, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 };
		for (auto& iter : mapDirectoryEntry)
		{
			// CLSID가 EQNEDT32.EXE가 아닌 Directory는 해당사항 없음
			if (0 != memcmp(iter.second.btGUID, pbEqnedtClsid, 16))
				continue;

			ST_BEHAVIOR detection;
			detection.strUrl.append("Local EQNEDT");
			detection.strName.append("Include EQNEDT32.EXE");
			detection.strDesc = "수식편집기 취약점을 유발할 수 있는 EQNEDT32.EXE를 포함하고 있다.";
			detection.Severity = 0;
			output->vecBehaviors.push_back(detection);

			std::vector<std::tstring> vecStream;
			std::tstring strTargetStream = MakeFormalPath(Format(TEXT("%s/*"), iter.first.c_str()).c_str());
			pParser->QueryStreamPath(strTargetStream, vecStream);
			if (vecStream.empty())
			{
				Log_Info(TEXT("Eqnedt32 with no stream data"));
				continue;
			}

			std::vector<BYTE> vecStreamBinary;
			for (std::tstring& strStreamPath : vecStream)
			{
				vecStreamBinary.clear();
				nRet = pParser->QueryData(strStreamPath, vecStreamBinary);
				if (EC_SUCCESS != nRet)
					throw exception_format(TEXT("QueryData(%s) Failure in %s"), strStreamPath.c_str(), iter.first.c_str());

				if (vecStreamBinary.size() < 0x0E)
					continue;

				int nFontNamePos = 0x0E;
				int nStringSize = 0;
				while (vecStreamBinary[nFontNamePos] != 0x00)
				{
					nFontNamePos++;
					nStringSize++;
				}

				if (0x20 < nStringSize)
				{
					ST_BEHAVIOR detection;
					detection.strUrl = "Local Document";
					detection.strName.append("CVE-2017-11882");
					detection.strDesc.append("수식편집기 프로그램인 EQNEDT32.EXE에서 BufferOverFlow를 통한 취약점 트리깅");
					detection.Severity = 10;
					output->vecBehaviors.push_back(detection);
				}
			}
		}
		
		// 내부에 존재하는 또다른 OOXML, COM 문서 추출
		std::vector<std::tstring> vecStream;
		pParser->QueryStreamPath(TEXT("*"), vecStream);
		if (vecStream.empty())
			return EC_SUCCESS;
		
		HANDLE hFile = nullptr;
		std::vector<BYTE> vecStreamBinary;
		for (std::tstring& strStream : vecStream)
		{
			try
			{
				nRet = pParser->QueryData(strStream, vecStreamBinary);
				if (EC_SUCCESS != nRet)
					throw exception_format(TEXT("QueryData(%s) in Failure"), strStream.c_str());
				
				if (vecStreamBinary.empty())
					throw exception_format(TEXT("%s is 0 size"), strStream.c_str());

				BYTE OOXMLSignature[4] = { 0x50, 0x4B, 0x03, 0x04 };
				BYTE COMSignature[8] = { 0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1 };
				if (memcmp(&vecStreamBinary[0], OOXMLSignature, 4) && memcmp(&vecStreamBinary[0], COMSignature, 8))
					continue;
				
				std::tstring strExtractedFile = Format(TEXT("../temp/Extracted/%s.spearcutter"), ExtractFileName(strStream).c_str());
				hFile = CreateFile(strExtractedFile.c_str(), GENERIC_WRITE_, CREATE_ALWAYS_, FILE_ATTRIBUTE_NORMAL_);
				if (nullptr == hFile)
					throw exception_format(TEXT("CreateFile(%s) Failure, %d"), strExtractedFile.c_str(), GetLastError());

				nRet = WriteFileContents(strExtractedFile, vecStreamBinary);
				if (EC_SUCCESS != nRet)
					throw exception_format(TEXT("WriteFileContents(%s) Failure"), strExtractedFile.c_str());

				CloseFile(hFile);
				output->vecExtractedFiles.push_back(std::make_pair(strExtractedFile,ASF)); // 추출된 파일
			}
			catch (const std::exception& e)
			{
				if (nullptr != hFile)
					CloseFile(hFile);
				Log_Warn("%s", e.what());
				continue;
			}
		}
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
		return nRet;
	}
	return EC_SUCCESS;
}
