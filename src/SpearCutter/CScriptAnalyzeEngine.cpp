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
bool CScriptAnalyzeEngine::Analyze(const ST_ANALYZE_PARAM *input, ST_ANALYZE_RESULT *output)
{

    for (int i = 0; i < input->vecScriptFIles.size(); i++)
    {
        int location = input->vecScriptFIles[i].second.first;
        std::string url;
        if (location == -1)
            url.append("Not");
        else
            url.append(input->vecURLs[location]);

        switch (input->vecScriptFIles[i].second.second)
        {
        case JS:
            checkFollina(input->vecScriptFIles[i].first, url, output->vecBehaviors);
            break;

        case PS:
            break;

        case VBS:
            checkVBAMacro(input->vecScriptFIles[i].first, url, output->vecBehaviors);
            checkXLMMacro(input->vecScriptFIles[i].first, url, output->vecBehaviors);
            break;

        default:
            throw engine_Exception("ScriptAnalyze", "s", "현재 지원하지 않는 타입의 스크립트 입니다.");
            break;
        }
    }

    return true;
}

bool CScriptAnalyzeEngine::checkFollina(std::string script, std::string url, std::vector<ST_BEHAVIOR> &vecBehaviors)
{
    // 탐지 됬는지 확인하기 위한 변수
    int count = 0;
    std::smatch match;

    std::regex follina0(R"(ms-msdt:[A-Za-z0-9\s\/\\]*)");
    if (std::regex_search(script, match, follina0))
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity = 7;
        msdt.strName = match.str();
        msdt.strDesc = "msdt 명령어 호출";
        vecBehaviors.push_back(msdt);
        count++;
    }

    std::regex follina1(R"(IT_RebrowseForFile=[A-Za-z0-9_!@#$%^&*()_+=:;"'<>.,/?-]*)");
    if (std::regex_search(script, match, follina1))
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity = 9;
        msdt.strName = match.str();
        msdt.strDesc = "원격으로 실행할 프로그램을 선택할 떄 사용하는 매개변수이다.";
        vecBehaviors.push_back(msdt);
        count++;
    }

    std::regex follina2(R"(IT_LaunchMethod=[A-Za-z0-9_!@#$%^&*()_+=:;"'<>.,/?-]*)");
    if (std::regex_search(script, match, follina2))
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity = 8;
        msdt.strName = match.str();
        msdt.strDesc = "프로그램 선택과 관련이 있으며, 자동으로 선택할 프로그램 없음을 설정";
        vecBehaviors.push_back(msdt);
        count++;
    }

    std::regex follina3(R"(IT_SelectProgram=[A-Za-z0-9_!@#$%^&*()_+=:;"'<>.,/?-]*)");
    if (std::regex_search(script, match, follina3))
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity = 8;
        msdt.strName = match.str();
        msdt.strDesc = "사전에 실행할 프로그램을 선택하여 프로그램 선택창을 우회할 수 있게 한다.";
        vecBehaviors.push_back(msdt);
    }

    std::regex follina4(R"(IT_BrowseForFile=[A-Za-z0-9!@#$%^&*():;_+='"`~|/.\[\]-]*)");
    if (std::regex_search(script, match, follina4))
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity = 9;
        msdt.strName = match.str();
        msdt.strDesc = "실행할 프로그램을 선택할 떄 사용하는 매개변수이다.";
        vecBehaviors.push_back(msdt);
        count++;
    }

    std::regex follina5(R"(IT_AutoTroubleshoot=[A-Za-z0-9_!@#$%^&*()_+=:;"'<>.,/?-]*)");
    if (std::regex_search(script, match, follina5))
    {
        ST_BEHAVIOR msdt;
        msdt.strUrl.append(url);
        msdt.Severity = 9;
        msdt.strName = match.str();
        msdt.strDesc = "취약점 유발과 무관하나, 자동으로 문제해결 기능을 우회할 수 있게 한다.";
        vecBehaviors.push_back(msdt);
        count++;
    }

    if (count > 0)
        return true;
    return false;
}

bool CScriptAnalyzeEngine::checkVBAMacro(std::string script, std::string urlorNot, std::vector<ST_BEHAVIOR> &vecBehaviors)
{
    /*
        악성매크로 의심 리스트
        "AutoOpen", "Workbook_Open", "Document_Open", "DocumentOpen", "Open"(제외), "auto_open()"
        "AutoExec", "AutoExit", "Auto_Close", "AutoClose", "DocumentChange",
        "AutoNew", "Document_New", "NewDocument", "CreateObject()", "allocateMemory",
        "copyMemory", "shellExecute", "Environment", "Write", "Put", "Binary", "Shell"
    */
    // 탐지 됬는지 확인하기 위한 변수
    int count = 0;
    std::smatch match;

    std::string url;
    if (urlorNot == "Not")
        url.append("Local Macro");
    else
        url.append(urlorNot);

    // 어디서 나온 매크로인지 알려주기 위해 사용
    std::string macroLocation;
    std::regex OLERe(R"(OLE stream: '[\s\w0-9!@#$%^&*()_<>,.?/:;"\[\]\{\}-]*')");
    std::regex_search(script, match, OLERe);

    int firstQuote = match.str().find('\'') + 1;
    int lastQute = match.str().size() - 1;
    macroLocation.append(match.str().substr(firstQuote, lastQute - firstQuote));

    std::regex macro0(R"(AutoOpen)");
    // 해당 리스트에 맞게 하나씩 검사하며 결과 전송.
    if (std::regex_search(script, match, macro0))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append(url);
        autoOpen.Severity = 7;
        autoOpen.strName = "Calling AutoOpen function used in a malicious macro";
        autoOpen.strDesc = macroLocation.c_str();
        autoOpen.strDesc.append(" 매크로 파일에서 악성 매크로에서 사용되는 AutoOpen 함수를 호출");
        vecBehaviors.push_back(autoOpen);
        count++;
    }

    std::regex macro1(R"(Workbook_Open)");
    if (std::regex_search(script, match, macro1))
    {
        ST_BEHAVIOR Workbook_Open;
        Workbook_Open.strUrl.append(url);
        Workbook_Open.Severity = 7;
        Workbook_Open.strName = "Calling Workbook_Open function used in a malicious macro";
        Workbook_Open.strDesc = macroLocation.c_str();
        Workbook_Open.strDesc.append(" 매크로 파일에서 악성 매크로에서 사용되는 Workbook_Open 함수를 호출");
        vecBehaviors.push_back(Workbook_Open);
        count++;
    }

    std::regex macro2(R"(Document_Open)");
    if (std::regex_search(script, match, macro2))
    {
        ST_BEHAVIOR Document_Open;
        Document_Open.strUrl.append(url);
        Document_Open.Severity = 7;
        Document_Open.strName = "Calling Document_Open function used in a malicious macro";
        Document_Open.strDesc = macroLocation.c_str();
        Document_Open.strDesc.append(" 매크로 파일에서 악성 매크로에서 사용되는 Document_Open 함수를 호출");
        vecBehaviors.push_back(Document_Open);
        count++;
    }

    std::regex macro3(R"(DocumentOpen)");
    if (std::regex_search(script, match, macro3))
    {
        ST_BEHAVIOR DocumentOpen;
        DocumentOpen.strUrl.append(url);
        DocumentOpen.Severity = 7;
        DocumentOpen.strName = "Calling a function used in a malicious macro";
        DocumentOpen.strDesc = macroLocation.c_str();
        DocumentOpen.strDesc.append(" 매크로 파일에서 악성 매크로에서 사용되는 함수를 호출");
        vecBehaviors.push_back(DocumentOpen);
        count++;
    }

    std::regex macro4(R"(AutoExec)");
    if (std::regex_search(script, match, macro4))
    {
        ST_BEHAVIOR AutoExec;
        AutoExec.strUrl.append(url);
        AutoExec.Severity = 10;
        AutoExec.strName = "Calling AutoExec function used in a malicious macro";
        AutoExec.strDesc = macroLocation.c_str();
        AutoExec.strDesc.append(" 매크로 파일에서 악성 매크로에서 사용되는 AutoExec 함수를 호출");
        vecBehaviors.push_back(AutoExec);
        count++;
    }

    std::regex macro5(R"(AutoExit)");
    if (std::regex_search(script, match, macro5))
    {
        ST_BEHAVIOR AutoExit;
        AutoExit.strUrl.append(url);
        AutoExit.Severity = 9;
        AutoExit.strName = "Calling AutoExit function used in a malicious macro";
        AutoExit.strDesc = macroLocation.c_str();
        AutoExit.strDesc.append(" 매크로 파일에서 악성 매크로에서 사용되는 AutoExit 함수를 호출");
        vecBehaviors.push_back(AutoExit);
        count++;
    }

    std::regex macro6(R"(Auto_Close)");
    if (std::regex_search(script, match, macro6))
    {
        ST_BEHAVIOR Auto_Close;
        Auto_Close.strUrl.append(url);
        Auto_Close.Severity = 7;
        Auto_Close.strName = "Calling Auto_Close function used in a malicious macro";
        Auto_Close.strDesc = macroLocation.c_str();
        Auto_Close.strDesc.append(" 매크로 파일에서 악성 매크로에서 사용되는 Auto_Close 함수를 호출");
        vecBehaviors.push_back(Auto_Close);
        count++;
    }

    std::regex macro7(R"(AutoClose)");
    if (std::regex_search(script, match, macro7))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append(url);
        autoOpen.Severity = 7;
        autoOpen.strName = "Calling AutoClose function used in a malicious macro";
        autoOpen.strDesc = macroLocation.c_str();
        autoOpen.strDesc.append(" 매크로 파일에서 악성 매크로에서 사용되는 AutoClose 함수를 호출");
        vecBehaviors.push_back(autoOpen);
        count++;
    }

    std::regex macro8(R"(DocumentChange)");
    if (std::regex_search(script, match, macro8))
    {
        ST_BEHAVIOR DocumentChange;
        DocumentChange.strUrl.append(url);
        DocumentChange.Severity = 9;
        DocumentChange.strName = "Calling DocumentChange function used in a malicious macro";
        DocumentChange.strDesc = macroLocation.c_str();
        DocumentChange.strDesc.append(" 매크로 파일에서 악성 매크로에서 사용되는 DocumentChange 함수를 호출");
        vecBehaviors.push_back(DocumentChange);
        count++;
    }

    std::regex macro9(R"(AutoNew)");
    if (std::regex_search(script, match, macro9))
    {
        ST_BEHAVIOR AutoNew;
        AutoNew.strUrl.append(url);
        AutoNew.Severity = 10;
        AutoNew.strName = "Calling AutoNew function used in a malicious macro";
        AutoNew.strDesc = macroLocation.c_str();
        AutoNew.strDesc.append(" 매크로 파일에서 악성 매크로에서 사용되는 AutoNew 함수를 호출");
        vecBehaviors.push_back(AutoNew);
        count++;
    }

    std::regex macro10(R"(Document_New)");
    if (std::regex_search(script, match, macro10))
    {
        ST_BEHAVIOR Document_New;
        Document_New.strUrl.append(url);
        Document_New.Severity = 9;
        Document_New.strName = "Calling a function used in a malicious macro";
        Document_New.strDesc = macroLocation.c_str();
        Document_New.strDesc.append(" 매크로 파일에서 악성 매크로에서 사용되는 함수를 호출");
        vecBehaviors.push_back(Document_New);
        count++;
    }

    std::regex macro11(R"(NewDocument)");
    if (std::regex_search(script, match, macro11))
    {
        ST_BEHAVIOR NewDocument;
        NewDocument.strUrl.append(url);
        NewDocument.Severity = 8;
        NewDocument.strName = "Calling Document_New function used in a malicious macro";
        NewDocument.strDesc = macroLocation.c_str();
        NewDocument.strDesc.append(" 매크로 파일에서 악성 매크로에서 사용되는 Document_New 함수를 호출");
        vecBehaviors.push_back(NewDocument);
        count++;
    }

    std::regex macro12(R"(CreateObject)");
    if (std::regex_search(script, match, macro12))
    {
        ST_BEHAVIOR CreateObject;
        CreateObject.strUrl.append(url);
        CreateObject.Severity = 8;
        CreateObject.strName = "Calling CreateObject function used in a malicious macro";
        CreateObject.strDesc = macroLocation.c_str();
        CreateObject.strDesc.append(" 매크로 파일에서 악성 매크로에서 사용되는 CreateObject 함수를 호출");
        vecBehaviors.push_back(CreateObject);
        count++;
    }

    std::regex macro13(R"(allocateMemory)");
    if (std::regex_search(script, match, macro13))
    {
        ST_BEHAVIOR allocateMemory;
        allocateMemory.strUrl.append(url);
        allocateMemory.Severity = 8;
        allocateMemory.strName = "Calling allocateMemory function used in a malicious macro";
        allocateMemory.strDesc = macroLocation.c_str();
        allocateMemory.strDesc.append(" 매크로 파일에서 악성 매크로에서 사용되는 allocateMemory 함수를 호출");
        vecBehaviors.push_back(allocateMemory);
        count++;
    }

    std::regex macro14(R"(copyMemory)");
    if (std::regex_search(script, match, macro14))
    {
        ST_BEHAVIOR copyMemory;
        copyMemory.strUrl.append(url);
        copyMemory.Severity = 8;
        copyMemory.strName = "Calling copyMemory function used in a malicious macro";
        copyMemory.strDesc = macroLocation.c_str();
        copyMemory.strDesc.append(" 매크로 파일에서 악성 매크로에서 사용되는 copyMemory 함수를 호출");
        vecBehaviors.push_back(copyMemory);
        count++;
    }

    std::regex macro15(R"(shellExecute)");
    if (std::regex_search(script, match, macro15))
    {
        ST_BEHAVIOR shellExecute;
        shellExecute.strUrl.append(url);
        shellExecute.Severity = 8;
        shellExecute.strName = "Calling shellExecute function used in a malicious macro";
        shellExecute.strDesc = macroLocation.c_str();
        shellExecute.strDesc.append(" 매크로 파일에서 악성 매크로에서 사용되는 shellExecute 함수를 호출");
        vecBehaviors.push_back(shellExecute);
        count++;
    }

    std::regex macro16(R"(.Shell)");
    if (std::regex_search(script, match, macro16))
    {
        ST_BEHAVIOR Shell;
        Shell.strUrl.append(url);
        Shell.Severity = 8;
        Shell.strName = "Calling Shell function used in a malicious macro";
        Shell.strDesc = macroLocation.c_str();
        Shell.strDesc.append("매크로 파일에서 악성 매크로에서 사용되는 Shell 함수를 호출");
        vecBehaviors.push_back(Shell);
        count++;
    }

    std::regex macro17(R"(\.Open)");
    if (std::regex_search(script, match, macro17))
    {
        ST_BEHAVIOR Open;
        Open.strUrl.append(url);
        Open.Severity = 8;
        Open.strName = "Calling Open function used in a malicious macro";
        Open.strDesc = macroLocation.c_str();
        Open.strDesc.append("매크로 파일에서 사용되는 Open 함수를 호출");
        vecBehaviors.push_back(Open);
        count++;
    }

    std::regex macro18(R"(Environment)");
    if(std::regex_search(script, match, macro16))
    {
        ST_BEHAVIOR Environment;
        Environment.strUrl.append(url);
        Environment.Severity=8;
        Environment.strName="Calling Environ function used in a malicious macro";
        Environment.strDesc = macroLocation.c_str();
        Environment.strDesc.append("매크로 파일에서 시스템 환경 정보를 읽는데 사용되는 Environment 함수를 호출");
        vecBehaviors.push_back(Environment);
        count++;
    }
    // std::regex macro16(R"(auto_open())");
    // if(std::regex_search(script, match, macro16))
    // {
    //     ST_BEHAVIOR auto_open;
    //     auto_open.strUrl.append(url);
    //     auto_open.Severity=8;
    //     auto_open.strName="Calling auto_open function used in a malicious macro";
    //     auto_open.strDesc="악성 매크로에서 사용되는 auto_open 함수를 호출";
    //     vecBehaviors.push_back(auto_open);
    //     count++;
    // }

    // std::regex macro17(R"(auto_close())");
    // if(std::regex_search(script, match, macro16))
    // {
    //     ST_BEHAVIOR auto_close;
    //     auto_close.strUrl.append(url);
    //     auto_close.Severity=8;
    //     auto_close.strName="Calling auto_close function used in a malicious macro";
    //     auto_close.strDesc="악성 매크로에서 사용되는 auto_close 함수를 호출";
    //     vecBehaviors.push_back(auto_close);
    //     count++;
    // }

    

    // std::regex macro18(R"(Write)");
    // if(std::regex_search(script, match, macro18))
    // {
    //     ST_BEHAVIOR Write;
    //     Write.strUrl.append(url);
    //     Write.Severity=8;
    //     Write.strName="Calling Write function used in a malicious macro";
    //     Write.strDesc="악성 매크로에서 사용되는 Write 함수를 호출";
    //     vecBehaviors.push_back(Write);
    //     count++;
    // }

    // std::regex macro19(R"(Put)");
    // if(std::regex_search(script, match, macro19))
    // {
    //     ST_BEHAVIOR Put;
    //     Put.strUrl.append(url);
    //     Put.Severity=8;
    //     Put.strName="Calling Put function used in a malicious macro";
    //     Put.strDesc="악성 매크로에서 사용되는 Put 함수를 호출";
    //     vecBehaviors.push_back(Put);
    //     count++;
    // }

    // std::regex macro20(R"(Binary)");
    // if(std::regex_search(script, match, macro20))
    // {
    //     ST_BEHAVIOR Binary;
    //     Binary.strUrl.append(url);
    //     Binary.Severity=8;
    //     Binary.strName="Calling Binary function used in a malicious macro";
    //     Binary.strDesc="악성 매크로에서 사용되는 Binary 함수를 호출";
    //     vecBehaviors.push_back(Binary);
    //     count++;
    // }

    if (count > 0)
        return true;
    return false;
}

bool CScriptAnalyzeEngine::checkXLMMacro(std::string script, std::string urlorNot, std::vector<ST_BEHAVIOR> &vecBehaviors)
{
    // 탐지 됬는지 확인하기 위한 변수
    int count = 0;
    std::smatch match;

    std::string url;
    if (urlorNot == "Not")
        url.append("Local Macro");
    else
        url.append(urlorNot);

    // 어디서 나온 매크로인지 알려주기 위해 사용
    std::string macroLocation;
    std::regex OLERe(R"(OLE stream: '[\s\w0-9!@#$%^&*()_<>,.?/:;"\[\]\{\}-]*')");
    std::regex_search(script, match, OLERe);

    int firstQuote = match.str().find('\'') + 1;
    int lastQute = match.str().size() - 1;
    macroLocation.append(match.str().substr(firstQuote, lastQute - firstQuote));

    std::regex xlm0(R"((u|U)(r|R)(l|L)(m|M)(o|O)(n|N))");
    if (std::regex_search(script, match, xlm0))
    {
        ST_BEHAVIOR URLMon;
        URLMon.strUrl.append(url);
        URLMon.Severity = 7;
        URLMon.strName = "Use URLMon Lib";
        URLMon.strDesc = macroLocation.c_str();
        URLMon.strDesc.append(" 매크로 파일에 외부파일 다운에 사용되는 URLMon 라이브러리를 호출");
        vecBehaviors.push_back(URLMon);
        count++;
    }

    std::regex xlm1(R"(URLDownloadToFileA)");
    // 해당 리스트에 맞게 하나씩 검사하며 결과 전송.
    if (std::regex_search(script, match, xlm1))
    {
        ST_BEHAVIOR URLDownload;
        URLDownload.strUrl.append(url);
        URLDownload.Severity = 7;
        URLDownload.strName = "Use URLDownloadToFile";
        URLDownload.strDesc = macroLocation;
        URLDownload.strDesc.append(" 매크로 파일에 파일 다운에 사용되는 URLDownloadToFile 함수를 호출");
        vecBehaviors.push_back(URLDownload);
        count++;
    }

    std::regex xlm2(R"(https?[\w.%/?=:-]*)");
    if (std::regex_search(script, match, xlm2))
    {
        // 다운받는 파일에 대해 추출하기 위해
        int loc = match.str().find_last_of('/');
        int size = match.str().size();
        std::string downFile = match.str().substr(loc + 1, size);

        ST_BEHAVIOR URL;
        URL.strUrl.append(url);
        URL.Severity = 9;
        URL.strName = "Download File From Url";
        URL.strDesc = macroLocation.c_str();
        URL.strDesc.append(" 매크로 파일에 다음 ");
        URL.strDesc.append(downFile);
        URL.strDesc.append("파일을 다운받기 위한 URL을 사용되었습니다.");
        vecBehaviors.push_back(URL);
        count++;
    }

    std::regex xlm3(R"(run)");
    if (std::regex_search(script, match, xlm3))
    {
        ST_BEHAVIOR run;
        run.strUrl.append(url);
        run.Severity = 6;
        run.strName = "Call run";
        run.strDesc = macroLocation.c_str();
        run.strDesc.append(" 매크로 파일에 악성 파일 또는 스크립트 등을 실행하기 위한 명령어가 사용되었습니다.");
        vecBehaviors.push_back(run);
        count++;
    }

    std::regex xlm4(R"(EXEC)");
    if (std::regex_search(script, match, xlm4))
    {
        ST_BEHAVIOR EXEC;
        EXEC.strUrl.append(url);
        EXEC.Severity = 6;
        EXEC.strName = "Call EXEC";
        EXEC.strDesc = macroLocation.c_str();
        EXEC.strDesc.append(" 매크로 파일에 파일 또는 악성행위를 실행하기 위한 명령어가 사용되었습니다.");
        vecBehaviors.push_back(EXEC);
        count++;
    }

    std::regex xlm5(R"(CALL\()");
    if (std::regex_search(script, match, xlm5))
    {
        ST_BEHAVIOR CALL;
        CALL.strUrl.append(url);
        CALL.Severity = 6;
        CALL.strName = "Call CALL";
        CALL.strDesc = macroLocation.c_str();
        CALL.strDesc.append(" 매크로 파일에 악성 파일을 실행하기 위해 호출되는 키워드를 사용했습니다.");
        vecBehaviors.push_back(CALL);
        count++;
    }
    return true;
}