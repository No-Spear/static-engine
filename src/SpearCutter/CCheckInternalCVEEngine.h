#pragma once

#include "CEngineSuper.h"
#include "CException.h"
#include "CCheckListSuper.h"
#include "../DocumentParser/DocumentParser.h"


class CCheckInternalCVEEngine : public CEngineSuper {
private:
    std::map<std::tstring, CCheckListSuper*> m_mapCheckLists;                   // 어떠한 CVE이름과 해당 CVE엔진을 넣는다.
    
public:
    CCheckInternalCVEEngine();
    ~CCheckInternalCVEEngine();
    bool Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output);
};