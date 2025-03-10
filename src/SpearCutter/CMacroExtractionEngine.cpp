#include "CMacroExtractionEngine.h"
#include "HelpFunc.h"

// 매크로가 있는지 확인하고 추출하는 엔진의 생성자
CMacroExtractionEngine::CMacroExtractionEngine() : CEngineSuper(5, "MacroExtraction")
{

}

// 매크로가 있는지 확인하고 추출하는 엔진의 소멸자
CMacroExtractionEngine::~CMacroExtractionEngine()
{

}


// 분석을 의뢰한 파일에서 매크로 파일을 가져오는 함수
bool CMacroExtractionEngine::getMacroDataFromFile(const char* location, std::vector<std::pair<std::string, std::pair<int, int>> >& scriptlist)
{   
    std::string olevba;
    std::regex space(R"( )");
    std::regex leftbracket(R"(\()");
    std::regex rightbracket(R"(\))");
    std::regex andName(R"(\&)");
    olevba.append("olevba ");
    std::string unspace = std::regex_replace(location, space, "\\ ");
    std::string unleft = std::regex_replace(unspace, leftbracket, "\\(");
    std::string unright = std::regex_replace(unleft, rightbracket, "\\)");
    olevba.append(std::regex_replace(unright, andName, "\\&"));
    olevba.append(" --decode -c >> result.log");

    // python의 olevba를 통해 vbaProject.bin에서 vba파일을 가져온다.
    int ret = system(olevba.c_str());
    if(ret == 1280)
    {
        remove("./result.log");
        throw engine_Exception("MacroExtractio","s","OleVBA를 통해 vba코드를 추출할 수 없습니다.");
    }
    // 추출된 파일을 읽는다.
    std::ifstream macroFile("./result.log");
    
    // 만약 파일을 열기가 실패했다면
    if(macroFile.fail())
        throw engine_Exception("MacroExtractio","s","추출된 매크로 파일을 열 수 없습니다.");
    
    // 읽을 파일에 대한 사이즈를 구하기 위해 파일의 맨끝으로 이동한다.
    macroFile.seekg(0,std::ios::end);
    int fileSize = macroFile.tellg();
    // 파일을 다시 맨 처음으로 이동시킨다.
    macroFile.seekg(0, std::ios::beg);
    // 파일에 대한 내용을 받을 스트링 객체 생성 및 사이즈 조정
    std::string macroData;

    macroData.resize(fileSize);
    macroFile.read(&macroData[0], fileSize);

    // 추출된 vba결과를 전부 읽은 후 삭제한다.
    if(remove("./result.log") != 0)
        throw engine_Exception("MacroExtractio","s","추출된 매크로 파일을 삭제할 수 없습니다.");

    // 시작에 나오는 내용을 제거하기 위한 정규표현식
    std::regex removeFrontPywinRe(R"(XLMMacroDeobfuscator: pywin[0-9]{2} is not installed \(only is required if you want to use MS Excel\))");
    std::regex removeFrontOlevbaRe(R"([\s]olevba [0-9]*.[0-9]*.[0-9]* on Python [0-9]*.[0-9]*.[0-9]* - http:\/\/decalage.info\/python\/oletools)");
    std::regex removeFrontEqualRe(R"([\s]={79}[\s]*)");
    std::regex removeTypeRe(R"(Type: (OpenXML|OLE))");
    macroData = std::regex_replace(macroData, removeFrontPywinRe, "");
    macroData = std::regex_replace(macroData, removeFrontOlevbaRe, "");
    macroData = std::regex_replace(macroData, removeFrontEqualRe, "");
    macroData = std::regex_replace(macroData, removeTypeRe, "");
    
    std::smatch match;
    std::regex emptyRe(R"(\(empty macro\))");
    // 매크로 정재 및 분리
    // 지금의 방식은 어떠한 매크로인지 확인이 가능하다.
    int curr;
    int prev = 0;
    curr = macroData.find("-------------------------------------------------------------------------------");
    while(curr != std::string::npos)
    {
        if(prev == 0)
        {
            prev = curr +1;
            curr = macroData.find("-------------------------------------------------------------------------------", prev);
            continue;
        }
        std::string split = macroData.substr(prev, curr-prev);
        // (empty macro) 이면 저장하지 않고 넘어간다.
        if(std::regex_search(split, match, emptyRe))
        {
            prev = curr +1;
            curr = macroData.find("-------------------------------------------------------------------------------", prev);
            continue;
        }
        
        // 매크로 스크립트 정제
        getMeanFulMacroData(split);
        // 매크로 스크립트를 저장한다.
        scriptlist.push_back(std::make_pair(split, std::make_pair(-1, VBS)));

        prev = curr +1;
        curr = macroData.find("-------------------------------------------------------------------------------", prev);
    }
    std::string lastsplit = macroData.substr(prev, curr-prev);
    // 마지막 매크로 스크립트 정제
    getMeanFulMacroData(lastsplit);
    // 마지막 매크로 스크립트 저장
    scriptlist.push_back(std::make_pair(lastsplit, std::make_pair(-1, VBS)));

    return true;
}

void CMacroExtractionEngine::getMeanFulMacroData(std::string& script)
{
    // Sbyte를 위해 사용하는 match
    std::smatch match;

    // """,숫자) 또는 & 또는 " & "을 제거하는 정규식
    std::regex trashRe(R"(("""",[\d]*\)\&)|(&)|(" & "))");
    std::regex splitRe(R"(------------------------------------------------------------------------------)");
    script = std::regex_replace(script, trashRe, "");
    script = std::regex_replace(script, splitRe, "");
    // 만약 sByte가 있다면 base64로 디코딩 해본다.
    std::regex sBytes(R"(sBytes = )");
    if(std::regex_search(script, match, sBytes))
    {
        //" sBytes = sByte  "를 모두 제거
        std::regex removesByte(R"("\nsBytes = sBytes  ")");
        script = std::regex_replace(script, removesByte, "");

        // 제거된 내용에서 sByte에 해당하는 내용을 찾아낸다.
        std::regex Encode(R"(sBytes = "[\w]*")");
        std::regex_search(script, match, Encode);

        std::string sbyte = match.str();
        // sByte = 을 제거
        sbyte.erase(0,10);
        sbyte = base64_decoder(sbyte);

        script = std::regex_replace(script, Encode, sbyte);
    }
    
    // 스트링 객체 사이즈 조정
    script.resize(script.size());
    // 메모리 재할당을 통한 낭비 메모리 제거
    script.shrink_to_fit();
}

bool CMacroExtractionEngine::Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output)
{
    std::string pszFile = input->vecInputFiles[0];

    /*
     * 엔진이 동작하기 전에 먼저 파일이 문서가 맞는지 확인하므로
     * 추가로 문서파일이 맞는지 확인할 필요가 없다.
    */ 
    getMacroDataFromFile(pszFile.c_str(), output->vecExtractedScript);

    return true;
}