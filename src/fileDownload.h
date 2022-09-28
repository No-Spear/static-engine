#pragma once 
#include <string>
#include <iostream> 
#include <fstream>
#include <curl/curl.h>
#include <curl/curlver.h>
#include <curl/easy.h>
#include <curl/urlapi.h>

using std::string;

struct ST_RESPONSE{

    unsigned char* response;
    size_t count;

};


class FileDownloader{

public:
    FileDownloader();
    string fileDownload(string url); 

private:
    
    CURL * curl;
    //CURLcode res; 

    void insertCnC(string url);
    string encodeBase64(const std::string &url);
    void requestAnalysisFile(string fileName);
    string saveFile(string encodingUrl, ST_RESPONSE Response);
    ST_RESPONSE getFileFromUrl(string url);
    static size_t write_buffer_callback(unsigned char* contents, size_t size, size_t nmemb, ST_RESPONSE* Response);    
};

