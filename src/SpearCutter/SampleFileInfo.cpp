#include "SampleFileInfo.h"
#include <iostream>
CSampleFileInfo::CSampleFileInfo()
{
}

CSampleFileInfo::~CSampleFileInfo()
{
	delete m_pDocumentParser;
}

ECODE CSampleFileInfo::Init(std::tstring strOriginName, std::tstring strFile)
{
	ECODE nRet = EC_READ_FAILURE;
	try
	{
		m_strFile = strFile;
		m_strFileDir = ExtractDirectory(strFile);
		m_strOriginFileName = ExtractFileNameWithoutExt(strOriginName);
		m_strFileHash = ExtractFileNameWithoutExt(strFile);
		m_strExt = MakeLower(ExtractFileExt(strFile));
		
		if (!IsFileExist(strFile))
			throw exception_format(TEXT("Not Exist File(%s)"), strFile.c_str());

		nRet = ReadFileContents(strFile, m_vecFileContents);

		if (EC_SUCCESS != nRet)
			throw exception_format(TEXT("ReadFileContents(%s) Failure"), strFile.c_str());

		BYTE OOXMLSignature[4] = { 0x50, 0x4B, 0x03, 0x04 };
		BYTE COMSignature[8] = { 0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1 };
		
		nRet = EC_INVALID_DATA;
		
		if (0 == memcmp(&m_vecFileContents[0], OOXMLSignature, 4))		m_pDocumentParser = new COoxmlParser;
		else if (0 == memcmp(&m_vecFileContents[0], COMSignature, 8))	m_pDocumentParser = new CCompoundParser;
		else throw exception_format(TEXT("%s is not Document File"), strFile.c_str());
		
		m_pDocumentParser->Parse(strFile);
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
		return nRet;
	}
	return EC_SUCCESS;
}
