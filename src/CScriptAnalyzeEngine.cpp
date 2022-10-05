#include "CScriptAnalyzeEngine.h"

// 엔진 객체 생성자
CScriptAnalyzeEngine::CScriptAnalyzeEngine() : CEngineSuper(4)
{

}

// 엔진 객체 소멸자
CScriptAnalyzeEngine::~CScriptAnalyzeEngine()
{

}

// 추출된 스크립트에서 악성 행위를 판단하는 함수
bool CScriptAnalyzeEngine::Analyze(ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output)
{
    std::cout << "분석엔진 시작" << std::endl;
    for(int i =0; i< input->vecScriptFIles.size(); i++)
        checkFollina(input->vecScriptFIles[i], output->vecBehaviors);

    return true;    
}

bool CScriptAnalyzeEngine::checkFollina(std::string script, std::vector<ST_BEHAVIOR>& vecBehaviors)
{
    // Follina 전용
    if(script.find("ms-msdt:") != std::string::npos)
    {
        ST_BEHAVIOR msdt;
        msdt.nSeverity=2;
        msdt.strName="Call msdt Function";
        msdt.strDesc="msdt 명령어 호출";
        vecBehaviors.push_back(msdt);
    }
    
    if(script.find("IT_RebrowseForFile") != std::string::npos)
    {
        ST_BEHAVIOR msdt;
        msdt.nSeverity=9;
        msdt.strName="Call IT_RebrowseForFile";
        msdt.strDesc="원격으로 실행할 프로그램을 선택할 떄 사용하는 매개변수이다.";
        vecBehaviors.push_back(msdt);
    }

    if(script.find("IT_LanuchMethod") != std::string::npos)
    {
        ST_BEHAVIOR msdt;
        msdt.nSeverity=7;
        msdt.strName="Call IT_LanuchMethod";
        msdt.strDesc="프로그램 선택과 관련이 있으며, 자동으로 선택할 프로그램 없음을 설정";
        vecBehaviors.push_back(msdt);
    }
    
    if(script.find("IT_SelectProgram") != std::string::npos)
    {
        ST_BEHAVIOR msdt;
        msdt.nSeverity=7;
        msdt.strName="Call IT_SelectProgram";
        msdt.strDesc="사전에 실행할 프로그램을 선택하여 프로그램 선택창을 우회할 수 있게 한다.";
        vecBehaviors.push_back(msdt);
    }

    if(script.find("IT_BrowseForFile") != std::string::npos)
    {
        ST_BEHAVIOR msdt;
        msdt.nSeverity=9;
        msdt.strName="Call IT_BrowseForFile";
        msdt.strDesc="실행할 프로그램을 선택할 떄 사용하는 매계변수이다.";
        vecBehaviors.push_back(msdt);
    }

    if(script.find("IT_AutoTrobleshoout") != std::string::npos)
    {
        ST_BEHAVIOR msdt;
        msdt.nSeverity=9;
        msdt.strName="Call IT_AutoTrobleshoout";
        msdt.strDesc="취약점 유발과 무관하나, 자동으로 문제해결 기능을 우회할 수 있게 한다.";
        vecBehaviors.push_back(msdt);
    }

    return true;
}
