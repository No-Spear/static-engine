#pragma once
#include "../DocumentParser/DocumentParser.h"
#include <regex>

using namespace core;

class CSampleFileInfo
{
	CSampleFileInfo();
	~CSampleFileInfo();
	
public:
	std::tstring m_strFile;
	std::tstring m_strFileDir;
	std::tstring m_strOriginFileName;
	std::tstring m_strFileHash;
	std::tstring m_strExt;
	std::vector<BYTE> m_vecFileContents;
	CDocumentParserSuper* m_pDocumentParser;
	
	static CSampleFileInfo* GetInstance()
	{
		static CSampleFileInfo instance;
		return &instance;
	}
	
	ECODE Init(std::tstring strOriginName, std::tstring strFile);
};

inline CSampleFileInfo* Sample()
{
	return CSampleFileInfo::GetInstance();
}