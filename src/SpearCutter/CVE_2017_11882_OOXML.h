#pragma once
#include "CCheckListSuper.h"

class CCVE_2017_11882_OOXML : public CCheckListSuper
{
public:
	CCVE_2017_11882_OOXML();
	~CCVE_2017_11882_OOXML();

	ECODE Analyze(const std::vector<std::tstring> vecExtractFiles, ST_ANALYZE_RESULT* output);
};