#include "CScriptAnalyzeEngine.h"

// 엔진 객체 생성자
CScriptAnalyzeEngine::CScriptAnalyzeEngine() : CEngineSuper(4, "ScriptAnalyze")
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

        std::string url;    
        if(location == -1)
            url.append("Not");
        url.append(input->vecURLs[location]);

        switch (input->vecScriptFIles[i].second.second)
        {
        case JS:
            checkFollina(input->vecScriptFIles[i].first, url, output->vecBehaviors);
            break;
        
        case PS:
            break;
        
        case VBS:
            checkMacro(input->vecScriptFIles[i].first, url, output->vecBehaviors);
            break;
        
        default:
            throw engine_Exception("ScriptAnalyze", "s", "현재 지원하지 않는 타입의 스크립트 입니다.");
            break;
        }
    }

    // 만약 분석 결과가 없다면 예외 발생.
    if(output->vecBehaviors.size() == 0)
        throw engine_Exception("ScriptAnalyze","s","현재 엔진의 분석 결과 탐지된 내용이 없습니다.");

    return true;    
}

bool CScriptAnalyzeEngine::checkFollina(std::string script, std::string url ,std::vector<ST_BEHAVIOR>& vecBehaviors)
{
    // 탐지 됬는지 확인하기 위한 변수
    int count = 0;
    std::smatch match;

    std::regex follina0(R"(ms-msdt:[A-Za-z0-9\s\/\\]*)");
    if(std::regex_search(script, match, follina0))
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity=7;
        msdt.strName=match.str();
        msdt.strDesc="msdt 명령어 호출";
        vecBehaviors.push_back(msdt);
        count++;
    }
    
    std::regex follina1(R"(IT_RebrowseForFile=[A-Za-z0-9_!@#$%^&*()_+=:;"'<>.,/?-]*)");
    if(std::regex_search(script, match, follina1))
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity=9;
        msdt.strName=match.str();
        msdt.strDesc="원격으로 실행할 프로그램을 선택할 떄 사용하는 매개변수이다.";
        vecBehaviors.push_back(msdt);
        count++;
    }

    std::regex follina2(R"(IT_LaunchMethod=[A-Za-z0-9_!@#$%^&*()_+=:;"'<>.,/?-]*)");
    if(std::regex_search(script, match, follina2))
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity=8;
        msdt.strName=match.str();
        msdt.strDesc="프로그램 선택과 관련이 있으며, 자동으로 선택할 프로그램 없음을 설정";
        vecBehaviors.push_back(msdt);
        count++;
    }
    
    std::regex follina3(R"(IT_SelectProgram=[A-Za-z0-9_!@#$%^&*()_+=:;"'<>.,/?-]*)");
    if(std::regex_search(script, match, follina3))
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity=8;
        msdt.strName=match.str();
        msdt.strDesc="사전에 실행할 프로그램을 선택하여 프로그램 선택창을 우회할 수 있게 한다.";
        vecBehaviors.push_back(msdt);
    }

    std::regex follina4(R"(IT_BrowseForFile=[A-Za-z0-9!@#$%^&*():;_+='"`~|/.\[\]-]*)");
    if(std::regex_search(script, match, follina4))
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity=9;
        msdt.strName=match.str();
        msdt.strDesc="실행할 프로그램을 선택할 떄 사용하는 매개변수이다.";
        vecBehaviors.push_back(msdt);
        count++;
    }

    std::regex follina5(R"(IT_AutoTroubleshoot=[A-Za-z0-9_!@#$%^&*()_+=:;"'<>.,/?-]*)");
    if(std::regex_search(script, match, follina5))
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity=9;
        msdt.strName=match.str();
        msdt.strDesc="취약점 유발과 무관하나, 자동으로 문제해결 기능을 우회할 수 있게 한다.";
        vecBehaviors.push_back(msdt);
        count++;
    }

    if(count > 0)
        return true;
    return false;
}

bool CScriptAnalyzeEngine::checkMacro(std::string script, std::string urlorNot, std::vector<ST_BEHAVIOR>& vecBehaiors)
{
    /* 
        악성매크로 의심 리스트
        "AutoOpen", "Workbook_Open", "Document_Open", "DocumentOpen", 
        "AutoExec", "AutoExit", "Auto_Close", "AutoClose", "DocumentChange",
        "AutoNew", "Document_New", "NewDocument", "CreateObject()", "allocateMemory",
        "copyMemory", "shellExecute"
    */
    // 탐지 됬는지 확인하기 위한 변수
    int count = 0;
    std::smatch match;
    std::string url;
    if(urlorNot == "Not")
        url.append("Local Macro");
    else
        url.append(urlorNot);

    std::regex macro0(R"(AutoOpen)");
    // 해당 리스트에 맞게 하나씩 검사하며 결과 전송.
    if(std::regex_search(script, match, macro0))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append(url);
        autoOpen.Severity=7;
        autoOpen.strName="Calling AutoOpen function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 AutoOpen 함수를 호출";
        vecBehaiors.push_back(autoOpen);
        count++;
    }

    std::regex macro1(R"(Workbook_Open)");
    if(std::regex_search(script, match, macro1))
    {
        ST_BEHAVIOR Workbook_Open;
        Workbook_Open.strUrl.append(url);
        Workbook_Open.Severity=7;
        Workbook_Open.strName="Calling Workbook_Open function used in a malicious macro";
        Workbook_Open.strDesc="악성 매크로에서 사용되는 Workbook_Open 함수를 호출";
        vecBehaiors.push_back(Workbook_Open);
        count++;
    }

    std::regex macro2(R"(Document_Open)");
    if(std::regex_search(script, match, macro2))
    {
        ST_BEHAVIOR Document_Open;
        Document_Open.strUrl.append(url);
        Document_Open.Severity=7;
        Document_Open.strName="Calling Document_Open function used in a malicious macro";
        Document_Open.strDesc="악성 매크로에서 사용되는 Document_Open 함수를 호출";
        vecBehaiors.push_back(Document_Open);
        count++;
    }

    std::regex macro3(R"(DocumentOpen)");
    if(std::regex_search(script, match, macro3))
    {
        ST_BEHAVIOR DocumentOpen;
        DocumentOpen.strUrl.append(url);
        DocumentOpen.Severity=7;
        DocumentOpen.strName="Calling a function used in a malicious macro";
        DocumentOpen.strDesc="악성 매크로에서 사용되는 함수를 호출";
        vecBehaiors.push_back(DocumentOpen);
        count++;
    }

    std::regex macro4(R"(AutoExec)");
    if(std::regex_search(script, match, macro4))
    {
        ST_BEHAVIOR AutoExec;
        AutoExec.strUrl.append(url);
        AutoExec.Severity=10;
        AutoExec.strName="Calling AutoExec function used in a malicious macro";
        AutoExec.strDesc="악성 매크로에서 사용되는 AutoExec 함수를 호출";
        vecBehaiors.push_back(AutoExec);
        count++;
    }

    std::regex macro5(R"(AutoExit)");
    if(std::regex_search(script, match, macro5))
    {
        ST_BEHAVIOR AutoExit;
        AutoExit.strUrl.append(url);
        AutoExit.Severity=9;
        AutoExit.strName="Calling AutoExit function used in a malicious macro";
        AutoExit.strDesc="악성 매크로에서 사용되는 AutoExit 함수를 호출";
        vecBehaiors.push_back(AutoExit);
        count++;
    }

    std::regex macro6(R"(Auto_Close)");
    if(std::regex_search(script, match, macro6))
    {
        ST_BEHAVIOR Auto_Close;
        Auto_Close.strUrl.append(url);
        Auto_Close.Severity=7;
        Auto_Close.strName="Calling Auto_Close function used in a malicious macro";
        Auto_Close.strDesc="악성 매크로에서 사용되는 Auto_Close 함수를 호출";
        vecBehaiors.push_back(Auto_Close);
        count++;
    }

    std::regex macro7(R"(AutoClose)");
    if(std::regex_search(script, match, macro7))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append(url);
        autoOpen.Severity=7;
        autoOpen.strName="Calling AutoClose function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 AutoClose 함수를 호출";
        vecBehaiors.push_back(autoOpen);
    }

    std::regex macro8(R"(DocumentChange)");
    if(std::regex_search(script, match, macro8))
    {
        ST_BEHAVIOR DocumentChange;
        DocumentChange.strUrl.append(url);
        DocumentChange.Severity=9;
        DocumentChange.strName="Calling DocumentChange function used in a malicious macro";
        DocumentChange.strDesc="악성 매크로에서 사용되는 DocumentChange 함수를 호출";
        vecBehaiors.push_back(DocumentChange);
        count++;
    }

    std::regex macro9(R"(AutoNew)");
    if(std::regex_search(script, match, macro9))
    {
        ST_BEHAVIOR AutoNew;
        AutoNew.strUrl.append(url);
        AutoNew.Severity=10;
        AutoNew.strName="Calling AutoNew function used in a malicious macro";
        AutoNew.strDesc="악성 매크로에서 사용되는 AutoNew 함수를 호출";
        vecBehaiors.push_back(AutoNew);
    }

    std::regex macro10(R"(Document_New)");
    if(std::regex_search(script, match, macro10))
    {
        ST_BEHAVIOR Document_New;
        Document_New.strUrl.append(url);
        Document_New.Severity=9;
        Document_New.strName="Calling a function used in a malicious macro";
        Document_New.strDesc="악성 매크로에서 사용되는 함수를 호출";
        vecBehaiors.push_back(Document_New);
        count++;
    }

    std::regex macro11(R"(NewDocument)");
    if(std::regex_search(script, match, macro11))
    {
        ST_BEHAVIOR NewDocument;
        NewDocument.strUrl.append(url);
        NewDocument.Severity=8;
        NewDocument.strName="Calling Document_New function used in a malicious macro";
        NewDocument.strDesc="악성 매크로에서 사용되는 Document_New 함수를 호출";
        vecBehaiors.push_back(NewDocument);
        count++;
    }

    std::regex macro12(R"(CreateObject)");
    if(std::regex_search(script, match, macro12))
    {
        ST_BEHAVIOR CreateObject;
        CreateObject.strUrl.append(url);
        CreateObject.Severity=8;
        CreateObject.strName="Calling CreateObject function used in a malicious macro";
        CreateObject.strDesc="악성 매크로에서 사용되는 CreateObject 함수를 호출";
        vecBehaiors.push_back(CreateObject);
        count++;
    }

    std::regex macro13(R"(allocateMemory)");
    if(std::regex_search(script, match, macro13))
    {
        ST_BEHAVIOR allocateMemory;
        allocateMemory.strUrl.append(url);
        allocateMemory.Severity=8;
        allocateMemory.strName="Calling allocateMemory function used in a malicious macro";
        allocateMemory.strDesc="악성 매크로에서 사용되는 allocateMemory 함수를 호출";
        vecBehaiors.push_back(allocateMemory);
        count++;
    }

    std::regex macro14(R"(copyMemory)");
    if(std::regex_search(script, match, macro14))
    {
        ST_BEHAVIOR copyMemory;
        copyMemory.strUrl.append(url);
        copyMemory.Severity=8;
        copyMemory.strName="Calling copyMemory function used in a malicious macro";
        copyMemory.strDesc="악성 매크로에서 사용되는 copyMemory 함수를 호출";
        vecBehaiors.push_back(copyMemory);
        count++;
    }

    std::regex macro15(R"(shellExecute)");
    if(std::regex_search(script, match, macro15))
    {
        ST_BEHAVIOR shellExecute;
        shellExecute.strUrl.append(url);
        shellExecute.Severity=8;
        shellExecute.strName="Calling shellExecute function used in a malicious macro";
        shellExecute.strDesc="악성 매크로에서 사용되는 shellExecute 함수를 호출";
        vecBehaiors.push_back(shellExecute);
        count++;
    }
    
    if(count > 0)
        return true;
    return false;
}