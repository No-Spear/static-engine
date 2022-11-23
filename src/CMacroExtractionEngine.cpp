#include "CMacroExtractionEngine.h"

// 매크로가 있는지 확인하고 추출하는 엔진의 생성자
CMacroExtractionEngine::CMacroExtractionEngine() : CEngineSuper(4, "MacroExtraction")
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

// 문서에서 파일의 내용에 접근할 수 있는지 확인하는 함수
bool CMacroExtractionEngine::open(const char* pszFile)
{
    // OOXML 객체를 ZIP 파일로 변환하여 Open 
    this->macroFile = zip_open(pszFile, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
    if(this->macroFile == NULL) 
        throw engine_Exception("MacroExtraction", "s", "분석을 의로한 파일을 확인할 수 없습니다.");

    return true;
}

// 분석을 의뢰한 파일에서 매크로 파일을 가져오는 함수
std::string CMacroExtractionEngine::getMacroDataFromFile(const char* location)
{
    // OOXML 객체의 상위, 하위 Stream 데이터를 얻어온다.
    if(zip_entry_open(this->macroFile, location) != 0) 
        throw engine_Exception("MacroExtraction", "s", "전달 받은 위치의 파일을 열 수 없습니다.");

    // 문서파일에서 매크로 파일을 엔진에 저장한다..
    if(zip_entry_fread(this->macroFile, "vbaProject.bin") < 0) 
        throw engine_Exception("MacroExtraction", "s", "매크로 파일이 있는 파일의 내용을 확인할 수 없습니다.");
    
    // python의 olevba를 통해 vbaProject.bin에서 vba파일을 가져온다.
    int ret = system("olevba vbaProject.bin --decode -c >> result.log");
    if(ret != 0)
        throw engine_Exception("MacroExtractio","s","OleVBA를 통해 vba코드를 추출할 수 없습니다.");

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

    // 추출된 파일을 전부 읽은 후 삭제한다.
    if(remove("./vbaProject.bin") != 0)
        throw engine_Exception("MacroExtractio","s","추출된 매크로 파일을 삭제할 수 없습니다.");
    
    // 추출된 vba결과를 전부 읽은 후 삭제한다.
    if(remove("./result.log") != 0)
        throw engine_Exception("MacroExtractio","s","추출된 매크로 파일을 삭제할 수 없습니다.");
    return macroData;
}

bool CMacroExtractionEngine::Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output)
{
    std::string macroData;
    std::string location;
    switch (extractFileExetoPath(input->vecInputFiles[0].first).front())
    {
    case 'd':
        open(input->vecInputFiles[0].first.c_str());
        location.append("word/vbaProject.bin");
        macroData = (std::string)getMacroDataFromFile(location.c_str());
        break;

    case 'x':
        open(input->vecInputFiles[0].first.c_str());
        location.append("xl/vbaProject.bin");
        macroData = (std::string)getMacroDataFromFile(location.c_str());
        break;

    case 'p':
        open(input->vecInputFiles[0].first.c_str());
        location.append("ppt/vbaProject.bin");
        macroData = (std::string)getMacroDataFromFile(location.c_str());
        break;

    default:
        throw engine_Exception("MacroExtraction", "s", "매크로 추출을 지원하지않는 형식의 문서 입니다.");
    }
 
    // 출력에 다운로드된 url의 위치는 -1로 안쓰는 숫자를 사용한다.
    // 추출된 파일에 대한 데이터를 분석엔진에 넘겨준다.
    output->vecExtractedScript.push_back(std::make_pair(macroData, std::make_pair(-1, VBS)));
    return true;
}