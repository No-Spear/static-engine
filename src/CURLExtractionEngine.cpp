#include "CURLExtractionEngine.h"

// 문서 파일 포맷 생성자
ContainerParserSuper::ContainerParserSuper()
{

}

// 문서 파일 포맷 소멸자
ContainerParserSuper::~ContainerParserSuper()
{

}

// OOXML 파일 포맷 생성자
OOXMLParser::OOXMLParser()
{

}

// OOXML 파일 포맷 소멸자
OOXMLParser::~OOXMLParser()
{

}

// OOXML 형식의 문서 파일을 여는 함수
bool OOXMLParser::open(const char* pszFile, const char* parserInfo)
{
    // 문서의 타입을 보고 문서의 타입에 맞게 contentxml을 재정의
    if(parserInfo == "WordParser")
        this->contentxml = "word/_rels/document.xml.rels";
    else if(parserInfo == "ExcelParser")
        this->contentxml = "";
    else if(parserInfo == "PowerPointParser")
        this->contentxml = "";
    

    // OOXML 객체를 ZIP 파일로 변환하여 Open 
    this->OOXML = zip_open(pszFile, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
    if(this->OOXML == NULL)
    {
        std::cout << "분석을 의뢰한 파일을 열 수 없습니다." << std::endl;
        return false;
    }
    // OOXML 객체의 Contentxml 데이터를 얻어온다.
    if(zip_entry_open(this->OOXML, contentxml) != 0)
    {
        std::cout << "파일을 열 수 없습니다." << std::endl;
        return false;
    }
    return true;
}

// OOXML 형식의 문서 파일을 닫는 함수
bool OOXMLParser::close(void)
{
    free(this->OOXML);
    free(this->buffer);
    return true;
}

// OOXML 형식의 문서 파일의 스트림 데이터를 얻는 함수
void* OOXMLParser::getStreamData(void)
{
    // 문서파일에서 XML 데이터를 buf에 저장한다.
    if(zip_entry_read(this->OOXML, &buffer, &bufsize) < 0)
    {
        std::cout << "파일 내용을 열 수 없습니다." << std::endl;
        return NULL;
    }
    return this->buffer;
}

// Compound 파일 포맷 생성자
CompoundParser::CompoundParser()
{

}

// Compound 파일 포맷 소멸자
CompoundParser::~CompoundParser()
{

}

// 문서 형식 생성자
DocumentParserSuper::DocumentParserSuper(ContainerParserSuper* pContainer)
{
    this->container = pContainer;
}

// 문서 형식 소멸자
DocumentParserSuper::~DocumentParserSuper()
{
    delete(this->container);
}

// Word 문서 형식에 대한 생성자
WordParser::WordParser(ContainerParserSuper* pContainer) : DocumentParserSuper(pContainer)
{
    this->paserInfo = "WordParser";
}

// Word 문서 형식에 대한 소멸자
WordParser::~WordParser()
{
    
}

// Word 문서에서 얻은 스트림 데이터에서 Url 데이터만 가져오는 함수
std::string WordParser::parsingUrl(const std::string input)
{
    // 1차 정규식을 통해 Target="~"부분을 가져온다.
    std::smatch firstmatch;
    std::regex firstre(R"(Target[\s]*=[\s]*"[a-zA-Z0-9-_.~!*'();:@&=+$,/?%#\[\]]*")");
    std::regex_search(input, firstmatch, firstre);

    // 이후 2차 정규식을 통해 mhtml의 부분을 제거하고 순수 url을 가져온다.
    std::smatch secondmatch;
    std::string urlinput = firstmatch.str();
    std::regex scondre("https?://[A-Za-z0-9./]*");
    std::regex_search(urlinput, secondmatch, scondre);
    return secondmatch.str();
}

// 어떠한 문서파일을 분석하는지 알려주는 함수
char* WordParser::getParserInfo()
{
    return (char*)this->paserInfo;
}

// 문서파일을 열고 문서파일의 스트림 데이터에서 Url을 추출하고 돌려주는 함수
std::vector<std::string> WordParser::getUrlList(std::string samplePath)
{
    // 문서파일의 스트림 데이터를 받을 포인터
    char* buffer;
    // 문서파일의 스트림 데이터서 뽑은 Url을 받을 벡터
    std::vector<std::string> UrlList;

    std::regex re(R"(<Relationship[\s]*Id=[\s]*"[A-Za-z0-9]*"[\s]*Type[\s]*=[\s]*"[A-Za-z0-9-:/.]*\/oleObject"[\s]*Target[\s]*=[\s]*"[a-zA-Z0-9-_.~!*'();:@&=+$,/?%#]*"[\s]*TargetMode[\s]*=[\s]*"External")");
    std::smatch match;

    // 문서파일의 스트림 데이터를 열수 있는지 확인.
    if(!this->container->open(samplePath.c_str(), getParserInfo()))
        return UrlList;
    
    buffer = (char*)this->container->getStreamData();

    // 문서파일의 스트림 데이터에서 OleObject의 Url만 뽑아 낸다.
    std::string xml((char*)buffer);
    while (std::regex_search(xml, match, re)) {
        UrlList.push_back(parsingUrl(match.str()));
        xml = match.suffix();
    }
    return UrlList;
}

// Excel 문서 형식에 대한 생성자
ExcelParser::ExcelParser(ContainerParserSuper* pContainer) : DocumentParserSuper(pContainer)
{
    this->paserInfo = "ExcelParser";
}

// Excel 문서 형식에 대한 소멸자
ExcelParser::~ExcelParser()
{

}

// Excel 문서에서 얻은 스트림 데이터에서 Url 데이터만 가져오는 함수
std::string ExcelParser::parsingUrl(const std::string input)
{
    // 1차 정규식을 통해 Target="~"부분을 가져온다.
    std::smatch firstmatch;
    std::regex firstre(R"(Target[\s]*=[\s]*"[a-zA-Z0-9-_.~!*'();:@&=+$,/?%#\[\]]*")");
    std::regex_search(input, firstmatch, firstre);

    // 이후 2차 정규식을 통해 mhtml의 부분을 제거하고 순수 url을 가져온다.
    std::smatch secondmatch;
    std::string urlinput = firstmatch.str();
    std::regex scondre("https?://[A-Za-z0-9./]*");
    std::regex_search(urlinput, secondmatch, scondre);
    return secondmatch.str();
}

// 어떠한 문서파일을 분석하는지 알려주는 함수
char* ExcelParser::getParserInfo()
{
    return (char*)this->paserInfo;
}

// 문서파일을 열고 문서파일의 스트림 데이터에서 Url을 추출하고 돌려주는 함수
std::vector<std::string> ExcelParser::getUrlList(std::string samplePath)
{
    // 문서파일의 스트림 데이터를 받을 포인터
    char* buffer;
    // 문서파일의 스트림 데이터서 뽑은 Url을 받을 벡터
    std::vector<std::string> UrlList;

    std::regex re(R"(<Relationship[\s]*Id=[\s]*"[A-Za-z0-9]*"[\s]*Type[\s]*=[\s]*"[A-Za-z0-9-:/.]*\/oleObject"[\s]*Target[\s]*=[\s]*"[a-zA-Z0-9-_.~!*'();:@&=+$,/?%#]*"[\s]*TargetMode[\s]*=[\s]*"External")");
    std::smatch match;

    // 문서파일의 스트림 데이터를 열수 있는지 확인. 
    // 해당부분 처리 필요.
    if(!this->container->open(samplePath.c_str(), getParserInfo()))
        return UrlList;
    
    buffer = (char*)this->container->getStreamData();

    // 문서파일의 스트림 데이터에서 OleObject의 Url만 뽑아 낸다.
    std::string xml((char*)buffer);
    while (std::regex_search(xml, match, re)) {
        UrlList.push_back(parsingUrl(match.str()));
        xml = match.suffix();
    }
    return UrlList;
}

// PowerPoint 문서 형식에 대한 생성자
PowerPointParser::PowerPointParser(ContainerParserSuper* pContainer) : DocumentParserSuper(pContainer)
{
    this->paserInfo = "PowerPointParser";
}

// PowerPoint 문서 형식에 대한 소멸자
PowerPointParser::~PowerPointParser()
{

}

// PowerPoint 문서에서 얻은 스트림 데이터에서 Url 데이터만 가져오는 함수
std::string PowerPointParser::parsingUrl(const std::string input)
{
    // 1차 정규식을 통해 Target="~"부분을 가져온다.
    std::smatch firstmatch;
    std::regex firstre(R"(Target[\s]*=[\s]*"[a-zA-Z0-9-_.~!*'();:@&=+$,/?%#\[\]]*")");
    std::regex_search(input, firstmatch, firstre);

    // 이후 2차 정규식을 통해 mhtml의 부분을 제거하고 순수 url을 가져온다.
    std::smatch secondmatch;
    std::string urlinput = firstmatch.str();
    std::regex scondre("https?://[A-Za-z0-9./]*");
    std::regex_search(urlinput, secondmatch, scondre);
    return secondmatch.str();
}

// 어떠한 문서파일을 분석하는지 알려주는 함수
char* PowerPointParser::getParserInfo()
{
    return (char*)this->paserInfo;
}

// 문서파일을 열고 문서파일의 스트림 데이터에서 Url을 추출하고 돌려주는 함수
std::vector<std::string> PowerPointParser::getUrlList(std::string samplePath)
{
    // 문서파일의 스트림 데이터를 받을 포인터
    char* buffer;
    // 문서파일의 스트림 데이터서 뽑은 Url을 받을 벡터
    std::vector<std::string> UrlList;

    std::regex re(R"(<Relationship[\s]*Id=[\s]*"[A-Za-z0-9]*"[\s]*Type[\s]*=[\s]*"[A-Za-z0-9-:/.]*\/oleObject"[\s]*Target[\s]*=[\s]*"[a-zA-Z0-9-_.~!*'();:@&=+$,/?%#]*"[\s]*TargetMode[\s]*=[\s]*"External")");
    std::smatch match;

    // 문서파일의 스트림 데이터를 열수 있는지 확인. 
    // 해당부분 처리 필요.
    if(!this->container->open(samplePath.c_str(), getParserInfo()))
        return UrlList;
    
    buffer = (char*)this->container->getStreamData();

    // 문서파일의 스트림 데이터에서 OleObject의 Url만 뽑아 낸다.
    std::string xml((char*)buffer);
    while (std::regex_search(xml, match, re)) {
        UrlList.push_back(parsingUrl(match.str()));
        xml = match.suffix();
    }
    return UrlList;
}

// 엔진객체 생성자
CURLExtractEngine::CURLExtractEngine() : CEngineSuper(1)
{
    
}



// 엔진객체 소멸자
CURLExtractEngine::~CURLExtractEngine()
{

}

// 분석의뢰를 받은 파일에 대한 형식을 얻어내는 함수
std::string CURLExtractEngine::extractFileExe(const std::string docpath)
{
    // 파일 형식자를 찾기위해 마지막 .의 위치를 찾는다.
    int location = docpath.find_last_of('.');
    // 해당 파일형식자를 새로운 스트링에 담는다.
    std::string doctype(docpath.substr(location+1));
    // 해당 파일 형식자를 vector에 넣어둔다.
    return doctype;
}

bool CURLExtractEngine::urlParsing(std::string input, std::string doctype, std::vector<std::string>& output)
{
    // 입력받은 파일에 맞는 객체를 생성
    // Word 파일의 경우
    if(doctype.front() == 'd')
    {
        if(doctype.back() == 'x')
            this->sampleDocument = new WordParser(new OOXMLParser());
        // else
        //     this->sampleDocument = new WordParser(new CompoundParser());
    }
//     else if(doctype.front() == 'x')
//     {
//         if(doctype.back() == 'x')
//             this->sampleDocument = new ExcelParser(new OOXMLParser());
//         else
//             this->sampleDocument = new ExcelParser(new CompoundParser());
//     }
//    else if(doctype.front() == 'p')
//    {
//         if(doctype.back() == 'x')
//             this->sampleDocument = new PowerPointParser(new OOXMLParser());
//         else
//             this->sampleDocument = new PowerPointParser(new CompoundParser());
//    }
    else
    {
        std::cout << "현재 지원하지 않는 문서 형식입니다." << std::endl;
        return false;
    }
    output = this->sampleDocument->getUrlList(input);
    return true;
}

// CURLExtractEngine의 URL추출 함수
bool CURLExtractEngine::Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output)
{
    std::vector<std::string> urlList;

    // 해당 파일의 정보에 맞게 url을 가져온다.
    if(!urlParsing(input->vecInputFiles[0], extractFileExe(input->vecInputFiles[0]) ,urlList))
        return false;

    // 추출한 주소들을 각각 복사.
    output->vecExtractedUrls.reserve(urlList.size() + output->vecExtractedUrls.size());
    output->vecExtractedUrls.insert(output->vecExtractedUrls.end(), urlList.begin(), urlList.end());
    return true;
}