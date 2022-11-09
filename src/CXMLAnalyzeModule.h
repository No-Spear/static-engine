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

private:
    
    // int ExtractEqnEditBinData(void* pFileData, size_t tFileSize, std::vector<BYTE> vecBinData);
    // int AnalyzeEqnEditBinData(const std::vector<BYTE>& vecBinData, ST_ANALYZE_RESULT* output);


    std::vector<string> vecKeyStrings;
    std::map<int,string> mapRegularExpressions;

    int AnalyzeByRegex();
    
    std::map<string,string> readDocFiles(std::vector<string> vecfileContainer);
    std::map<string,string> decodeScript(std::vector<string> vecfileContainer);

    bool returnResult(int AnalyzeResult, ST_ANALYZE_RESULT* output);
    

    
};