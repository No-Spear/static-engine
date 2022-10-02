#pragma once

#include "CEngineSuper.h"

class CScriptExtractionEngine : public CEngineSuper {
private:
   
public:
    CScriptExtractionEngine();                                                  // 클래스 생성자
    ~CScriptExtractionEngine();                                                 // 클래스 소멸자
    bool Analyze(ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output);           // 검사 함수
};