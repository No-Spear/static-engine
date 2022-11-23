#pragma once
#include "CEngineSuper.h"
#include "CException.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <regex>
#include <zip.h>

class CMacroExtractionEngine : public CEngineSuper {
private:
    zip_t* macroFile;                                                           // 분석을 의로한 파일

    std::string extractFileExetoPath(const std::string docpath);                // 문서의 위치로부터 문서 타입을 가져오기 위한 함수
    bool open(const char* pszFile);                                             // 분석을 의로하는 파일을 열수 있는지 홛인하는 함수
    std::string getMacroDataFromFile(const char* location);                     // 분석을 의로한 파일에서 매크로 파일의 내용을 가져오는 함수
    
public:
    CMacroExtractionEngine();                                                   // 생성자
    ~CMacroExtractionEngine();                                                  // 소멸자
    bool Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output);     // 검사 함수
};