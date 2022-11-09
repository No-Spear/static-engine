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
#include <sys/stat.h>



using std::string;

class CXMLParsingEngine : public CEngineSuper
{
public:
    const int NoFile = 0;   

    CXMLParsingEngine();
    ~CXMLParsingEngine();

    bool Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output);

private:


    std::map<string,string> xmls;
    zip_t* OOXML;
    size_t bufferSize;
    void removeTempFiles(std::vector<string> vecfileContainer);
    bool isDocument(const string filePath, const string Signature);
    string getFileExt(const string filePath);
    string getFileSignature(const string filePath);
    std::vector<string> unzipDocument(const string filePath);

};