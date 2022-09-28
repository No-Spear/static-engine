#pragma once 
#include <string>
#include <iostream> 
#include <fstream>
#include <curl/curl.h>
#include <curl/curlver.h>
#include <curl/easy.h>
#include <curl/urlapi.h>

using std::string;
class FileDownloader{



public:
    FileDownloader();
    CURL * curl;
    CURLcode res; 
    string fileDownload(string url);

    
    

private:
    void insertCnC(string url);
    void requestAnalysisFile(string fileName);
    string getFileFromUrl(string url);
};