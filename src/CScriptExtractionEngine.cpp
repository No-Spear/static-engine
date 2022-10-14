#include "CScriptExtractionEngine.h"

// 엔진 객체 생성자
CScriptExtractionEngine::CScriptExtractionEngine() : CEngineSuper(3)
{

}

// 엔진 객체 소멸자
CScriptExtractionEngine::~CScriptExtractionEngine()
{

}

// C&C로부터 다운로드 받은 파일에 대한 정보를 얻기 위한 함수
std::string CScriptExtractionEngine::checkFileType(const std::string fpath)
{
    // 파일 형식자를 찾기위해 마지막 .의 위치를 찾는다.
    int location = fpath.find_last_of('.');
    // 해당 파일형식자를 새로운 스트링에 담는다.
    std::string ftype(fpath.substr(location+1));
    // 해당 파일 형식자를 벡터에 넣어둔다.
    return ftype;
}

// 추출된 스크립트에서 의미 있는 내용만 추출하는 함수
void CScriptExtractionEngine::getMeanfulScript(std::string& script)
{
    std::regex first(R"(<script>[\s]*[\/A-Z\s]*)");
    std::regex last(R"([\s]*</script>)");
    script = std::regex_replace(script, first, "");
    script = std::regex_replace(script, last, "");
}

bool CScriptExtractionEngine::getHtmlScriptData(const char* fpath, std::vector<std::string>& scriptlist)
{
    // 파일 입출력
    std::ifstream html;
    // 파일에 대한 정보를 읽을 string 객체
    std::string buf;
    html.open(fpath, std::ios::in);
    // 해당 파일을 열 수 없다면
    if(!html)
    {
        // 에러를 출력하고 리턴
        std::cerr << "Can't Open Html File" << std::endl;
        return false;
    }
    html.seekg(0, std::ios::end);
	// 그리고 그 위치를 읽는다. (파일의 크기)
	int size = html.tellg();
	// 그 크기의 문자열을 할당한다.
	buf.resize(size);
	// 위치 지정자를 다시 파일 맨 앞으로 옮긴다.
	html.seekg(0, std::ios::beg);
	// 파일 전체 내용을 읽어서 문자열에 저장한다.
	html.read(&buf[0], size);
    html.close();

    // Html문서에서 Script를 추출하기 위한 정규 표현식
    std::regex re(R"(<script>[\s]*[\/A-Za-z"'.,=:;\/_?$()-\[\]\s\\]*[\s]*<\/script>)");
    std::smatch match;

    while (std::regex_search(buf, match, re)) {
        scriptlist.push_back(match.str());
        buf = match.suffix();
    }
    return true;
}

bool CScriptExtractionEngine::getDocmScriptData(const char* fpath)
{
    return true;
}

bool CScriptExtractionEngine::getDotmScriptData(const char* fpath)
{
    return true;
}

// CScriptExtractionEngine의 스크립트 추출 함수
bool CScriptExtractionEngine::Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output)
{
    // 현재 inputfile의 vecfile의 수만큼 반복하여 파일의 형식을 찾는다.
    for(int i =0; i < input->vecInputFiles.size(); i++)
    {
        // 파일 타입에 대한 정보를 받을 변수
        std::string filetype =  checkFileType(input->vecInputFiles[i]);
        // 만약 받은 파일이 html이라면
        if(filetype.compare("html") == 0)
        { 
            getHtmlScriptData(input->vecInputFiles[i].c_str(), output->vecExtractedScript);
            for(int j=0; j < input->vecScriptFIles.size(); j++)
                getMeanfulScript(output->vecExtractedScript[j]);
            return true;
        }
        // 만약 받은 파일이 docm이라면
        else if(filetype.compare("docm") == 0)
        {

            return true;
        }
        // 만약 받은 파일이 dotm이라면
        else if(filetype.compare("dotm") == 0)
        {

            return true;
        }
        // 아무것도 해당하지 않는다면 
        else
            return false;
    }
    return true;
}