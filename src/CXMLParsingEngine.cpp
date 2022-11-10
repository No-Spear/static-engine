#include "CXMLParsingEngine.h"

CXMLParsingEngine::CXMLParsingEngine() : CEngineSuper(0, "XMLParsing")
{}

CXMLParsingEngine::~CXMLParsingEngine()
{}

using std::string;
bool CXMLParsingEngine::Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output)
{

    CXMLAnalyzeModule AnalyzeModule;    
    const string OoxmlSignature = "8075";
    const string compoundSignature = "208207";    
    const string inputFile = input->vecInputFiles[0].first;
    std::vector<string> vecFileContainer;
    bool result = false;


    if(isDocument(inputFile,OoxmlSignature))
    {
        vecFileContainer = unzipDocument(inputFile);
        if(vecFileContainer.empty())
            return false;

        result = AnalyzeModule.Analyze(vecFileContainer, output);
        removeTempFiles(vecFileContainer);
        
    }
    else if(isDocument(inputFile,compoundSignature))
    {
        vecFileContainer.push_back(inputFile);
        result = AnalyzeModule.Analyze(vecFileContainer, output);
    }
    else
        try
        {
            throw std::runtime_error("Unspecified type data");            
        }
        catch (const std::exception& e)
        {
            printf("%s\n",e.what());
            return false;
        }
        
    
    return result;
    
}

void CXMLParsingEngine::removeTempFiles(std::vector<string> vecfileContainer)
{
    int r;
    for(string file : vecfileContainer)
    {
        r = remove(file.c_str());
    }

}


bool CXMLParsingEngine::isDocument(const string filePath, const string Signature) 
{

    const size_t signatureSize = Signature.size();

    string ext = getFileExt(filePath); //파일 확장자 가져오기
    string fileSignature = getFileSignature(filePath); //파일 시그니처 가져오기

    if (fileSignature == "")
        return false;

    if (fileSignature.substr(0, signatureSize) != Signature)
        return false;

    std::set<std::string> setDocExt = { "doc", "docx", "docm", "dotm" };
    std::set<std::string> setPptExt = { "ppt", "pptx", "pptm", "potm", "ppsx" };
    std::set<std::string> setXlsExt = { "xls", "xlsx", "xlsm", "xltm" };
    if (setDocExt.find(ext) != setDocExt.end())return true;  /* OOXML DOC */ ;
    if (setPptExt.find(ext) != setPptExt.end())return true;  /* OOXML DOC */ ;
    if (setXlsExt.find(ext) != setXlsExt.end())return true;  /* OOXML DOC */ ;

    return false;
}

string CXMLParsingEngine::getFileExt(const string filePath)
{

    int dotPosition = 0;
    dotPosition = filePath.find_last_of('.');
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
            fileSignature = "";
            return fileSignature;
        }
    }

    for(int i =0; i<2;i++)
    {   
        int a = readFile.get();
        fileSignature += std::to_string(a);
    }

    return fileSignature;
}

std::vector<string> CXMLParsingEngine::unzipDocument(const string filePath) // 문서파일 모든 xml 읽기 
{   
    std::vector<string> vecfileContainer;
    this->OOXML = zip_open(filePath.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
    if(this->OOXML == NULL)
    {
        std::cout << "파일을 열 수 없습니다." << std::endl;
        return vecfileContainer;
    }

    int i, n = zip_entries_total(OOXML);
    for (i = 0; i < n; i++)
    {  
        zip_entry_openbyindex(OOXML,i);
        
        int isdir = zip_entry_isdir(OOXML);
        if(isdir == 1)
            continue;

        string name(string(zip_entry_name(OOXML)));

        std::set<std::string> setExceptionExts = {"png","jpg","svg","jpeg"};
        if(setExceptionExts.find(getFileExt(name)) != setExceptionExts.end())
            continue;
        
        char * buf = NULL;
        size_t bufsize= 0;
        int slashPosition = name.find_last_of('/');
        name = "../temp/" + name.substr(slashPosition+1);

        zip_entry_fread(OOXML,name.c_str());
        vecfileContainer.push_back(name);

    }
    
    for(string name : vecfileContainer)
    {
        if(chmod(name.c_str(), 0555)) std::cout << "권한 변경 실패" << std::endl;
    }
   
    free(this->OOXML);

    return vecfileContainer;
}


