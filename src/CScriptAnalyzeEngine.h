#pragma once

#include "CEngineSuper.h"

class CScriptAnalyzeEngine : public CEngineSuper {
private:
   
public:
    CScriptAnalyzeEngine();                                                  // 클래스 생성자
    ~CScriptAnalyzeEngine();                                                 // 클래스 소멸자
    bool Analyze(ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output);           // 검사 함수
};