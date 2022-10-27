#include "CEngineSuper.h"
#include "CException.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex> 
#include <set>
#include <zip.h>

using std::string;

class CXMLParsingEngine
{
public:
    CXMLParsingEngine();
    ~CXMLParsingEngine();
    bool Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output);

private:
    zip_t* OOXML;
    void* buffer;
    size_t bufferSize;
    bool isDocument(const string filePath);
    string getFileExt(const string filePath);
    string getFileSignature(const string filePath);
    string unzipDocument(const string filePath);
};