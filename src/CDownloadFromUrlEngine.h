#pragma once
#include "CEngineSuper.h"
#include <string> 
#include <fstream>
#include <sstream>
#include <time.h>
#include <curl/curl.h>
#include <curl/curlver.h>
#include <curl/easy.h>
#include <curl/urlapi.h>
#include <mysql/mysql.h>

using std::string;

struct ST_RESPONSE{

    size_t count;
    string fileName;
    string path;
    unsigned char* response;
    

};

class CDownloadFromUrlEngine : public CEngineSuper
{

public:
    CDownloadFromUrlEngine();
    ~CDownloadFromUrlEngine();
    ST_ANALYZE_RESULT Analyze(ST_ANALYZE_PARAM input, ST_ANALYZE_RESULT output);

private:
    
    CURL * curl;
    string getFileName(string url);   
    ST_RESPONSE getFileFromUrl(string url);
    static size_t writeBufferCallback(unsigned char* contents, size_t size, size_t nmemb, ST_RESPONSE* Response);

};