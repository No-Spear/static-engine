#pragma once
#include "CCheckListSuper.h"

class CCVE_2017_11882_COM : public CCheckListSuper
{
public:
	CCVE_2017_11882_COM();
	~CCVE_2017_11882_COM();

	ECODE Analyze(const std::vector<std::tstring> vecExtractFiles, ST_ANALYZE_RESULT* output);
};