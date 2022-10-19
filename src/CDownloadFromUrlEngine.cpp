#include "CDownloadFromUrlEngine.h"

CDownloadFromUrlEngine::CDownloadFromUrlEngine() : CEngineSuper(2)
{
 
    char DBHost[] = "nospear.c9jy6dsf1qz4.ap-northeast-2.rds.amazonaws.co";
    char DBUser[] = "nospear";
    char DBPass[] = "nospear!";
    char DBName[] = "cncDB";
    mysql_init(&connect);
    conn = mysql_real_connect(&connect, DBHost, DBUser , DBPass, DBName, 3306, (char *)NULL, 0);
  
    if(conn == NULL)
    {
        fprintf(stderr, "Mysql connection error : %s", mysql_error(&connect));
    }
}

CDownloadFromUrlEngine::~CDownloadFromUrlEngine()
{
    mysql_close(conn);
}

bool CDownloadFromUrlEngine::queryCnCUrl(string url,string fileName,ST_RESPONSE * Response)
{
    string sql = "";
    url = "'" + url + "'";    
    std::cout << fileName << std::endl;
    if(!fileName.compare("NoFile")){
        sql = "SELECT fileHash from cncTable where url=" + url;   
    }else
    {
        fileName = "'" + fileName + "'";
        sql ="SELECT fileHash from cncTable where url=" + url + "and fileHash=" + fileName;   
    }

    std::cout << sql << std::endl;
    if(mysql_query(conn,sql.c_str()) !=0){
        return false;
    }

    result = mysql_store_result(conn);
    if ((row = mysql_fetch_row(result)) == NULL){
        return false;
    }

    if(!fileName.compare("NoFile"))Response->path = "../temp/" + string(row[0]);
    std::cout << Response->path << std::endl;
    return true;
}

string CDownloadFromUrlEngine::getExtension(string fileName)
{

    std::istringstream ss(fileName);
    string extension;
    while(getline(ss, extension, '.'));

    return "."+extension;
}

void CDownloadFromUrlEngine::getPath(ST_RESPONSE *Response)
{   
    std::ifstream in(Response->path);
    string file;
    in.seekg(0, std::ios::end);

    int size = in.tellg();
    file.resize(size);

    in.seekg(0, std::ios::beg);

    in.read(&file[0], size);

    // 파일 불러와서 해시 값 추출
    unsigned char digest[SHA256::DIGEST_SIZE] = {0,};
    char fileName[65] = {0,};

    SHA256 ctx = SHA256();
    ctx.init(); 
    ctx.update((unsigned char *)file.c_str(),size);
    ctx.final(digest);

    for(int i=0;i<SHA256::DIGEST_SIZE;i++){
        sprintf(fileName+i*2,"%02x",digest[i]);    
    }
    string extension = getExtension(Response->fileName);
    string tempFileName = Response->fileName;
    Response->fileName = string(fileName) + extension; 
    std::cout << "File Name is "<<Response->fileName << std::endl;
    string newPath = "../temp/" + Response->fileName;
    if(access(newPath.c_str(),F_OK)==0){
        remove(Response->path.c_str());
        Response->path = newPath;        
        return;
    }
    if(-1 == rename(Response->path.c_str(),newPath.c_str() ))Response->path = "../temp/" + string(tempFileName);

    Response->path = newPath;
}

bool CDownloadFromUrlEngine::Analyze(const ST_ANALYZE_PARAM *input, ST_ANALYZE_RESULT *output)
{   
    int fileStatus;
    for(int i =0; i < input->vecURLs.size(); i++){


        ST_RESPONSE Response = getFileFromUrl(getDomain(input->vecURLs[i]));
        std::cout << Response.count << std::endl;
        if(Response.count == 0){
            if(queryCnCUrl(input->vecURLs[i],Response.fileName,&Response))fileStatus = CCOF;
            else fileStatus = CCNF;

            output->vecExtractedFiles.push_back(std::make_pair(Response.path,fileStatus));
            std::cout << "CNC Status and File Status " << fileStatus <<std::endl;
            if(fileStatus == CCNF && input->vecURLs.size() == i+1)return false;
            else if(fileStatus == CCNF && input->vecURLs.size() != i+1) continue;

            

        }else{
            getPath(&Response);
                // 
            if(queryCnCUrl(input->vecURLs[i],Response.fileName,&Response))fileStatus = COOF;
            else fileStatus = CONF;

            output->vecExtractedFiles.push_back(std::make_pair(Response.path,fileStatus));
            std::cout << Response.path << std::endl;
            std::cout << "CNC Status and File Status " << fileStatus <<std::endl;
        }
    }
    return true;
}

string CDownloadFromUrlEngine::getFileName(string url)
{
    std::istringstream ss(url);
    string fileName;
    while(getline(ss, fileName, '/'));

    return fileName;

}

string CDownloadFromUrlEngine::getDomain(string url)
{
    string encoded_slash = "\%2f";
    while(1)
    {     
        if(url.find("\%2f") == string::npos)break;
        url.replace(url.find("\%2f"),encoded_slash.length(),"/");
    }
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
    std::cout << settingUrl << std::endl;
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
        return Response;
    }

    curl_easy_setopt(curl, CURLOPT_URL, surl);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeBufferCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Response);

    curl_slist* slist = nullptr;
    slist = curl_slist_append(slist,"ACCPET: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);

    std::cout << "File Download Start" <<std::endl;

    CURLcode err_code = curl_easy_perform(curl);
    if (err_code !=CURLE_OK)
    {   
        
        std::cout << "No File or No URL" <<std::endl;
        Response.count = 0;
        Response.fileName = "NoFile";
        Response.path = "../temp";
        Response.response = nullptr;
        return Response;
    }
    if(Response.count > 838860800){
        std::cout << "No File or No URL" <<std::endl;
        Response.count = 0;
        Response.fileName = "NoFile";
        Response.path = "../temp";
        Response.response = nullptr;
        return Response;
    }

    std::size_t response_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
   

    curl_slist_free_all(slist);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    std::cout << "File Download Finish" <<std::endl;
    return Response;

}

size_t CDownloadFromUrlEngine::writeBufferCallback(unsigned char* contents, size_t size, size_t nmemb, ST_RESPONSE* Response)
{
    Response->count = size * nmemb;
    if(Response->count > 838860800){
        return Response->count;
    }
    if (Response->response == nullptr && Response->count <= 0)
    {
        std::cout << Response->count << std::endl;
        return Response->count;
    }
    Response->response = contents;

    std::ofstream writeFile;


    
    string path = string("../temp/") + Response->fileName; 

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