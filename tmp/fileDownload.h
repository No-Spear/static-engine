#pragma once 
#include <string>
#include <iostream> 
#include <fstream>
#include <sstream>
#include <vector>
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


class FileDownloader{

public:
    MYSQL* conn;
    MYSQL_RES* result; 
    MYSQL_ROW row;
    FileDownloader();
    ~FileDownloader();
    string fileDownload(string url); 
    

private:
    
    CURL * curl;
    //CURLcode res; 

    void insertCnC(string url);
    string getFileName(string url);
    string requestAnalysisFile(string fileName);    
    ST_RESPONSE getFileFromUrl(string url);
    static size_t writeBufferCallback(unsigned char* contents, size_t size, size_t nmemb, ST_RESPONSE* Response);    
};