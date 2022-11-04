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
bool CScriptAnalyzeEngine::Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output)
{
    for(int i =0; i< input->vecScriptFIles.size(); i++)
    {
        int location =  input->vecScriptFIles[i].second.first;
        switch (input->vecScriptFIles[i].second.second)
        {
        case JS:
            checkFollina(input->vecScriptFIles[i].first, input->vecURLs[location], output->vecBehaviors);
            break;
        
        case PS:
            break;
        
        case VBS:
            checkMacro(input->vecScriptFIles[i].first, output->vecBehaviors);
            break;
        
        default:
            throw engine_Exception("ScriptAnalyze", "s", "현재 지원하지 않는 타입의 스크립트 입니다.");
            break;
        }
    }
    return true;    
}

bool CScriptAnalyzeEngine::checkFollina(std::string script, std::string url ,std::vector<ST_BEHAVIOR>& vecBehaviors)
{
    // 검사하는 부분을 정규표현식으로 변경하기
    // Follina 전용
    if(script.find("ms-msdt:") != std::string::npos)
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity=7;
        msdt.strName="Call msdt Function";
        msdt.strDesc="msdt 명령어 호출";
        vecBehaviors.push_back(msdt);
    }
    
    if(script.find("IT_RebrowseForFile") != std::string::npos)
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity=9;
        msdt.strName="Use IT_RebrowseForFile";
        msdt.strDesc="원격으로 실행할 프로그램을 선택할 떄 사용하는 매개변수이다.";
        vecBehaviors.push_back(msdt);
    }

    if(script.find("IT_LaunchMethod") != std::string::npos)
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity=8;
        msdt.strName="Use IT_LaunchMethod";
        msdt.strDesc="프로그램 선택과 관련이 있으며, 자동으로 선택할 프로그램 없음을 설정";
        vecBehaviors.push_back(msdt);
    }
    
    if(script.find("IT_SelectProgram") != std::string::npos)
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity=8;
        msdt.strName="Use IT_SelectProgram";
        msdt.strDesc="사전에 실행할 프로그램을 선택하여 프로그램 선택창을 우회할 수 있게 한다.";
        vecBehaviors.push_back(msdt);
    }

    if(script.find("IT_BrowseForFile") != std::string::npos)
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity=9;
        msdt.strName="Use IT_BrowseForFile";
        msdt.strDesc="실행할 프로그램을 선택할 떄 사용하는 매개변수이다.";
        vecBehaviors.push_back(msdt);
    }

    if(script.find("IT_AutoTroubleshoot") != std::string::npos)
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity=9;
        msdt.strName="Use IT_AutoTroubleshoot";
        msdt.strDesc="취약점 유발과 무관하나, 자동으로 문제해결 기능을 우회할 수 있게 한다.";
        vecBehaviors.push_back(msdt);
    }

    return true;
}

bool CScriptAnalyzeEngine::checkMacro(std::string script, std::vector<ST_BEHAVIOR>& vecBehaiors)
{
    /* 
        악성매크로 의심 리스트 -> 정규표현식 리스트로 변경해야함.
        "AutoOpen", "Workbook_Open", "Document_Open", "DocumentOpen", 
        "AutoExec", "AutoExit", "Auto_Close", "AutoClose", "DocumentChange",
        "AutoNew", "Document_New", "NewDocument", "CreateObject()", "allocateMemory",
        "copyMemory", "shellExecute"
    */

    std::regex macroAnomal(R"(AutoOpen)");
    // 해당 리스트에 맞게 하나씩 검사하며 결과 전송.
    std::smatch match;
    if(std::regex_search(script, match, macroAnomal))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append("Local Macro");
        autoOpen.Severity=7;
        autoOpen.strName="Calling AutoOpen function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 AutoOpen 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }
    
    std::regex macroAnomal(R"(Workbook_Open)");
    if(std::regex_search(script, match, macroAnomal))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append("Local Macro");
        autoOpen.Severity=7;
        autoOpen.strName="Calling Workbook_Open function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 Workbook_Open 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }

    std::regex macroAnomal(R"(Document_Open)");
    if(std::regex_search(script, match, macroAnomal))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append("Local Macro");
        autoOpen.Severity=7;
        autoOpen.strName="Calling Document_Open function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 Document_Open 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }

    std::regex macroAnomal(R"(DocumentOpen)");
    if(std::regex_search(script, match, macroAnomal))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append("Local Macro");
        autoOpen.Severity=7;
        autoOpen.strName="Calling a function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }

    std::regex macroAnomal(R"(AutoExec)");
    if(std::regex_search(script, match, macroAnomal))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append("Local Macro");
        autoOpen.Severity=10;
        autoOpen.strName="Calling AutoExec function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 AutoExec 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }

    std::regex macroAnomal(R"(AutoExit)");
    if(std::regex_search(script, match, macroAnomal))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append("Local Macro");
        autoOpen.Severity=9;
        autoOpen.strName="Calling AutoExit function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 AutoExit 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }

    std::regex macroAnomal(R"(Auto_Close)");
    if(std::regex_search(script, match, macroAnomal))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append("Local Macro");
        autoOpen.Severity=7;
        autoOpen.strName="Calling Auto_Close function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 Auto_Close 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }

    std::regex macroAnomal(R"(AutoClose)");
    if(std::regex_search(script, match, macroAnomal))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append("Local Macro");
        autoOpen.Severity=7;
        autoOpen.strName="Calling AutoClose function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 AutoClose 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }

    std::regex macroAnomal(R"(DocumentChange)");
    if(std::regex_search(script, match, macroAnomal))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append("Local Macro");
        autoOpen.Severity=9;
        autoOpen.strName="Calling DocumentChange function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 DocumentChange 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }

    std::regex macroAnomal(R"(AutoNew)");
    if(std::regex_search(script, match, macroAnomal))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append("Local Macro");
        autoOpen.Severity=10;
        autoOpen.strName="Calling AutoNew function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 AutoNew 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }

    std::regex macroAnomal(R"(Document_New)");
    if(std::regex_search(script, match, macroAnomal))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append("Local Macro");
        autoOpen.Severity=9;
        autoOpen.strName="Calling a function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }

    std::regex macroAnomal(R"(NewDocument)");
    if(std::regex_search(script, match, macroAnomal))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append("Local Macro");
        autoOpen.Severity=8;
        autoOpen.strName="Calling Document_New function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 Document_New 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }

    std::regex macroAnomal(R"(CreateObject)");
    if(std::regex_search(script, match, macroAnomal))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append("Local Macro");
        autoOpen.Severity=8;
        autoOpen.strName="Calling a function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }

    std::regex macroAnomal(R"(allocateMemory)");
    if(std::regex_search(script, match, macroAnomal))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append("Local Macro");
        autoOpen.Severity=8;
        autoOpen.strName="Calling a function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }

    std::regex macroAnomal(R"(copyMemory)");
    if(std::regex_search(script, match, macroAnomal))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append("Local Macro");
        autoOpen.Severity=8;
        autoOpen.strName="Calling allocateMemory function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 allocateMemory 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }

    std::regex macroAnomal(R"(shellExecute)");
    if(std::regex_search(script, match, macroAnomal))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append("Local Macro");
        autoOpen.Severity=8;
        autoOpen.strName="Calling a function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }
}