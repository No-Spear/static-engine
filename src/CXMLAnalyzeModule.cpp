#include "CXMLAnalyzeModule.h"

CXMLAnalyzeModule::CXMLAnalyzeModule()
{
    mapRegularExpressions.insert(std::pair(EQUATION_EDITOR_VOL_V1,"ole10native"));
    mapRegularExpressions.insert(std::pair(EQUATION_EDITOR_VOL_V2,"equationnative"));

}

bool CXMLAnalyzeModule::Analyze(std::vector<string> fileNames, ST_ANALYZE_RESULT* output)
{
    int AnalyzeResult;

    std::map<string, string> files = decodeScript(fileNames);
    AnalyzeResult = AnalyzeByRegex();
    if(returnResult(AnalyzeResult,output)) return true;


    return false;
}


std::map<string,string> CXMLAnalyzeModule::decodeScript(std::vector<string> vecFileContainer)
{
    //분석 완료 시 추가 예정
    std::map<string,string> files = readDocFiles(vecFileContainer);
    for(std::map<string,string>::iterator it = files.begin(); it != files.end(); it++)
    {   
        // std::string& strScriptContext = ConvertMBSFromUnicode(it->second);
        // 유니코드 to ascii 만들기
        std::string& strScriptContext = it->second;

        string keyString = "";

        if(it->first.find(".rels") != string::npos && it->first.find(".xml") !=string::npos)
            continue;

        if(int(u_char(strScriptContext[0])) != 208 && int(u_char(strScriptContext[1])) != 207 )
            continue;

        for(int i = 1280; i < 1310; i += 2)
        {
            if(strScriptContext[i] == ' ') continue;
            keyString = keyString + strScriptContext[i];
        }
        

        std::cout << "keyString : " << keyString << std::endl;
        vecKeyStrings.push_back(keyString);
    }
    
    return files;
}

int CXMLAnalyzeModule::AnalyzeByRegex()
{

    for(std::map<int,string>::iterator it = mapRegularExpressions.begin();
        it != mapRegularExpressions.end();
        it++)
    {
        int vulnerabilityNumber = it->first;
        string strRegularExpression = it->second;
        
        std::regex re(strRegularExpression,std::regex::grep | std::regex::icase);
        for(string keyString : vecKeyStrings)
        if(std::regex_search(keyString,re)) 
            return vulnerabilityNumber;
    }
    
    return NORMAL_FILE;

}

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

std::map<string,string> CXMLAnalyzeModule::readDocFiles(std::vector<string> vecFileContainer)
{
    std::map<string,string> files;
    for(string name : vecFileContainer)
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