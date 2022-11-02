#include "CXMLAnalyzeModule.h"

CXMLAnalyzeModule::CXMLAnalyzeModule()
{

    regularExpressions.insert(std::pair(EQUATION_EDITOR_VOL,"RegularExpression"));
    //분석 완료시 추가될 부분

}

bool CXMLAnalyzeModule::Analyze(string xmls, ST_ANALYZE_RESULT* output)
{
    int AnalyzeResult;

    if(!decodeScript(xmls))
    {
        std::cout << "Fail script decode" <<std::endl;
        return false;
    }

    AnalyzeResult = AnalyzeByRegex();

    if(returnResult(AnalyzeResult,output)) return true;


    return false;
}


bool CXMLAnalyzeModule::decodeScript(string xmls)
{
    //분석 완료 시 추가 예정
}

int CXMLAnalyzeModule::AnalyzeByRegex()
{

    for(std::map<int,string>::iterator it = regularExpressions.begin();
        it != regularExpressions.end();
        it++)
    {
        std::regex re(it->second);
        if(std::regex_match(decodingScript, re)) return it->first;
    }

    return NORMAL_FILE;

}

bool CXMLAnalyzeModule::returnResult(int AnalyzeResult, ST_ANALYZE_RESULT* output)
{
    switch(AnalyzeResult)
    {
        case EQUATION_EDITOR_VOL:
            //수식편집기 취약점 output Behaviors에 입력
            // case 추가
        default:
            std::cout << "Normal File" << std::endl; 
            return false;  
    }

    return true;
}