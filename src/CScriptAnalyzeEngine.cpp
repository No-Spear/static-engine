#include "CScriptAnalyzeEngine.h"

std::string base64_decoder(const std::string& input)
{
    static const std::string b = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
 
    std::string result;
    std::vector<int> T(256, -1);
 
    for (int i = 0; i < 64; i++)
        T[b[i]] = i;
 
    int val = 0;
    int valb = -8;
 
    for (u_char c : input) 
    {
        if (T[c] == -1)
            break;
 
        val = (val << 6) + T[c];
        valb += 6;
 
        if (valb >= 0) 
        {
            result.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return result;
}

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

bool CScriptAnalyzeEngine::checkVBAMacro(std::string script, std::string urlorNot, std::vector<ST_BEHAVIOR>& vecBehaviors)
{
    /* 
        악성매크로 의심 리스트
        "AutoOpen", "Workbook_Open", "Document_Open", "DocumentOpen", "Open"
        "AutoExec", "AutoExit", "Auto_Close", "AutoClose", "DocumentChange",
        "AutoNew", "Document_New", "NewDocument", "CreateObject()", "allocateMemory",
        "copyMemory", "shellExecute", "Environ", "Write", "Put", "Binary", "Shell"
    */
    // 탐지 됬는지 확인하기 위한 변수
    int count = 0;
    std::smatch match;

    std::string url;
    if(urlorNot == "Not")
        url.append("Local Macro");
    else
        url.append(urlorNot);

    std::string replaceScript = script;

    std::regex sBytes(R"(sBytes = )");
    if(std::regex_search(replaceScript, match, sBytes))
    {
        // sBytes = sByte 를 모두 제거
        std::regex remove(R"(\nsBytes = sBytes  )");
        replaceScript = std::regex_replace(replaceScript, remove, "");

        // 제거된 내용에서 sByte에 해당하는 내용을 찾아낸다.
        std::regex Encode(R"(sBytes = [\w]*)");
        std::regex_search(replaceScript, match, Encode);

        std::string sbyte = match.str();
        // sByte = 을 제거
        sbyte.erase(0,9);
        sbyte = base64_decoder(sbyte);

        replaceScript = std::regex_replace(replaceScript, Encode, sbyte);
    }

    std::regex macro0(R"(AutoOpen)");
    // 해당 리스트에 맞게 하나씩 검사하며 결과 전송.
    if(std::regex_search(replaceScript, match, macro0))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append(url);
        autoOpen.Severity=7;
        autoOpen.strName="Calling AutoOpen function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 AutoOpen 함수를 호출";
        vecBehaviors.push_back(autoOpen);
        count++;
    }

    std::regex macro1(R"(Workbook_Open)");
    if(std::regex_search(replaceScript, match, macro1))
    {
        ST_BEHAVIOR Workbook_Open;
        Workbook_Open.strUrl.append(url);
        Workbook_Open.Severity=7;
        Workbook_Open.strName="Calling Workbook_Open function used in a malicious macro";
        Workbook_Open.strDesc="악성 매크로에서 사용되는 Workbook_Open 함수를 호출";
        vecBehaviors.push_back(Workbook_Open);
        count++;
    }

    std::regex macro2(R"(Document_Open)");
    if(std::regex_search(replaceScript, match, macro2))
    {
        ST_BEHAVIOR Document_Open;
        Document_Open.strUrl.append(url);
        Document_Open.Severity=7;
        Document_Open.strName="Calling Document_Open function used in a malicious macro";
        Document_Open.strDesc="악성 매크로에서 사용되는 Document_Open 함수를 호출";
        vecBehaviors.push_back(Document_Open);
        count++;
    }

    std::regex macro3(R"(DocumentOpen)");
    if(std::regex_search(replaceScript, match, macro3))
    {
        ST_BEHAVIOR DocumentOpen;
        DocumentOpen.strUrl.append(url);
        DocumentOpen.Severity=7;
        DocumentOpen.strName="Calling a function used in a malicious macro";
        DocumentOpen.strDesc="악성 매크로에서 사용되는 함수를 호출";
        vecBehaviors.push_back(DocumentOpen);
        count++;
    }

    std::regex macro4(R"(AutoExec)");
    if(std::regex_search(replaceScript, match, macro4))
    {
        ST_BEHAVIOR AutoExec;
        AutoExec.strUrl.append(url);
        AutoExec.Severity=10;
        AutoExec.strName="Calling AutoExec function used in a malicious macro";
        AutoExec.strDesc="악성 매크로에서 사용되는 AutoExec 함수를 호출";
        vecBehaviors.push_back(AutoExec);
        count++;
    }

    std::regex macro5(R"(AutoExit)");
    if(std::regex_search(replaceScript, match, macro5))
    {
        ST_BEHAVIOR AutoExit;
        AutoExit.strUrl.append(url);
        AutoExit.Severity=9;
        AutoExit.strName="Calling AutoExit function used in a malicious macro";
        AutoExit.strDesc="악성 매크로에서 사용되는 AutoExit 함수를 호출";
        vecBehaviors.push_back(AutoExit);
        count++;
    }

    std::regex macro6(R"(Auto_Close)");
    if(std::regex_search(replaceScript, match, macro6))
    {
        ST_BEHAVIOR Auto_Close;
        Auto_Close.strUrl.append(url);
        Auto_Close.Severity=7;
        Auto_Close.strName="Calling Auto_Close function used in a malicious macro";
        Auto_Close.strDesc="악성 매크로에서 사용되는 Auto_Close 함수를 호출";
        vecBehaviors.push_back(Auto_Close);
        count++;
    }

    std::regex macro7(R"(AutoClose)");
    if(std::regex_search(replaceScript, match, macro7))
    {
        ST_BEHAVIOR autoOpen;
        autoOpen.strUrl.append(url);
        autoOpen.Severity=7;
        autoOpen.strName="Calling AutoClose function used in a malicious macro";
        autoOpen.strDesc="악성 매크로에서 사용되는 AutoClose 함수를 호출";
        vecBehaviors.push_back(autoOpen);
        count++;
    }

    std::regex macro8(R"(DocumentChange)");
    if(std::regex_search(replaceScript, match, macro8))
    {
        ST_BEHAVIOR DocumentChange;
        DocumentChange.strUrl.append(url);
        DocumentChange.Severity=9;
        DocumentChange.strName="Calling DocumentChange function used in a malicious macro";
        DocumentChange.strDesc="악성 매크로에서 사용되는 DocumentChange 함수를 호출";
        vecBehaviors.push_back(DocumentChange);
        count++;
    }

    std::regex macro9(R"(AutoNew)");
    if(std::regex_search(replaceScript, match, macro9))
    {
        ST_BEHAVIOR AutoNew;
        AutoNew.strUrl.append(url);
        AutoNew.Severity=10;
        AutoNew.strName="Calling AutoNew function used in a malicious macro";
        AutoNew.strDesc="악성 매크로에서 사용되는 AutoNew 함수를 호출";
        vecBehaviors.push_back(AutoNew);
        count++;
    }

    std::regex macro10(R"(Document_New)");
    if(std::regex_search(replaceScript, match, macro10))
    {
        ST_BEHAVIOR Document_New;
        Document_New.strUrl.append(url);
        Document_New.Severity=9;
        Document_New.strName="Calling a function used in a malicious macro";
        Document_New.strDesc="악성 매크로에서 사용되는 함수를 호출";
        vecBehaviors.push_back(Document_New);
        count++;
    }

    std::regex macro11(R"(NewDocument)");
    if(std::regex_search(replaceScript, match, macro11))
    {
        ST_BEHAVIOR NewDocument;
        NewDocument.strUrl.append(url);
        NewDocument.Severity=8;
        NewDocument.strName="Calling Document_New function used in a malicious macro";
        NewDocument.strDesc="악성 매크로에서 사용되는 Document_New 함수를 호출";
        vecBehaviors.push_back(NewDocument);
        count++;
    }

    std::regex macro12(R"(CreateObject)");
    if(std::regex_search(replaceScript, match, macro12))
    {
        ST_BEHAVIOR CreateObject;
        CreateObject.strUrl.append(url);
        CreateObject.Severity=8;
        CreateObject.strName="Calling CreateObject function used in a malicious macro";
        CreateObject.strDesc="악성 매크로에서 사용되는 CreateObject 함수를 호출";
        vecBehaviors.push_back(CreateObject);
        count++;
    }

    std::regex macro13(R"(allocateMemory)");
    if(std::regex_search(replaceScript, match, macro13))
    {
        ST_BEHAVIOR allocateMemory;
        allocateMemory.strUrl.append(url);
        allocateMemory.Severity=8;
        allocateMemory.strName="Calling allocateMemory function used in a malicious macro";
        allocateMemory.strDesc="악성 매크로에서 사용되는 allocateMemory 함수를 호출";
        vecBehaviors.push_back(allocateMemory);
        count++;
    }

    std::regex macro14(R"(copyMemory)");
    if(std::regex_search(replaceScript, match, macro14))
    {
        ST_BEHAVIOR copyMemory;
        copyMemory.strUrl.append(url);
        copyMemory.Severity=8;
        copyMemory.strName="Calling copyMemory function used in a malicious macro";
        copyMemory.strDesc="악성 매크로에서 사용되는 copyMemory 함수를 호출";
        vecBehaviors.push_back(copyMemory);
        count++;
    }

    std::regex macro15(R"(shellExecute)");
    if(std::regex_search(replaceScript, match, macro15))
    {
        ST_BEHAVIOR shellExecute;
        shellExecute.strUrl.append(url);
        shellExecute.Severity=8;
        shellExecute.strName="Calling shellExecute function used in a malicious macro";
        shellExecute.strDesc="악성 매크로에서 사용되는 shellExecute 함수를 호출";
        vecBehaviors.push_back(shellExecute);
        count++;
    }

    std::regex macro16(R"(Environ)");
    if(std::regex_search(replaceScript, match, macro16))
    {
        ST_BEHAVIOR Environ;
        Environ.strUrl.append(url);
        Environ.Severity=8;
        Environ.strName="Calling Environ function used in a malicious macro";
        Environ.strDesc="악성 매크로에서 사용되는 Environ 함수를 호출";
        vecBehaviors.push_back(Environ);
        count++;
    }

    std::regex macro17(R"(Open)");
    if(std::regex_search(replaceScript, match, macro17))
    {
        ST_BEHAVIOR Open;
        Open.strUrl.append(url);
        Open.Severity=8;
        Open.strName="Calling Open function used in a malicious macro";
        Open.strDesc="악성 매크로에서 사용되는 Open 함수를 호출";
        vecBehaviors.push_back(Open);
        count++;
    }

    std::regex macro18(R"(Write)");
    if(std::regex_search(replaceScript, match, macro18))
    {
        ST_BEHAVIOR Write;
        Write.strUrl.append(url);
        Write.Severity=8;
        Write.strName="Calling Write function used in a malicious macro";
        Write.strDesc="악성 매크로에서 사용되는 Write 함수를 호출";
        vecBehaviors.push_back(Write);
        count++;
    }

    std::regex macro19(R"(Put)");
    if(std::regex_search(replaceScript, match, macro19))
    {
        ST_BEHAVIOR Put;
        Put.strUrl.append(url);
        Put.Severity=8;
        Put.strName="Calling Put function used in a malicious macro";
        Put.strDesc="악성 매크로에서 사용되는 Put 함수를 호출";
        vecBehaviors.push_back(Put);
        count++;
    }
    
    std::regex macro20(R"(Binary)");
    if(std::regex_search(replaceScript, match, macro20))
    {
        ST_BEHAVIOR Binary;
        Binary.strUrl.append(url);
        Binary.Severity=8;
        Binary.strName="Calling Binary function used in a malicious macro";
        Binary.strDesc="악성 매크로에서 사용되는 Binary 함수를 호출";
        vecBehaviors.push_back(Binary);
        count++;
    }
    
    std::regex macro21(R"(Shell)");
    if(std::regex_search(replaceScript, match, macro21))
    {
        ST_BEHAVIOR Shell;
        Shell.strUrl.append(url);
        Shell.Severity=8;
        Shell.strName="Calling Shell function used in a malicious macro";
        Shell.strDesc="악성 매크로에서 사용되는 Shell 함수를 호출";
        vecBehaviors.push_back(Shell);
        count++;
    }
    
    if(count > 0)
        return true;
    return false;
}

bool CScriptAnalyzeEngine::checkXLMMacro(std::string script, std::string urlorNot, std::vector<ST_BEHAVIOR>& vecBehaviors)
{
    // 탐지 됬는지 확인하기 위한 변수
    int count = 0;
    std::smatch match;

    std::string url;
    if(urlorNot == "Not")
        url.append("Local Macro");
    else
        url.append(urlorNot);

    std::regex xlm0(R"(URLDownloadToFileA)");
    // 해당 리스트에 맞게 하나씩 검사하며 결과 전송.
    if(std::regex_search(script, match, xlm0))
    {
        ST_BEHAVIOR URLDownload;
        URLDownload.strUrl.append(url);
        URLDownload.Severity=7;
        URLDownload.strName="Use URLDownloadToFile";
        URLDownload.strDesc="악성 XLM 매크로에서 파일 다운에 사용되는 URLDownloadToFile 키워드를 호출";
        vecBehaviors.push_back(URLDownload);
        count++;
    }

    std::regex xlm1(R"(https?[\w.%/?=:-]*)");
    if(std::regex_search(script, match, xlm1))
    {
        // 다운받는 파일에 대해 추출하기 위해
        int loc = match.str().find_last_of('/');
        int size = match.str().size();
        std::string downFile = match.str().substr(loc+1, size);

        ST_BEHAVIOR URL;
        URL.strUrl.append(url);
        URL.Severity=9;
        URL.strName="Download File From Url";
        URL.strDesc="악성 XLM 매크로에서";
        URL.strDesc.append(downFile);
        URL.strDesc.append("다운에 사용되는 URL을 사용");
        vecBehaviors.push_back(URL);
        count++;
    }
    
    std::regex xlm2(R"(run)");
    if(std::regex_search(script, match, xlm2))
    {
        ST_BEHAVIOR run;
        run.strUrl.append(url);
        run.Severity=6;
        run.strName="Call run";
        run.strDesc="악성 XLM 매크로에서 파일을 실행하기 위해 호출되는 키워드이다.";
        vecBehaviors.push_back(run);
        count++;
    }

    std::regex xlm3(R"(EXEC)");
    if(std::regex_search(script, match, xlm3))
    {
        ST_BEHAVIOR EXEC;
        EXEC.strUrl.append(url);
        EXEC.Severity=6;
        EXEC.strName="Call EXEC";
        EXEC.strDesc="악성 XLM 매크로에서 파일을 실행하기 위해 호출되는 키워드이다.";
        vecBehaviors.push_back(EXEC);
        count++;
    }

    std::regex xlm4(R"(CALL)");
    if(std::regex_search(script, match, xlm4))
    {
        ST_BEHAVIOR CALL;
        CALL.strUrl.append(url);
        CALL.Severity=6;
        CALL.strName="Call CALL";
        CALL.strDesc="악성 XLM 매크로에서 파일을 실행하기 위해 호출되는 키워드이다.";
        vecBehaviors.push_back(CALL);
        count++;
    }
    return true;
}