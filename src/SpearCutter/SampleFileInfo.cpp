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

		// EncryptionInfo의 바이너리 데이터
		BYTE BEncryptionInfo[28] = { 0x45, 0x00, 0x6E, 0x00, 0x63, 0x00, 0x72, 0x00, 0x79, 0x00, 0x70, 0x00, 0x74, 0x00, 0x69, 0x00,
									0x6F, 0x00, 0x6E, 0x00, 0x49, 0x00, 0x6E, 0x00, 0x66, 0x00, 0x6F, 0x00 };
		// EncryptedPackage의 바이너리 데이터
		BYTE BEncryptedPackage[32] = { 0x45, 0x00, 0x6E, 0x00, 0x63, 0x00, 0x72, 0x00, 0x79, 0x00, 0x70, 0x00, 0x74, 0x00, 0x65, 0x00,
									0x64, 0x00, 0x50, 0x00, 0x61, 0x00, 0x63, 0x00, 0x6B, 0x00, 0x61, 0x00, 0x67, 0x00, 0x65, 0x00 };
		
		// 먼약 파일이 잠겨있다면?
		if(0 == memcmp(&m_vecFileContents[1152], BEncryptionInfo, 28) && 0 == memcmp(&m_vecFileContents[1280], BEncryptedPackage, 32))
		{
			std::cout << "파일이 잠겨있다." << std::endl;
			std::tstring msoffcrypto_tool;
			std::regex space(R"( )");
    		std::regex leftbracket(R"(\()");
    		std::regex rightbracket(R"(\))");
    		std::regex andName(R"(\&)");
			msoffcrypto_tool.append("msoffcrypto-tool ");
			std::tstring unspace = TCSFromMBS(std::regex_replace(strFile, space, "\\ "));
    		std::tstring unleft = TCSFromMBS(std::regex_replace(unspace, leftbracket, "\\("));
    		std::tstring unright = TCSFromMBS(std::regex_replace(unleft, rightbracket, "\\)"));
    		msoffcrypto_tool.append(std::regex_replace(unright, andName, "\\&"));
			msoffcrypto_tool.append(" ../temp/Decrypted/");
			msoffcrypto_tool.append(m_strFileHash+".");
			msoffcrypto_tool.append(m_strExt);
			msoffcrypto_tool.append(" -p VelvetSweatshop");
			std::cout << msoffcrypto_tool << std::endl;
			system(msoffcrypto_tool.c_str());
		}
		
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
