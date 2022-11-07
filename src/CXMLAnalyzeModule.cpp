#include "CXMLAnalyzeModule.h"

CXMLAnalyzeModule::CXMLAnalyzeModule()
{
    regularExpressions.insert(std::pair(EQUATION_EDITOR_VOL_V1,"ole10native"));
    regularExpressions.insert(std::pair(EQUATION_EDITOR_VOL_V2,"equationnative"));

}

bool CXMLAnalyzeModule::Analyze(std::vector<string> fileNames, ST_ANALYZE_RESULT* output)
{
    int AnalyzeResult;

    std::map<string, string> files = decodeScript(fileNames);
    AnalyzeResult = AnalyzeByRegex();
    if(returnResult(AnalyzeResult,output)) return true;


    return false;
}

bool CXMLAnalyzeModule::CompoundAnalyze(string filePath,  ST_ANALYZE_RESULT* output)
{
    int AnalyzeResult;

    getKeyString(filePath);

    AnalyzeResult = AnalyzeByRegex();

    if(returnResult(AnalyzeResult,output)) return true;

    return false;    

}

std::map<string,string> CXMLAnalyzeModule::decodeScript(std::vector<string> fileNames)
{
    //분석 완료 시 추가 예정
    std::map<string,string> files = readDocFiles(fileNames);
    for(std::map<string,string>::iterator it = files.begin(); it != files.end(); it++)
    {   
        string keyString = "";
        if(it->first.find(".rels") != string::npos && it->first.find(".xml") !=string::npos) continue;
        if(int(u_char(it->second[0])) != 208 && int(u_char(it->second[1])) != 207 )continue;

        for(int i = 1280; i < 1310; i += 2)
        {
            if(it->second[i] == ' ') continue;
            keyString = keyString + it->second[i];
        }
        

        std::cout << "keyString : " << keyString << std::endl;
        keyStrings.push_back(keyString);
    }
    
    return files;
}

void CXMLAnalyzeModule::getKeyString(string filePath)
{
    string file;
    string keyString = "";
    std::ifstream in(filePath, std::ifstream::binary);
    if(!in.is_open()){
        std::cout << "file Can't Open" << std::endl;
        return;
    }
    in.seekg(0,std::ios::end);
    int size = in.tellg();
    file.resize(size);
    in.seekg(0,std::ios::beg);
    in.read(&file[0],size);
    in.close();

    for(int i = 1280; i < 1310; i += 2)
    {
        if(file[i] == ' ') continue;
        keyString = keyString + file[i];
    }
    std::cout << "keyString : " << keyString << std::endl;
    keyStrings.push_back(keyString);
}

// string CXMLAnalyzeModule::replaceAll(const string &str, const string &pattern, const string &replace)
// {
//     string result = str;
//     string::size_type pos = 0;
//     string::size_type offset = 0;

//     while((pos = result.find(pattern, offset)) != string::npos)
//     {
//         result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
//         offset = pos + replace.size();
//     }

//     return result;
// }

int CXMLAnalyzeModule::AnalyzeByRegex()
{

    for(std::map<int,string>::iterator it = regularExpressions.begin();
        it != regularExpressions.end();
        it++)
    {
        std::regex re(it->second,std::regex::grep | std::regex::icase);
        for(string keyString : keyStrings)
        if(std::regex_search(keyString,re)) return it->first;
    }
    
    return NORMAL_FILE;

}

// bool CXMLAnalyzeModule::checkFiles(std::map<string, string> files, std::regex re)
// {

//     for(std::map<string,string>::iterator it = files.begin();
//         it != files.end();
//         it++)
//     {
//         if(it->first.find(".rels") != string::npos && it->first.find(".xml") !=string::npos) continue;
        
        
//         if(std::regex_search(it->second,re)){ 
//             return true;
//         }

//     }

//     return false;

// }

bool CXMLAnalyzeModule::returnResult(int AnalyzeResult, ST_ANALYZE_RESULT* output)
{
    ST_BEHAVIOR behavior;
    switch(AnalyzeResult)
    {
        case EQUATION_EDITOR_VOL_V1:
            std::cout << "EQUATION_EDITOR_VOL_V1" << std::endl;
            behavior.Severity = 8;
            behavior.strDesc = "수식편집기 호출";
            behavior.strName = "수식편집기 취약점";
            behavior.strUrl = "";
            output->vecBehaviors.push_back(behavior);
            break;
        case EQUATION_EDITOR_VOL_V2:
            std::cout << "EQUATION_EDITOR_VOL_V2" << std::endl;
            behavior.Severity = 8;
            behavior.strDesc = "수식편집기 호출";
            behavior.strName = "수식편집기 취약점";
            behavior.strUrl = "";
            output->vecBehaviors.push_back(behavior);
            break;    
        default:
            std::cout << "Normal File" << std::endl; 
            return false;  
    }

    return true;
}

std::map<string,string> CXMLAnalyzeModule::readDocFiles(std::vector<string> fileNames)
{
    std::map<string,string> files;
    for(string name : fileNames)
    {
        string file;
        
        std::ifstream in(name, std::ifstream::binary);
        if(!in.is_open()){
            std::cout << "file Can't Open" << std::endl;
            continue;
        }
        in.seekg(0,std::ios::end);
        int size = in.tellg();
        file.resize(size);
        in.seekg(0,std::ios::beg);
        in.read(&file[0],size);
        in.close();
        files.insert(std::pair(name,file));

    }

    
    return files;
}