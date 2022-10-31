#include "CEngineSuper.h"
#include "CXMLAnalyzeModule.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex> 
#include <set>
#include <map>
#include <stdexcept>
#include <zip.h>


using std::string;

class CXMLParsingEngine
{
public:

    const string NoFile = "NO FILE";   
    
    CXMLParsingEngine();
    ~CXMLParsingEngine();
    bool Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output);

private: 
    std::map<string,string> xmls;
    zip_t* OOXML;
    size_t bufferSize;
    void organizeMemory();
    bool isDocument(const string filePath);
    string getFileExt(const string filePath);
    string getFileSignature(const string filePath);
    string unzipDocument(const string filePath);
};