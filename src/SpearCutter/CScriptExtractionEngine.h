#pragma once
#include "CEngineSuper.h"
#include "CException.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <regex>

class CScriptExtractionEngine : public CEngineSuper {
private:
    
    std::string checkFileType(const std::string fpath);                                                                         // 파일의 타입을 확인하는 함수
    bool checkFileDownloadStatus(const std::string fpath, int i);                                                               // 다운받아온 파일에 대한 상태를 확인하는 함수
    void getMeanfulScript(std::string& script, std::string type);                                                               // 의미없는 문자가 제거된 스크립트를 만들어주는 함수
    bool getHtmlScriptData(const char* fpath, int i, std::vector<std::pair<std::string, std::pair<int, int>> >& scriptlist);    // HTML문서에서 스크립트 내용 추출하는 함수
    int getHtmlScriptType(const std::string scriptData);                                                                        // HTML문서에서 스크립트의 형식을 추출하는 함수
    bool getMacroSciptData(const char* fpath, int i, std::vector<std::pair<std::string, std::pair<int, int>> >& scriptlist);    // 매크로 사용 서식파일, 매크로 사용 템플릿 파일에서 스크립트 내용을 추출하는 함수

public:
    CScriptExtractionEngine();                                                                                                  // 클래스 생성자
    ~CScriptExtractionEngine();                                                                                                 // 클래스 소멸자
    bool Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output);                                                     // 검사 함수
};