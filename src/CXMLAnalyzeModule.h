#include "CEngineSuper.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex> 
#include <set>
#include <map>
#include <stdexcept>

enum{
    EQUATION_EDITOR_VOL,
    NORMAL_FILE
};



using std::string;

class CXMLAnalyzeModule
{
public:
    CXMLAnalyzeModule();
    bool Analyze(string xmls, ST_ANALYZE_RESULT* output);

private:
    bool decodeScript(string xmls);
    int AnalyzeByRegex();
    bool returnResult(int AnalyzeResult, ST_ANALYZE_RESULT* output);
    string decodingScript;
    std::map<int,string> regularExpressions;
};