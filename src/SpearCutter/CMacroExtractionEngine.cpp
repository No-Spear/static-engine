#include "CMacroExtractionEngine.h"

// 매크로가 있는지 확인하고 추출하는 엔진의 생성자
CMacroExtractionEngine::CMacroExtractionEngine() : CEngineSuper(5, "MacroExtraction")
{

}

// 매크로가 있는지 확인하고 추출하는 엔진의 소멸자
CMacroExtractionEngine::~CMacroExtractionEngine()
{

}

// 문서의 위치로부터 문서 타입을 가져오기 위한 함수
std::string CMacroExtractionEngine::extractFileExetoPath(const std::string docpath)
{
    // 파일 형식자를 찾기위해 마지막 .의 위치를 찾는다.
    int location = docpath.find_last_of('.');
    // 해당 파일형식자를 새로운 스트링에 담는다.
    std::string doctype(docpath.substr(location+1));
    // 해당 파일 형식자를 vector에 넣어둔다.
    return doctype;
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
    std::regex useless(R"(XLMMacroDeobfuscator: pywin32 is not installed \(only is required if you want to use MS Excel\)\nolevba [0-9.]* on Python [0-9.]* - http:\/\/decalage.info\/python\/oletools)");
    macroData = std::regex_replace(macroData, useless, "");

    // 매크로 정재 및 분리
    int curr;
    int prev = 0;
    curr = macroData.find("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ");
    while(curr != std::string::npos)
    {
        if(prev == 0)
        {
            prev = curr +1;
            curr = macroData.find("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ", prev);
            continue;
        }
        std::string split = macroData.substr(prev, curr-prev);
        getMeanFulMacroData(split);
        scriptlist.push_back(std::make_pair(split, std::make_pair(-1, VBS)));
        prev = curr +1;
        curr = macroData.find("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ", prev);
    }
    std::string lastsplit = macroData.substr(prev, curr-prev);
    getMeanFulMacroData(lastsplit);
    lastsplit.pop_back();
    lastsplit.pop_back();
    lastsplit.pop_back();
    scriptlist.push_back(std::make_pair(lastsplit, std::make_pair(-1, VBS)));

    return true;
}

void CMacroExtractionEngine::getMeanFulMacroData(std::string& script)
{
    // olevba 결과 출력 정제용
    // - - - 를 제거하기 위한 정규표현식
    std::regex first(R"(\s?(- ){38}\s?)");
    std::regex second(R"(\n?\n?(-------------------------------------------------------------------------------[\w\s:./'-]*))");
    std::regex last(R"([&()]|(,\d)|["])");
    script = std::regex_replace(script, first, "");
    script = std::regex_replace(script, second, "");
    script = std::regex_replace(script, last, "");
    // 스트링 객체 사이즈 조정
    script.resize(script.size());
    // 메모리 재할당을 통한 낭비 메모리 제거
    script.shrink_to_fit();
}

bool CMacroExtractionEngine::Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output)
{
    std::string macroData;
    std::string pszFile = input->vecInputFiles[0];

    if(extractFileExetoPath(pszFile).front() == 'd' 
      | extractFileExetoPath(pszFile).front() == 'D'
      | extractFileExetoPath(pszFile).front() == 'x'
      | extractFileExetoPath(pszFile).front() == 'X'
      | extractFileExetoPath(pszFile).front() == 'p'
      | extractFileExetoPath(pszFile).front() == 'P' 
      )
        getMacroDataFromFile(pszFile.c_str(), output->vecExtractedScript);
    else
        throw engine_Exception("MacroExtraction", "s", "매크로 추출을 지원하지않는 형식의 문서 입니다.");

    return true;
}