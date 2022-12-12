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
private:
    MYSQL* conn;
    MYSQL connect;
    MYSQL_RES* result; 
    MYSQL_ROW row;
    CURL * curl;

public:
    CDownloadFromUrlEngine();
    ~CDownloadFromUrlEngine();
    bool Analyze(const ST_ANALYZE_PARAM *input, ST_ANALYZE_RESULT *output);

private:

    ST_RESPONSE getFileFromUrl(string url);
    
    bool queryCnCUrl(string url,string fileName,ST_RESPONSE * Response);
    void getPath(ST_RESPONSE *Response);
    string getExtension(string fileName);
    string getFileName(string url);   
    string getDomain(string url);
    std::string replaceAll(std::string str, const std::string from, const std::string to);
    static size_t writeBufferCallback(unsigned char* contents, size_t size, size_t nmemb, ST_RESPONSE* Response);
    

};