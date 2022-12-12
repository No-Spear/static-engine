#pragma once
#include "../DocumentParser/DocumentParser.h"
#include "CEngineSuper.h"
#include "struct.h"

using namespace core;

class CCheckListSuper {

public:
    virtual ECODE Analyze(const std::vector<std::tstring> vecExtractFiles, ST_ANALYZE_RESULT* output) = 0;
};