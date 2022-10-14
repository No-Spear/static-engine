#pragma once
#include <iostream>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <regex>

#include "CEngineSuper.h"

class CScriptExtractionEngine : public CEngineSuper {
private:
    
    std::string checkFileType(const std::string fpath);                                 // 파일의 타입을 확인하는 함수

    void getMeanfulScript(std::string& script);                                         // 의미없는 문자가 제거된 스크립트를 만들어주는 함수

    bool getHtmlScriptData(const char* fpath, std::vector<std::string>& scriptlist);    // HTML문서에서 스크립트 내용 추출하는 함수

    bool getDocmScriptData(const char* fpath);                                          // Docm 파일에서 스크립트 내용을 추출하는 함수

    bool getDotmScriptData(const char* fpath);                                          // Dotm 파일에서 스크립트 내용을 추출하는 함수

public:
    CScriptExtractionEngine();                                          // 클래스 생성자
    ~CScriptExtractionEngine();                                         // 클래스 소멸자
    bool Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output);   // 검사 함수
};