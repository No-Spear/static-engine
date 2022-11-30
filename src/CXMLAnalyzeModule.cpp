#include "CXMLAnalyzeModule.h"

CXMLAnalyzeModule::CXMLAnalyzeModule()
{


    mapRegularExpressions.insert(std::pair(EQUATION_EDITOR_VOL_V1,"[o][l][e][1][0][n][a][t][i][v][e]"));
    mapRegularExpressions.insert(std::pair(EQUATION_EDITOR_VOL_V2,"[e][q][u][a][t][i][o][n][n][a][t][i][v][e]"));

}

bool CXMLAnalyzeModule::Analyze(std::vector<string> fileNames, ST_ANALYZE_RESULT* output)
{
    int AnalyzeResult;

    {
        AnalyzeResult = ExtractEqnEditData(fileNames);
        // 취약점 추출 함수 추가 
    }

    if(returnResult(AnalyzeResult,output)) return true;


    return false;
}


// void CXMLAnalyzeModule::ExtractEqnEditData(std::vector<string> vecFileContainer)
// {
//     std::map<string,string> mapFileContentsContainer = readDocFiles(vecFileContainer);
//     for(std::map<string,string>::iterator it = mapFileContentsContainer.begin();
//     it != mapFileContentsContainer.end(); 
//     it++)
//     {   
//         // std::string& strScriptContext = ConvertMBSFromUnicode(it->second);

//         std::string& strScriptContext = it->second;

//         string keyString = "";

//         if(it->first.find(".rels") != string::npos && it->first.find(".xml") !=string::npos)
//             continue;

//         if(int(u_char(strScriptContext[0])) != 208 && int(u_char(strScriptContext[1])) != 207 )
//             continue;

//         if(strScriptContext.size() < 1310)
//             continue;

//         for(int i = 1280; i < 1310; i += 2)
//         {
//             if(strScriptContext[i] == ' ') continue;
//             keyString = keyString + strScriptContext[i];
//         }
//         std::cout << "keyString : " << keyString << std::endl;
//         vecKeyStrings.push_back(keyString);
//     }
    
// }

int CXMLAnalyzeModule::ExtractEqnEditData(std::vector<string> vecFileContainer)
{
    std::map<string,string> mapFileContentsContainer = readDocFiles(vecFileContainer);
    for(std::map<string,string>::iterator it = mapFileContentsContainer.begin();
    it != mapFileContentsContainer.end(); 
    it++)
    {   
        
        std::string strScriptContext = it->second;
        
        strScriptContext.erase(remove(strScriptContext.begin(), strScriptContext.end(),'\0'),strScriptContext.end());
        string keyString = "";

        if(it->first.find(".rels") != string::npos && it->first.find(".xml") !=string::npos)
            continue;

        if(int(u_char(strScriptContext[0])) != 208 && int(u_char(strScriptContext[1])) != 207 )
            continue;

        if(strScriptContext.size() < 1310)
            continue;


        for(std::map<int,string>::iterator ut = mapRegularExpressions.begin();
            ut != mapRegularExpressions.end();
            ut++)
        {
            int vulnerabilityNumber = ut->first;
            string strRegularExpression = ut->second;
            std::smatch match;
            std::regex re(strRegularExpression,std::regex::grep | std::regex::icase);
            if(!std::regex_search(strScriptContext, match,re))
                continue;

            
            std::cout << match[0].str() << std::endl;
            return vulnerabilityNumber;
        }
    }
    return NORMAL_FILE;
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
    std::map<string,string> mapFileContentsContainer;
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
        mapFileContentsContainer.insert(std::pair(name,file));

    }

    
    return mapFileContentsContainer;
}