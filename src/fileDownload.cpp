#include "fileDownload.h"

FileDownloader::FileDownloader()
{
   
}

FileDownloader::~FileDownloader()
{

}

string FileDownloader::getFileName(string url)
{
    std::istringstream ss(url);
    string fileName;

    while(getline(ss, fileName, '/'));

    return fileName;

}

string FileDownloader::fileDownload(string url) // return type은 Result구조체로 스크립트 분석 모듈 완성시 교체 예정
{
    ST_RESPONSE Response = getFileFromUrl(url);
    if(Response.count == 0){
        return "No File";//바꿀 예정
    }
    std::cout << Response.path << std::endl;
    string result = requestAnalysisFile(Response.path);
    if("Malware" == result){
        return "It is Not Malware";
    }

    insertCnC(url);
    
    return result;
    
}


void FileDownloader::insertCnC(string url)
{
    MYSQL* conn;
    MYSQL connect;
    MYSQL_RES* result; 
    MYSQL_ROW row;

    url = "'" + url + "'";
    string values = "(" + url + "," + "'1ABDEF');"; 
    char DBHost[] = "localhost";
    char DBUser[] = "root";
    char DBPass[] = "DBPW1234";
    char DBName[] = "VSERVER";

    mysql_init(&connect);
    conn = mysql_real_connect(&connect, DBHost, DBUser , DBPass, DBName, 3306, (char *)NULL, 0);

    if(!conn){
        return;
    }
    

    string sql ="INSERT INTO CnCTB(URL,file_hash) VALUES" + values;
    std::cout << sql << std::endl;
    if(mysql_query(conn,sql.c_str()) !=0){
        std::cout << "Mysql Error" << std::endl;
        return;
    }
    mysql_close(conn);
    return;

}

string FileDownloader::requestAnalysisFile(string fileName) // return type은 Result구조체로 스크립트 분석 모듈 완성시 교체 예정
{

    time_t t = time(NULL); // 외부모듈 script 검사 모듈 추가 부분 랜덤으로 대체
    // int random = (t%10)+1;
    int random = 5;
    if(0 < random && random < 4){
        return "Benign";
    }

    if(4 < random && random < 8){
        return "Suspicious";
    }


    return "Malware";

}    


ST_RESPONSE FileDownloader::getFileFromUrl(string url)
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

    if(!writeFile.is_open()){
        std::cout << "file can't open" <<std::endl;
        writeFile.close();
        return Response->count;
    }   

    for(int i =0; i < Response->count; i++){


        writeFile.write(reinterpret_cast<const char*>(Response->response), Response->count);
        
    }
    return Response->count;
}