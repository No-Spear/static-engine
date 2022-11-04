#pragma once
#include "CEngineSuper.h"
#include "CException.h"
#include <regex>

class CScriptAnalyzeEngine : public CEngineSuper {
private:

    bool checkFollina(std::string script, std::string url, std::vector<ST_BEHAVIOR>& vecBehaviors);     // Follina를 통한 악성행위를 확인하는 함수
    bool checkMacro(std::string script, std::vector<ST_BEHAVIOR>& vecBehaiors);                         // 매크로를 통한 악성행위를 확인하는 함수

public:
    CScriptAnalyzeEngine();                                                                             // 클래스 생성자
    ~CScriptAnalyzeEngine();                                                                            // 클래스 소멸자
    bool Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output);                             // 검사 함수
};