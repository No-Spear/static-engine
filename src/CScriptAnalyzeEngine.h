#pragma once
#include "CEngineSuper.h"
#include "CException.h"

class CScriptAnalyzeEngine : public CEngineSuper {
private:

    bool checkFollina(std::string script, std::string url, std::vector<ST_BEHAVIOR>& vecBehaviors);

public:
    CScriptAnalyzeEngine();                                                  // 클래스 생성자
    ~CScriptAnalyzeEngine();                                                 // 클래스 소멸자
    bool Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output);        // 검사 함수
};