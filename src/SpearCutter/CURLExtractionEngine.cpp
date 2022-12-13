#include "CURLExtractionEngine.h"

// 엔진객체 생성자
CURLExtractEngine::CURLExtractEngine() : CEngineSuper(1, "URLExtraction")
{
    
}

// 엔진객체 소멸자
CURLExtractEngine::~CURLExtractEngine()
{

}

// OOXML형식의 문서에서 외부개체 URL을 가져오는 함수
std::vector<std::string> CURLExtractEngine::GetOOXMLFormat_ExternalURL()
{
    std::vector<std::string> extractedURLList;
    ECODE nRet = EC_SYSTEM_ERROR;
    // 엔진에 대한 parser를 OOXML형식으로 변경
    COoxmlParser* pParser = (COoxmlParser*)Sample()->m_pDocumentParser;
    
    std::vector<std::tstring> vecRels;
    pParser->QueryStreamPath(TEXT("*/*/*.xml.rels"), vecRels);
    for(std::tstring& strRelsPath : vecRels)
    {
        std::cout << "현재 분석하는 파일: " << strRelsPath << std::endl;

        std::vector<BYTE> vecRelsBinary;
        nRet = pParser->QueryData(strRelsPath, vecRelsBinary);
        if(EC_SUCCESS != nRet)
            throw engine_Exception("URLExtraction", "ss", strRelsPath.c_str(),"를 QuertData에 실패했습니다.");
        
        ST_RELS stRels;
        // xml.rels 파일을 읽어온다.
        if(!ReadXmlFromString(&stRels, TCSFromUTF8((LPCSTR)vecRelsBinary.data(), vecRelsBinary.size())))
            throw engine_Exception("URLExtraction", "ss", strRelsPath.c_str(), "를 ReadFromUTF8을 통해 읽기 실패했습니다");

        for(int i =0; i < stRels.vecRelationship.size(); i++)
        {   
            std::regex externalObjectsRe(R"(oleObject|attachedTemplate|frame)");
            std::smatch match;
            // 만약 TargetMode가 External이고 Type에 oleObject, attachedTemplate, frame이 들어갔다면
            if(stRels.vecRelationship[i].strTargetMode == "External" && std::regex_search(stRels.vecRelationship[i].strType, match, externalObjectsRe))
            {
                std::cout << strRelsPath << "에서 외부개체 Url 발견" << std::endl;
                std::regex extractUrlRe(R"((h|H)(t|T)(t|T)(p|P)s?[\w.%/?=:@_-]*)");
                std::regex_search(stRels.vecRelationship[i].strTarget, match, extractUrlRe);
                extractedURLList.push_back(match.str());
            }
        }
    }
    pParser->Close();
    return extractedURLList;
}

// 미구현 영역
std::vector<std::string> CURLExtractEngine::GetCompoundFormat_ExternalURL()
{
    std::vector<std::string> extractedURLList;
    ECODE nRet = EC_SYSTEM_ERROR;
    // 엔진에 대한 parser를 OOXML형식으로 변경
    CCompoundParser* pParser = (CCompoundParser*)Sample()->m_pDocumentParser;
    std::vector<std::tstring> vecRels;
    std::map<std::tstring, ST_OLE_DIRECTORY_ENTRY> mapDirectoryEntries;

    pParser->QueryStoragePath(TEXT("*"), mapDirectoryEntries);

    return extractedURLList;
}

// CURLExtractEngine의 URL추출 함수
bool CURLExtractEngine::Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output)
{
    // 만약 샘플 파일이 OOXML 형식이라면?
    if(Sample()->m_pDocumentParser->GetType() == DOCUMENT_OOXML)
        output->vecExtractedUrls = GetOOXMLFormat_ExternalURL();
    // 컴파운드 형식이라 판단
    else
        output->vecExtractedUrls = GetCompoundFormat_ExternalURL();

    if(output->vecExtractedUrls.empty()) 
        return false;
    
    for(int i = 0; i < output->vecExtractedUrls.size(); i++)
    {
        ST_BEHAVIOR url;
        url.strUrl.append("Document Inner External Object URL");
        url.Severity = 0;
        url.strName.append("문서 내부에 외부개체를 다운로드를 위한 URL이 있습니다.");
        url.strDesc.append(output->vecExtractedUrls[i]);
        url.strDesc.append("을 통해 파일을 다운받고 있습니다.");
        output->vecBehaviors.push_back(url);
    }
    return true;
}
