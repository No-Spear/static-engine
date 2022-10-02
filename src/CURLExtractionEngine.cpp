#include "CURLExtractionEngine.h"

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

// 문서의 타입을 반환
std::string OOXml::getDocType()
{
    return this->doctype;
}

// 문서에서 C&C 서버 리스트를 돌려준다.
bool OOXml::getUrlData(std::vector<std::string>& output)
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

// 엔진객체 생성자
CURLExtractEngine::CURLExtractEngine()
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
    for (int i =0; i < input->vecInputFiles.size(); i++)
    {
        if(!urlParsing(input->vecInputFiles[i], urllist))
            return false;
    }

    // 추출한 주소들을 각각 복사.
    input->vecURLs.reserve(urllist.size() + input->vecURLs.size());
    input->vecURLs.insert(input->vecURLs.end(), urllist.begin(), urllist.end());
    // 추출한 주소들을 각각 복사.
    output->vecExtractedUrls.reserve(urllist.size() + output->vecExtractedUrls.size());
    output->vecExtractedUrls.insert(output->vecExtractedUrls.end(), urllist.begin(), urllist.end());

    // Copy를 통한 복사(작동안됨)
    // std::copy(urllist.begin(), urllist.end(), input->vecURLs.begin());
    // std::copy(urllist.begin(), urllist.end(), output->vecExtractedUrls.begin());
    return true;
}

bool CURLExtractEngine::urlParsing(std::string input, std::vector<std::string>& output)
{
    // 입력받은 파일의 객체를 생성
    this->document = new OOXml(input.c_str());
    if(!this->document->getUrlData(output))
        return false;
    
    // OOXML 객체 소멸자 호출.
    this->document->~OOXml();
    return true;
}