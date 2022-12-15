#pragma once
#include "CEngineSuper.h"
#include "CException.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <regex>

class CMacroExtractionEngine : public CEngineSuper {
private:
    bool getMacroDataFromFile(const char* location, std::vector<std::pair<std::string, std::pair<int, int>> >& scriptlist);     // 분석을 의로한 파일에서 매크로 파일의 내용을 가져오는 함수
    void getMeanFulMacroData(std::string& script);                                                                              // olevba로 뽑은 매크로 데이터 정보를 정재하는 함수
    
public:
    CMacroExtractionEngine();                                                                                                   // 생성자
    ~CMacroExtractionEngine();                                                                                                  // 소멸자
    bool Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output);                                                     // 검사 함수
};