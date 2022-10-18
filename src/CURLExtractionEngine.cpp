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
bool OOXMLParser::open(const char* pszFile)
{    
    // OOXML 객체를 ZIP 파일로 변환하여 Open 
    this->OOXML = zip_open(pszFile, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
    if(this->OOXML == NULL)
    {
        std::cout << "분석을 의뢰한 파일을 열 수 없습니다." << std::endl;
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
char* OOXMLParser::getStreamData(const char* location)
{   
    // OOXML 객체의 Contentxml 데이터를 얻어온다.
    if(zip_entry_open(this->OOXML, location) != 0)
    {
        std::cout << "파일을 열 수 없습니다." << std::endl;
        return NULL;
    }

    // 문서파일에서 하위 XML 데이터를 buf에 저장한다.
    if(zip_entry_read(this->OOXML, &buffer, &bufsize) < 0)
    {
        std::cout << "Cotent Xml 파일 내용을 열 수 없습니다." << std::endl;
        return NULL;
    }
    return (char*)this->buffer;
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


// 문서파일을 열고 문서파일의 스트림 데이터에서 Url을 추출하고 돌려주는 함수
std::vector<std::string> WordParser::getUrlList(std::string samplePath)
{
    // 문서파일의 스트림 데이터를 받을 포인터
    char* buffer;
    // 문서파일의 스트림 데이터서 뽑은 Url을 받을 벡터
    std::vector<std::string> UrlList;

    std::regex re(R"(<Relationship[\s]*Id=[\s]*"[A-Za-z0-9]*"[\s]*Type[\s]*=[\s]*"[A-Za-z0-9-:/.]*\/oleObject"[\s]*Target[\s]*=[\s]*"[a-zA-Z0-9-_.~!*'();:@&=+$,/?%#]*"[\s]*TargetMode[\s]*=[\s]*"External")");
    std::smatch match;

    // Word 문서의 contentxml의 정의
    const char* contentxml = "word/_rels/document.xml.rels";

    // 문서파일의 스트림 데이터를 열수 있는지 확인.
    if(!this->container->open(samplePath.c_str()))
        return UrlList;
    
    buffer = (char*)this->container->getStreamData(contentxml);

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

std::string ExcelParser::parsingContentxml(const std::string input)
{
    std::istringstream iss(input);
    std::string buffer;
    // Target="worksheets. 제거
    getline(iss, buffer, '/');
    getline(iss, buffer, '"');

    return buffer;
}

std::vector<std::string> ExcelParser::getContenxmlList(const char* highStream)
{
    std::vector<std::string> contentxmlList;
    // stream data를 string으로 변환
    std::string streamData(highStream);
    
    std::smatch match;
    std::regex re(R"(Target[\s]*=[\s]*"worksheets\/[A-Za-z0-9]*.xml)");
     while (std::regex_search(streamData, match, re)) { 
        contentxmlList.push_back("xl/worksheets/"+ parsingContentxml(match.str())+".rels");
        streamData = match.suffix();
    }
    return contentxmlList;
}   

// 문서파일을 열고 문서파일의 스트림 데이터에서 Url을 추출하고 돌려주는 함수
std::vector<std::string> ExcelParser::getUrlList(std::string samplePath)
{
    // 문서파일의 스트림 데이터를 받을 포인터
    char* buffer;
    // 문서파일의 스트림 데이터서 뽑은 Url을 받을 벡터
    std::vector<std::string> urlList;

    std::regex re(R"(<Relationship[\s]*Id=[\s]*"[A-Za-z0-9]*"[\s]*Type[\s]*=[\s]*"[A-Za-z0-9-:/.]*\/oleObject"[\s]*Target[\s]*=[\s]*"[a-zA-Z0-9-_.~!*'();:@&=+$,/?%#]*"[\s]*TargetMode[\s]*=[\s]*"External")");
    std::smatch match;

    //문서파일이 해당 위치에 있는지 확인
    if(!this->container->open(samplePath.c_str()))
        return urlList;
    
    // 상위 스트림에 대한 정보를 가져온다.
    char* highStream = this->container->getStreamData("xl/_rels/workbook.xml.rels");
    // Excel 문서의 상위 stream에서 contentxml 리스트를 가져온다.
    std::vector<std::string> contentxml = getContenxmlList(highStream);

    // contentxml의 수 만큼 url을 찾아낸다.
    for(int i =0; i< contentxml.size(); i++)
    {
        buffer = this->container->getStreamData(contentxml[i].c_str());
        // 만약 해당 contentxml이 없다면 종료
        if(buffer == NULL)
            return urlList;
        // 문서파일의 스트림 데이터에서 OleObject의 Url만 뽑아 낸다.
        std::string xml(buffer);
        while (std::regex_search(xml, match, re)) {
            urlList.push_back(parsingUrl(match.str()));
            xml = match.suffix();
        }
    }
    return urlList;
}

// PowerPoint 문서 형식에 대한 생성자
PowerPointParser::PowerPointParser(ContainerParserSuper* pContainer) : DocumentParserSuper(pContainer)
{

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

std::vector<std::string> PowerPointParser::getContenxmlList(const char* highStream)
{
    std::vector<std::string> contentxmlList;
    // stream data를 string으로 변환
    std::string streamData(highStream);
    
    std::smatch match;
    std::regex re(R"(slide[0-9]*.xml)");
     while (std::regex_search(streamData, match, re)) {
        contentxmlList.push_back("ppt/slides/_rels/"+ match.str()+".rels");
        streamData = match.suffix();
    }
    return contentxmlList;
}   

// 문서파일을 열고 문서파일의 스트림 데이터에서 Url을 추출하고 돌려주는 함수
std::vector<std::string> PowerPointParser::getUrlList(std::string samplePath)
{
    // 문서파일의 스트림 데이터를 받을 포인터
    char* buffer;
    // 문서파일의 스트림 데이터서 뽑은 Url을 받을 벡터
    std::vector<std::string> urlList;

    std::regex re(R"(<Relationship[\s]*Id=[\s]*"[A-Za-z0-9]*"[\s]*Type[\s]*=[\s]*"[A-Za-z0-9-:/.]*\/oleObject"[\s]*Target[\s]*=[\s]*"[a-zA-Z0-9-_.~!*'();:@&=+$,/?%#]*"[\s]*TargetMode[\s]*=[\s]*"External")");
    std::smatch match;

    //문서파일이 해당 위치에 있는지 확인
    if(!this->container->open(samplePath.c_str()))
        return urlList;
    
    // 상위 스트림에 대한 정보를 가져온다.
    char* highStream = this->container->getStreamData("ppt/_rels/presentation.xml.rels");
    // PowerPoint 문서의 상위 stream에서  contentxml 리스트를 가져온다.
    std::vector<std::string> contentxml = getContenxmlList(highStream);

    // contentxml의 수 만큼 url을 찾아낸다.
    for(int i =0; i< contentxml.size(); i++)
    {
        buffer = this->container->getStreamData(contentxml[i].c_str());
        // 만약 contentxml 파일이 없을 경우 종료
        if(buffer == NULL)
            return urlList;
        // 문서파일의 스트림 데이터에서 OleObject의 Url만 뽑아 낸다.
        std::string xml(buffer);
        while (std::regex_search(xml, match, re)) {
            urlList.push_back(parsingUrl(match.str()));
            xml = match.suffix();
        }
    }
    return urlList;
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
std::string CURLExtractEngine::extractFileExetoPath(const std::string docpath)
{
    // 파일 형식자를 찾기위해 마지막 .의 위치를 찾는다.
    int location = docpath.find_last_of('.');
    // 해당 파일형식자를 새로운 스트링에 담는다.
    std::string doctype(docpath.substr(location+1));
    // 해당 파일 형식자를 vector에 넣어둔다.
    return doctype;
}

// 문서의 시그니처로부터 문서 타입을 가져오는 함수
std::string CURLExtractEngine::extractFileExetoSignature(const std::string docpath)
{
    std::string signature;
    std::stringstream signaturestream;
    // 분석의뢰파일을 바이너리 형태로 데이터를 읽는다.
    std::ifstream document(docpath, std::ios::binary);

    // 만약 파일을 열지 못했다면
    if (document.fail())
	{
		std::cout << "파일을 열고 파일의 시그니처를 알 수 없습니다" << std::endl;
		return signature;
	}

    // 파일의 시그니처 값에 해당하는 4byte를 읽어온다.
    for(int i =0; i<4; i++)
    {
        int sig = document.get();
        signaturestream << std::hex << sig;
    }
    signature = signaturestream.str();
    return signature;
}                      


bool CURLExtractEngine::urlParsing(std::string input, std::string doctype, std::vector<std::string>& output)
{
    // 문서의 시그니처를 확인 
    std::string documentSignature = extractFileExetoSignature(input);
    
    // 입력받은 파일에 맞는 객체를 생성
    // Word 파일의 경우
    if(doctype.front() == 'd')
    {
        if(documentSignature.compare("504b34") == 0)
            this->sampleDocument = new WordParser(new OOXMLParser());
        // else
        //     this->sampleDocument = new WordParser(new CompoundParser());
    }
    else if(doctype.front() == 'x')
    {
        if(documentSignature.compare("504b34") == 0)
            this->sampleDocument = new ExcelParser(new OOXMLParser());
        // else
        //     this->sampleDocument = new ExcelParser(new CompoundParser());
    }
   else if(doctype.front() == 'p')
   {
        if(documentSignature.compare("504b34") == 0)
            this->sampleDocument = new PowerPointParser(new OOXMLParser());
        // else
        //     this->sampleDocument = new PowerPointParser(new CompoundParser());
   }
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
    if(!urlParsing(input->vecInputFiles[0].first, extractFileExetoPath(input->vecInputFiles[0].first) ,urlList))
        return false;

    // 추출한 주소들을 각각 복사.
    output->vecExtractedUrls.reserve(urlList.size() + output->vecExtractedUrls.size());
    output->vecExtractedUrls.insert(output->vecExtractedUrls.end(), urlList.begin(), urlList.end());
    return true;
}