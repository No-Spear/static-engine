#include "fileDownload.h"
static size_t write_buffer_callback(char* contents, size_t size, size_t nmemb, std::string* response)
{
    size_t count = size * nmemb;
    if (response != nullptr && count > 0)
    {
        response->append(contents, count);
    }

    return count;
}
FileDownloader::FileDownloader()
{

}

string FileDownloader::fileDownload(string url)
{

    getFileFromUrl(url);
    
}

void FileDownloader::insertCnC(string url)
{}

void FileDownloader::requestAnalysisFile(string fileName)
{}    

string FileDownloader::getFileFromUrl(string url)
{
    const char* surl = url.c_str();

    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if (curl == nullptr)
    {
        std::cout << "init failed" << std::endl;
        return nullptr;
    }

    
    curl_easy_setopt(curl, CURLOPT_URL, surl);

    std::string response;
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_slist* slist = nullptr;
    slist = curl_slist_append(slist,"ACCPET: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);

    CURLcode err_code = curl_easy_perform(curl);
    if (err_code !=CURLE_OK)
    {
        std::cout << "curl_esay_perform failed" <<std::endl;
        return nullptr;
    }


    std::size_t response_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    std::cout << "http response code : " << response_code << std::endl;
    std::cout << "http response : " << response << std::endl;
    
    curl_slist_free_all(slist);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

}

