#include "CScriptExtractionEngine.h"

// 엔진 객체 생성자
CScriptExtractionEngine::CScriptExtractionEngine() : CEngineSuper(3,"ScriptExtraction")
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

// C&C 서버에서 다운받아온 파일에 대한 상태를 확인하는 함수
bool CScriptExtractionEngine::checkFileDownloadStatus(const std::string fpath, int i)
{
    // <title> 4XX or 5XX를 잡기 위한 정규 표현식
    std::regex re("(<title>(4|5)[0-9]{2})");
    std::smatch match;
    
    // 다운받은 파일을 검증하기 위해 연다.
    std::ifstream file;
    // 파일에 대한 데이터를 받을 버퍼
    std::string buf;
    file.open(fpath, std::ios::in);
    // 만약 파일을 열기 싪패했다면
    if(!file)
        throw engine_Exception("ScriptExtraction", "sis", "DownloadEngine에서 받아온", i ,"번째 파일이 정상적으로 다운로드되지 못했습니다.");

    // 파일의 마지막 위치로 이동한다.
    file.seekg(0, std::ios::end);
    // 파일의 크기를 얻어온다.
    int size = file.tellg();
    // 스트링 객체의 크기를 재조정한다.
    buf.resize(size);
    // 파일을 다시 맨앞으로 이동한다.
    file.seekg(0, std::ios::beg);

    // 파일에서 사이즈(200)만큼 데이터를 읽는다.
    file.read(&buf[0], size);
    file.close();
    if(std::regex_search(buf, match, re))
        throw engine_Exception("ScriptExtraction", "sis", "DownloadEngine에서 받아온", i ,"번째 파일이 정상적으로 다운로드되지 못했습니다.");
    return true;
}

// 추출된 스크립트에서 의미 있는 내용만 추출하는 함수
void CScriptExtractionEngine::getMeanfulScript(std::string& script)
{
    std::regex first(R"(<script>[\s]*[\/A-Z\s]*)");
    std::regex last(R"((\/\/[A-Za-z]*)?[\s]*</script>)");
    script = std::regex_replace(script, first, "");
    script = std::regex_replace(script, last, "");
    // 스트링 객체 사이즈 조정
    script.resize(script.size());
    // 메모리 재할당을 통한 낭비 메모리 제거
    script.shrink_to_fit();
}

