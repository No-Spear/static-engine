#include "fileDownload.h"

string FileDownloader::encodeBase64(const std::string &url)
{
    std::string encodingUrl;

    int v = 0, valb = -6; 

    for (unsigned char c : url){
        v = (v << 8) + c;
        valb += 8;
        while(valb >=0){
            encodingUrl.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(v >> valb) & 0x3F]);
            valb -= 6;
        }
    }

    if (valb  > -6) encodingUrl.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((v << 8) >> (valb +8)) & 0x3F]);

    while(encodingUrl.size() % 4) encodingUrl.push_back('=');
    
    return encodingUrl;

}

FileDownloader::FileDownloader()
{

}

// FileDownloader::~FileDownloader()
// {

// }


string FileDownloader::fileDownload(string url)
{
    ST_RESPONSE Response = getFileFromUrl(url);
    if(Response.count == 0){
        return "No File";//바꿀 예정
    }
    std::cout << Response.path << std::endl;
    return "FILE";
    
}


void FileDownloader::insertCnC(string url)
{}

void FileDownloader::requestAnalysisFile(string fileName)
{}    


ST_RESPONSE FileDownloader::getFileFromUrl(string url)
{
    const char* surl = (const char *)url.c_str();
    ST_RESPONSE Response;

    Response.fileName = encodeBase64(url);
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl == nullptr)
    {
        std::cout << "init failed" << std::endl;
        Response.count = 0;
        Response.response = nullptr;
        return Response;
    }

    
    curl_easy_setopt(curl, CURLOPT_URL, surl);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeBufferCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Response);

    curl_slist* slist = nullptr;
    slist = curl_slist_append(slist,"ACCPET: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);

    CURLcode err_code = curl_easy_perform(curl);
    if (err_code !=CURLE_OK)
    {
        std::cout << "curl_esay_perform failed" <<std::endl;
        Response.count = 0;
        Response.response = nullptr;
        return Response;
    }


    std::size_t response_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
   

    curl_slist_free_all(slist);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return Response;

}


size_t FileDownloader::writeBufferCallback(unsigned char* contents, size_t size, size_t nmemb, ST_RESPONSE* Response)
{
    
    Response->count = size * nmemb;
    unsigned char* a[Response->count];
    if (Response->response == nullptr && Response->count <= 0)
    {
        std::cout << Response->count << std::endl;
        return Response->count;
    }
    Response->response = contents;

    std::ofstream writeFile;
    string path = string("./temp/") + Response->fileName + string(".png");
    Response->path = path;
    writeFile.open(path, std::ios::binary | std::ios::app);


    for(int i =0; i < Response->count; i++){
        if(!writeFile.is_open()){
            std::cout << "file can't open" <<std::endl;
            writeFile.close();
            break;
        }   

        writeFile.write(reinterpret_cast<const char*>(Response->response), Response->count);
        
    }
    return Response->count;
}