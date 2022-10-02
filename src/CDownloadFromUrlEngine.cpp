#include "CDownloadFromUrlEngine.h"

CDownloadFromUrlEngine::CDownloadFromUrlEngine()
{
}

CDownloadFromUrlEngine::~CDownloadFromUrlEngine()
{
}

bool CDownloadFromUrlEngine::Analyze(ST_ANALYZE_PARAM *input, ST_ANALYZE_RESULT *output)
{

    for(int i =0; i < input->vecURLs.size(); i++){
        ST_RESPONSE Response = getFileFromUrl(getDomain(input->vecURLs[i]));
        if(Response.count == 0){
            return false;
        }
        output->vecExtractedFiles.push_back(Response.path);
        std::cout << Response.path << std::endl;
    }
    return true;

}

string CDownloadFromUrlEngine::getFileName(string url)
{
    std::istringstream ss(url);
    string fileName;
    while(getline(ss, fileName, '/')){
        
    };

    return fileName;

}

string CDownloadFromUrlEngine::getDomain(string url)
{
    std::istringstream ss(url);
    string tempUrl;
    std::vector<string> domain;
    string settingUrl;
    int i = 0;
    while(getline(ss,tempUrl, '/')){
        if(1<i){
            domain.push_back(tempUrl);
        }
        i++;

    }

    for(int x =0; x < domain.size(); x++){
        settingUrl += domain[x];
        if((x+1) == domain.size()){
            break;
        } 
        settingUrl += "/";
       
    }

    return settingUrl;

}


ST_RESPONSE CDownloadFromUrlEngine::getFileFromUrl(string url)
{
    const char* surl = (const char *)url.c_str();
    ST_RESPONSE Response;
    Response.fileName = getFileName(url);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl == nullptr)
    {
        std::cout << "init failed" << std::endl;
        Response.count = 0;
        Response.response = nullptr;
        Response.fileName = nullptr;
        Response.path = nullptr;
        return Response;
    }

    curl_easy_setopt(curl, CURLOPT_URL, surl);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeBufferCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Response);

    curl_slist* slist = nullptr;
    slist = curl_slist_append(slist,"ACCPET: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);

    std::cout << "File Download Start" <<std::endl;
   
    CURLcode err_code = curl_easy_perform(curl);
    if (err_code !=CURLE_OK)
    {
        std::cout << "curl_esay_perform failed" <<std::endl;
        Response.count = 0;
        Response.response = nullptr;
        Response.fileName = nullptr;
        Response.path = nullptr;
        return Response;
    }


    std::size_t response_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
   

    curl_slist_free_all(slist);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return Response;

}

size_t CDownloadFromUrlEngine::writeBufferCallback(unsigned char* contents, size_t size, size_t nmemb, ST_RESPONSE* Response)
{
    
    Response->count = size * nmemb;
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

    if(!writeFile.is_open()){
        std::cout << "file can't open" <<std::endl;
        writeFile.close();
        return Response->count;
    }   



    writeFile.write(reinterpret_cast<const char*>(Response->response), Response->count);
        

    return Response->count;
}