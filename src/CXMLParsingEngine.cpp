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
    string ext = getFileExt(filePath); //파일 확장자 가져오기
    string fileSignature = getFileSignature(filePath); //파일 시그니처 가져오기


    if (fileSignature.substr(0, 2) != "PK")
    {
        std::cout << "OOXML 형식만 지원됩니다." << std::endl;
        return false;
    }

    std::set<std::string> setDocExt = { "doc", "docx", "docm", "dotm" };
    std::set<std::string> setPptExt = { "ppt", "pptx", "pptm", "potm" };
    std::set<std::string> setXlsExt = { "xls", "xlsx", "xlsm", "xltm" };
    if (setDocExt.find(ext) != setDocExt.end())return true;  /* OOXML DOC */ ;
    if (setPptExt.find(ext) != setPptExt.end())return true;  /* OOXML DOC */ ;
    if (setXlsExt.find(ext) != setXlsExt.end())return true;  /* OOXML DOC */ ;

    // if (setDocExt.count(ext) != 1)return true;  /* OOXML DOC */ ;
    // if (setPptExt.count(ext) == 1)return true;  /* OOXML DOC */ ;
    // if (setXlsExt.count(ext) == 1)return true;  /* OOXML DOC */ ;



    return false;
}

string CXMLParsingEngine::getFileExt(const string filePath)
{

    int location = filePath.find_last_of('.');

    string extension(filePath.substr(location+1));

    return extension;

}

string CXMLParsingEngine::getFileSignature(const string filePath)
{
    string fileSignature;
    std::ifstream readFile;
    std::ostringstream hexbin;

    readFile.open(filePath);


    if (!readFile.is_open())
    {
        //파일이 존재하지 않음 에러처리 해야함
    }

    for(int i =0; i<2;i++)
    {   
        char text;
        readFile.get(text);
        fileSignature += string(&text);
    }

    std::cout << fileSignature << std::endl;
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
    return xmlBuffer;

}