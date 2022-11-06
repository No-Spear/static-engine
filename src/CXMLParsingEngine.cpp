#include "CXMLParsingEngine.h"

CXMLParsingEngine::CXMLParsingEngine()
{}

CXMLParsingEngine::~CXMLParsingEngine()
{}

using std::string;
bool CXMLParsingEngine::Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output)
{
    std::map<string,string> files;
    if(!isDocument(input->vecInputFiles[0].first))return false; //문서 파일 검사

    std::vector<string> fileNames = unzipDocument(input->vecInputFiles[0].first);

    if(fileNames.size() == NoFile)return false;

    // OOXML 검사 부분 추가
    CXMLAnalyzeModule AnalyzeModule;
    AnalyzeModule.Analyze(fileNames, output);
    
    removeTempFiles(fileNames);
    
    return true;
}

void CXMLParsingEngine::removeTempFiles(std::vector<string> fileNames)
{
    int r;
    for(string file : fileNames)
    {
        r = remove(file.c_str());
    }

}


void CXMLParsingEngine::organizeMemory(){
    free(this->OOXML);
}

bool CXMLParsingEngine::isDocument(const string filePath) 
{
    const string OoxmlSignature = "8075";
    string ext = getFileExt(filePath); //파일 확장자 가져오기
    string fileSignature = getFileSignature(filePath); //파일 시그니처 가져오기

    // std::cout << fileSignature << std::endl;

    if (fileSignature.substr(0, 4) != OoxmlSignature)
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

    return false;
}

string CXMLParsingEngine::getFileExt(const string filePath)
{

    int dotPosition = filePath.find_last_of('.');
    string ext = filePath.substr(dotPosition+1);

    return ext;

}

string CXMLParsingEngine::getFileSignature(const string filePath)
{
    string fileSignature;
    std::ifstream readFile;
    std::ostringstream hexbin;

    readFile.open(filePath);


    if (!readFile.is_open())
    {
        try{
            throw std::runtime_error("파일이 존재하지 않습니다.");
        }catch (const std::exception& e)
        {
            printf("%s\n",e.what());
            return fileSignature;
        }
    }

    for(int i =0; i<2;i++)
    {   
        int a = readFile.get();
        // std::cout << a << std::endl;
        fileSignature += std::to_string(a);
    }

    return fileSignature;
}

std::vector<string> CXMLParsingEngine::unzipDocument(const string filePath) // 문서파일 모든 xml 읽기 
{   
    std::vector<string> fileNames;
    this->OOXML = zip_open(filePath.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
    if(this->OOXML == NULL)
    {
        std::cout << "파일을 열 수 없습니다." << std::endl;
        return fileNames;
    }

    int i, n = zip_entries_total(OOXML);
    for (i = 0; i < n; i++)
    {  
        zip_entry_openbyindex(OOXML,i);
        
        
            
        int isdir = zip_entry_isdir(OOXML);
        if(isdir == 1)continue;

        string name(string(zip_entry_name(OOXML)));
        std::set<std::string> exceptionExts = {"png","jpg","svg","jpeg"};
        if(exceptionExts.find(getFileExt(name)) != exceptionExts.end())continue;
        char * buf = NULL;
        size_t bufsize= 0;
        name = makeFileName(name);
        // std::cout << name << std::endl;
        zip_entry_fread(OOXML,name.c_str());
        fileNames.push_back(name);

    }
    
    
    organizeMemory();

    return fileNames;

}

string CXMLParsingEngine::makeFileName(string name)
{

    int slashPosition = name.find_last_of('/');
    string fileName = "../temp/" + name.substr(slashPosition+1);

    return fileName;
    

}

