#include "CURLExtractionEngine.h"

// 엔진객체 생성자
CURLExtractEngine::CURLExtractEngine() : CEngineSuper(1)
{
    
}

// 엔진객체 소멸자
CURLExtractEngine::~CURLExtractEngine()
{

}

// CURLExtractEngine의 URL추출 함수
bool CURLExtractEngine::Analyze(ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output)
{
    std::vector<std::string> urllist;
    // 입력받은 파일에 대한 정보를 확인한다.
    getDocumentInfo(input->vecInputFiles[0]);
    // 해당 파일의 정보에 맞게 url을 가져온다.
    if(!urlParsing(input->vecInputFiles[0], this->doctype, urllist))
        return false;

    // 추출한 주소들을 각각 복사.
    input->vecURLs.reserve(urllist.size() + input->vecURLs.size());
    input->vecURLs.insert(input->vecURLs.end(), urllist.begin(), urllist.end());
    // 추출한 주소들을 각각 복사.
    output->vecExtractedUrls.reserve(urllist.size() + output->vecExtractedUrls.size());
    output->vecExtractedUrls.insert(output->vecExtractedUrls.end(), urllist.begin(), urllist.end());

    // Copy를 통한 복사(작동안됨)
    // std::copy(urllist.begin(), urllist.end(), input->vecURLs.begin());
    // std::copy(urllist.begin(), urllist.end(), output->vecExtractedUrls.begin());
    // std::cout << "URL Analyze 함수 종료" << std::endl;
    
    // inputfiles에서 파일을 제거
    input->vecInputFiles.pop_back();
    return true;
}

bool CURLExtractEngine::urlParsing(std::string input, std::string doctype, std::vector<std::string>& output)
{
    // 입력받은 파일에 맞는 객체를 생성
    // Docx의 경우
    if(doctype.compare("docx") == 0)
        this->document = new Docx(input.c_str());
    // Xlsx의 경우
    // else if (doctype.compare("xlsx"))
    //     this->document = new Xlsx();

    if(!this->document->getUrlData(output))
        return false;
    
    // 메모리 해제
    free(this->document);
    return true;
}

void CURLExtractEngine::getDocumentInfo(const std::string docpath)
{
    // 파일 형식자를 찾기위해 마지막 .의 위치를 찾는다.
    int location = docpath.find_last_of('.');
    // 해당 파일형식자를 새로운 스트링에 담는다.
    std::string doctype(docpath.substr(location+1));
    // 해당 파일 형식자를 vector에 넣어둔다.
    this->doctype.append(doctype);
}

// Documentation(OOXML) 생성자
OOXml::OOXml(const char* docpath)
{   
    // zip pointer에 파일을 연다.
    this->document = zip_open(docpath, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
}

// Documentation(OOXML) 소멸자
OOXml::~OOXml()
{

}

// 문서에서 C&C 서버 리스트를 돌려준다.
bool OOXml::getUrlData(std::vector<std::string>& output)
{
    // void* buf = NULL;
    // size_t bufsize;

    // // URL을 추출하기 위해 정규표현식을 정의
    // std::regex re(R"(Target[\s]*=[\s]*"[a-zA-Z0-9-_.~!*'();:@&=+$,/?%#\[\]]*"[\s]*TargetMode[\s]*=[\s]*"External")");
    // std::smatch match;

    // // 문서에서 url과 관련된 "XML" 내용을 가져온다.
    // if(zip_entry_open(this->document, contentxml) != 0)
    //     return false;

    // // 문서파일에서 XML 데이터를 buf에 저장한다.
    // if(zip_entry_read(this->document, &buf, &bufsize) < 0)
    //     return false;

    // std::string xml((char*)buf);
    // while (std::regex_search(xml, match, re)) {
    //     // std::cout << match.str() << '\n';
    //     output.push_back(parsing(match.str()));
    //     xml = match.suffix();
    // }
    return true;
}

// 전달받은 문자열에서 URL만 추출하는 함수
std::string OOXml::parsing(std::string input)
{
    std::istringstream iss(input);
    std::string buffer;
    // vector를 통해 넣을 수 있지만 넣고 빼는 과정에 
    // 오버해드를 생각해 해당 과정으로 처리
    // Target="을 처리
    getline(iss, buffer, '"');
    // URL을 추출
    getline(iss, buffer, '"');
    // 마지막에 !가 있다면 제거
    if(buffer.back() == '!')
        buffer.pop_back();
    return buffer;
}

// Docx객체 생성자
Docx::Docx(const char* docpath) : OOXml(docpath)
{
    // 부모의 생성자를 호출 이후
    // Docx XML파일의 위치 삽입
    this->contentxml = "word/_rels/document.xml.rels";    
}

// Docx객체 소멸자
Docx::~Docx()
{

}

bool Docx::getUrlData(std::vector<std::string>& output)
{
    void* buf = NULL;
    size_t bufsize;

    // URL을 추출하기 위해 정규표현식을 정의
    std::regex re(R"(Target[\s]*=[\s]*"[a-zA-Z0-9-_.~!*'();:@&=+$,/?%#\[\]]*"[\s]*TargetMode[\s]*=[\s]*"External")");
    std::smatch match;

    // 문서에서 url과 관련된 "XML" 내용을 가져온다.
    if(zip_entry_open(this->document, contentxml) != 0)
        return false;

    // 문서파일에서 XML 데이터를 buf에 저장한다.
    if(zip_entry_read(this->document, &buf, &bufsize) < 0)
        return false;

    std::string xml((char*)buf);
    while (std::regex_search(xml, match, re)) {
        // std::cout << match.str() << '\n';
        output.push_back(parsing(match.str()));
        xml = match.suffix();
    }
    return true;
}