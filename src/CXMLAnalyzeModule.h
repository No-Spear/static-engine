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
    EQUATION_EDITOR_VOL_V1,
    EQUATION_EDITOR_VOL_V2,
    NORMAL_FILE
};



using std::string;

class CXMLAnalyzeModule 
{
public:
;
    CXMLAnalyzeModule();
    bool Analyze(std::vector<string> fileNames, ST_ANALYZE_RESULT* output);
    bool CompoundAnalyze(string filePath,  ST_ANALYZE_RESULT* output);

private:
    string keyString;
    std::map<string,string> decodeScript(std::vector<string> fileNames);
    void getKeyString(string filePath);
    // string replaceAll(const string &str, const string &pattern, const string &replace);
    int AnalyzeByRegex();
    // bool checkFiles(std::map<string, string> files, std::regex re);
    bool returnResult(int AnalyzeResult, ST_ANALYZE_RESULT* output);
    std::map<string,string> readDocFiles(std::vector<string> fileNames);
    string decodingScript;
    std::map<int,string> regularExpressions;
    
};