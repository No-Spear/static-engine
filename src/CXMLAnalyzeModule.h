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
private:
    std::vector<string> vecKeyStrings;
    std::map<int,string> mapRegularExpressions;

public:

    CXMLAnalyzeModule();
    bool Analyze(std::vector<string> fileNames, ST_ANALYZE_RESULT* output);

private:
    
    void ExtractEqnEditData(std::vector<string> vecfileContainer);
    int AnalyzeByRegex();
    std::map<string,string> readDocFiles(std::vector<string> vecfileContainer);
    bool returnResult(int AnalyzeResult, ST_ANALYZE_RESULT* output);
    

    
};