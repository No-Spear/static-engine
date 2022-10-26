#include "CXMLParsingEngine.h"

CXMLParsingEngine::CXMLParsingEngine(){}

CXMLParsingEngine::~CXMLParsingEngine(){}

using std::string;
bool CXMLParsingEngine::Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output)
{

    if(!isDocument(input->vecInputFiles[0].first))return false; //문서 파일 검사

    string xmlBuffer = unzipDocument(input->vecInputFiles[0].first);

    if(xmlBuffer.compare("") == 0)return false;

    std::cout << xmlBuffer << std::endl;

    return true;
}

bool CXMLParsingEngine::isDocument(const string filePath) 
{
    string extension = getFileExtension(filePath); //파일 확장자 가져오기
    std::cout << extension << std::endl;
    string fileSignature = getFileSignature(filePath); //파일 시그니처 가져오기
    std::cout << fileSignature << std::endl;

    if(fileSignature.compare("807534") == 0)
    {
        if(extension.front() == 'd')return true;

        if(extension.front() == 'x')return true;

        if(extension.front() == 'p')return true;

    }

    std::cout << "지원하지 않는 파일 형식입니다." << std::endl;
    return false;


}

string CXMLParsingEngine::getFileExtension(const string filePath)
{

    int location = filePath.find_last_of('.');

    string extension(filePath.substr(location+1));

    return extension;

}

string CXMLParsingEngine::getFileSignature(const string filePath)
{
    string fileSignature;
    std::ifstream readFile;

    readFile.open(filePath, std::ios::binary);


    if (!readFile.is_open())
    {
        //파일이 존재하지 않음 에러처리 해야함
    }

    for(int i =0; i<4;i++)
    {
        int binary = readFile.get();
        std::cout << binary << std::endl;
        fileSignature += std::to_string(binary);
    }
    return fileSignature;
}

string CXMLParsingEngine::unzipDocument(const string filePath) // 문서파일 모든 xml 읽기 
{   
    std::regex re("\\<\\?.*\\?\\>");
    string xmlBuffer;
    this->OOXML = zip_open(filePath.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
    if(this->OOXML == NULL)
    {
        std::cout << "파일을 열 수 없습니다." << std::endl;
        return xmlBuffer;
    }

    int i, n = zip_entries_total(OOXML);
    for (i = 0; i < n; i++)
    {  
        zip_entry_openbyindex(OOXML,i);
        {
            
            
            int isdir = zip_entry_isdir(OOXML);
            if(isdir == 1)continue;
            string name(string(zip_entry_name(OOXML)));
            if(name.find(".rels")==string::npos && name.find(".xml") == string::npos)continue; // .rels .xml 이외에 파일 무시
            char * buf = NULL;
            size_t bufsize= 0;
            zip_entry_read(OOXML, (void **)&buf, &bufsize);
            string tempBuf = string(buf);        
            if(i == 0) xmlBuffer = xmlBuffer + tempBuf; 
            if(i == 0) continue;
            tempBuf = std::regex_replace(tempBuf,re,""); // xml 선언부분 제거

            xmlBuffer = xmlBuffer + tempBuf;
            
        }  

    }
    return xmlBuffer;

}