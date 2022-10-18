#pragma once
#include "CEngineSuper.h"
#include "sha256.h"
#include <string> 
#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>
#include <unistd.h>
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
    bool Analyze(const ST_ANALYZE_PARAM *input, ST_ANALYZE_RESULT *output);

private:
    MYSQL* conn;
    MYSQL connect;
    MYSQL_RES* result; 
    MYSQL_ROW row;
    CURL * curl;
    bool queryCnCUrl(string url);
    void getPath(ST_RESPONSE *Response);
    string getFileName(string url);   
    string getDomain(string url);
    ST_RESPONSE getFileFromUrl(string url);
    static size_t writeBufferCallback(unsigned char* contents, size_t size, size_t nmemb, ST_RESPONSE* Response);

};