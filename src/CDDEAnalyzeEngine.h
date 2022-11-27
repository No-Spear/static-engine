#pragma once
#include "CEngineSuper.h"
#include "CException.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <zip.h>

class CDDEAnalyzeEngine : public CEngineSuper {
private:
    zip_t* OOXML;

    std::string extractFileExetoPath(const std::string docpath);
    std::string extractFileSignature(const std::string docpath);    
    std::vector<std::pair<std::string, std::string> > getXMLData(const char* path);
    bool checkDDEKeyword(const std::string location, const std::string xmlData, std::vector<ST_BEHAVIOR>& vecBehaviors);

public:
    CDDEAnalyzeEngine();
    ~CDDEAnalyzeEngine();
    bool Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output);
};