bool CScriptExtractionEngine::getHtmlScriptData(const char* fpath, int i, std::vector<std::pair<std::string, std::pair<int, int>> >& scriptlist)
{
    // 파일 입출력
    std::ifstream html;
    // 파일에 대한 정보를 읽을 string 객체
    std::string buf;
    html.open(fpath, std::ios::in);
    // 해당 파일을 열 수 없다면
    if(!html)
        throw engine_Exception("ScriptExtraction", "s", "다운받은 html 파일을 열 수 없습니다.");
    
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
    
    // 전달받은 html파일에서 스크립트 형태를 확인하고
    // Html문서에서 Script를 추출하기 위한 정규 표현식
    std::regex re(R"(<SCRIPT LANGUAGE="VBScript">[\s]*[A-Za-z0-9\s-!()=><%*&,.;:'"|#~`_]*<\/SCRIPT>|<script>[\s]*[\/A-Za-z"'.,=:;\/_?$()-\[\]\s\\]*[\s]*<\/script>)");
    std::smatch match;

    while (std::regex_search(buf, match, re)) {
        scriptlist.push_back(std::make_pair(match.str(), std::make_pair(i, getHtmlScriptType(match.str()))));
        buf = match.suffix();
    }
    if(scriptlist.empty())
        throw engine_Exception("ScriptExtraction", "s", "스크립트엔진에서 추출된 스크립트가 없습니다.");
    return true;
}

int CScriptExtractionEngine::getHtmlScriptType(const std::string scriptData)
{
    // 1차로 문서의 타입을 가지고 있는지 확인하기 위한 정규 표현식
    std::regex re(R"((LANGUAGE|language)[\s]*=[\s]*"VBScript"")");
    std::smatch match;
    if(std::regex_search(scriptData, match, re))
        return VBS;
    else
        return JS;
}

bool CScriptExtractionEngine::getMacroSciptData(const char* fpath, int i, std::vector<std::pair<std::string, std::pair<int, int>> >& scriptlist)
{
    // 파일 형식에 따른 매크로 파일의 위치
    std::string location;

    // 다운 받은 파일에 대한 형식에 따른 매크로 추출 위치를 정해준다.
    switch (checkFileType(fpath).front())
    {
    case 'd':
        location.append("word/vbaProject.bin");
        break;
    case 'x':
        location.append("xl/vbaProject.bin");
        break;
    case 'p':
        location.append("ppt/vbaProject.bin");
        break;
    default:
        throw engine_Exception("SciptExtraction", "s", "매크로 추출을 지원하지않는 형식의 문서 입니다.");
    }

    // 해당 파일의 데이터를 접근하기 위한 zip_t 구조체    
    zip_t* downloadFile;
    downloadFile = zip_open(fpath, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
    
    // 만약 다운받은 파일을 열 수 없다면
    if(downloadFile == NULL)
        throw engine_Exception("ScriptExtraction", "s", "다운받은 파일을 열 수 없습니다.");
    
    if(zip_entry_open(downloadFile, location.c_str()) != 0)
        throw engine_Exception("ScriptExtraction", "s", "다운받은 파일의 매크로 파일의 전달 받은 위치의 파일을 열 수 없습니다.");

    // 문서파일에서 매크로 파일을 엔진에 저장한다..
    if(zip_entry_fread(downloadFile, "vbaProject.bin") < 0) 
        throw engine_Exception("ScriptExtraction", "s", "다운받은 파일의 매크로 파일의 내용을 확인할 수 없습니다.");
    
    // 추출된 파일을 바이너리 형식으로 읽는다.
    std::ifstream macroFile("./vbaProject.bin", std::ios::binary);
    // 만약 파일을 열기가 실패했다면
    if(macroFile.fail())
        throw engine_Exception("ScriptExtraction", "s","다운받은 파일에서 추출된 매크로 파일을 열 수 없습니다.");
    
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
        throw engine_Exception("MacroExtractio","s","다운받은 파일에서 추출된 매크로 파일을 삭제할 수 없습니다.");
    
    // 추출된 매크로 데이터를 스크립트 리스트에 집어 넣는다.
    // 스크립트 데이터, url의 위치, 추출된 스크립트 타입
    // 순으로 넣는다.
    scriptlist.push_back(std::make_pair(macroData, std::make_pair(i, VBS)));

    return true;
}

// CScriptExtractionEngine의 스크립트 추출 함수
bool CScriptExtractionEngine::Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output)
{
    // 현재 inputfile의 vecfile의 수만큼 반복하여 파일의 형식을 찾는다.
    for(int i =0; i < input->vecInputFiles.size(); i++)
    {
        // 받아온 파일에 대해 1차적으로 검증
        try{
            // 만약 다운로드 엔진에서 다운로드 된 파일이 없을 경우
            if(input->vecInputFiles[i].first == "../temp")
                throw engine_Exception("ScriptExtraction", "isss", i, "번째 Url인 ", input->vecURLs[i].c_str(), "에서 다운로드 된 파일이 없습니다.");

            checkFileDownloadStatus(input->vecInputFiles[i].first, i);
        } catch(std::exception& e)
        {
            std::cout << e.what() << std::endl;
            continue;
        }
        // 파일 타입에 대한 정보를 받을 변수
        std::string filetype =  checkFileType(input->vecInputFiles[i].first);
        // 만약 받은 파일이 html이라면
        if(filetype == "html")
        { 
            // html 파일에서 script와 스크립트 타입을 확인한다.
            getHtmlScriptData(input->vecInputFiles[i].first.c_str(), i, output->vecExtractedScript);
            // 추출된 스크립트에서 의미 있는 스크립트만 추출한다.
            getMeanfulScript(output->vecExtractedScript[i].first);
            return true;
        }
        // 만약 받은 파일이 docm, xlsm, ppsm(매크로 탑제 파일)), otm, xltm, pptm(템플릿 파일)
        else if(filetype == "docm" | filetype == "xlsm" | filetype == "ppsm" |
                filetype == "dotm" | filetype == "xltm" | filetype == "pptm")
        {
            getMacroSciptData(input->vecInputFiles[i].first.c_str(), i, output->vecExtractedScript);
            return true;
        }
        // 아무것도 해당하지 않는다면 
        else
            throw engine_Exception("ScriptExtraction", "ss", filetype, "형식은 현재 스크립트를 추출엔진에서 지원하지 않는 파일입니다.");
    }
    
    // 만약 추출된 스크립트가 아무것도 없다면 에러 발생
    if(output->vecExtractedScript.empty())
        throw engine_Exception("ScriptExtraction", "s", "엔진에서 추출된 스크립트 파일이 아무것도 없습니다.");
    return true;
}