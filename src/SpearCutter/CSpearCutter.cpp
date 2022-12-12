#include "SampleFileInfo.h"
#include "CSpearCutter.h"

// SpearCutter엔진 생성자.
CSpearCutter::CSpearCutter()
{

}

// 정적엔진 CNoSpear 소멸자
CSpearCutter::~CSpearCutter()
{
    // 현재 정적엔진의 각종엔진을 제거
    this->m_Engines.clear();
    this->m_Engines.shrink_to_fit();
}

void CSpearCutter::InitializeFlags()
{
    this->maliciousInternalCVEFound = false;
    this->maliciousExternalCVEFound = false;
    this->maliciousMacroFound = false;
}

std::string CSpearCutter::MakeValue(const ST_REPORT& outReport)
{
    std::string values = "(" + std::to_string(outReport.nSeverity);
    values = values + "," + "'" + outReport.strDetectName + "'";
    values = values + "," + "'" + outReport.strHash + "'";
    values = values + "," + "'" + outReport.strName + "'";
    std::string vecValues = ", \"{";
    for(int i = 0; i < outReport.vecBehaviors.size(); i++){
        std::string tempName = ReplaceAll(outReport.vecBehaviors[i].strName,"\'",""); 
        tempName = ReplaceAll(tempName,"\"","");        
        if(i == 0){
            vecValues = vecValues + "'Behavior_" + std::to_string(i) + "' :" + "{";    
        }else{
        vecValues = vecValues + ", 'Behavior_" + std::to_string(i) + "' :" + "{";    
        }
        vecValues = vecValues + "'serverity' : " + "'" +std::to_string(outReport.vecBehaviors[i].Severity)+ "'";
        vecValues = vecValues + ", 'Desc' : " + "'" + outReport.vecBehaviors[i].strDesc + "'";
        vecValues = vecValues + ", 'Name' : " + "'" + tempName + "'";
        vecValues = vecValues + "}";
    }
    vecValues = vecValues + "}\"";

    values = values + vecValues + ")";

    return values;
}

std::string CSpearCutter::ReplaceAll(std::string str, const std::string from, const std::string to) {

	size_t start_pos = 0; //string처음부터 검사

	while ((start_pos = str.find(from, start_pos)) != std::string::npos)  //from을 찾을 수 없을 때까지
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // 중복검사를 피하고 from.length() > to.length()인 경우를 위해서
	}

	return str;
}

// DB에 저장하기 위한 결과를 만드는 함수
void CSpearCutter::MakeOutputReportForDB(const ST_ANALYZE_RESULT result, ST_REPORT& outReport, bool fileError)
{
    // 서버로 보낼 평균 위험도
    float totalSeverity = 0;
    // 분석 결과의 사이즈
    int behaviorSize = result.vecBehaviors.size();
    
    // NSeverity 결정을 위해
    for(int i =0; i< behaviorSize; i++)
        totalSeverity += result.vecBehaviors[i].Severity;

    // 악성행위의 정보가 있다면
    if(behaviorSize != 0)
    {
        // 위험도의 평균을 구하고 이를 전체 위험도에 더함.
        totalSeverity = totalSeverity / behaviorSize;

        // 문서 내부의 취약점과 매크로 취약점이 없으면서 
        if((this->maliciousInternalCVEFound == false && this->maliciousMacroFound == false) && this->maliciousExternalCVEFound == true)
        {
            if(totalSeverity < 2.5)
                outReport.nSeverity += 1;
            else if(totalSeverity < 5)
                outReport.nSeverity += 2;
            else if(totalSeverity < 7.5)
                outReport.nSeverity += 3;
            else
                outReport.nSeverity += 4;
        }
        else if(this->maliciousInternalCVEFound == true)
        {
            // 문서 내부의 취약점이 있다면 무조껀 10의 score를 준다.
            outReport.nSeverity = 10;
        }
        else
            outReport.nSeverity += totalSeverity;

        if(!outReport.strName.empty() && !outReport.strName.empty())
        {
            outReport.vecBehaviors.reserve(result.vecBehaviors.size() + outReport.vecBehaviors.size());
            outReport.vecBehaviors.insert(outReport.vecBehaviors.end(), result.vecBehaviors.begin(), result.vecBehaviors.end());
            return;
        }
        // 만약 전체 결과의 위험도가 5이하 라면 의심파일
        if(outReport.nSeverity == 0)
            outReport.strDetectName = "Normal File";
        else if(outReport.nSeverity <= 5 )
            outReport.strDetectName = "Suspicious File";
        // 위험도가 5초과 10이하라면 악성파일로 규정
        else if(outReport.nSeverity <= 10)
        {
            for(int i = 0; i < result.vecBehaviors.size(); i++)
            {
                if(result.vecBehaviors[i].strName.find("msdt") != std::string::npos)
                {
                    outReport.strDetectName = "Follina";
                    break;
                }
                else if(result.vecBehaviors[i].strUrl.find("Macro") != std::string::npos)
                {
                    outReport.strDetectName = "Malicious Macro(VBA or XLM)";
                    break;
                }
                else if (result.vecBehaviors[i].strName.find("CVE-2017-11882") != std::string::npos)
                {
                    outReport.strDetectName = "CVE-2017-11882";
                    break;
                }
                else
                    continue;
            }
        }
        if(fileError == false)
        {
            outReport.strHash.append(Sample()->m_strFileHash);
            outReport.strName.append(Sample()->m_strOriginFileName);
        }
        else
        {
            outReport.strHash.append(ExtractFileNameWithoutExt(result.vecExtractedFiles[0].first));
            outReport.strName.append(result.vecExtractedFiles[1].first);
        }        

        outReport.vecBehaviors.reserve(result.vecBehaviors.size() + outReport.vecBehaviors.size());
        outReport.vecBehaviors.insert(outReport.vecBehaviors.end(), result.vecBehaviors.begin(), result.vecBehaviors.end());
    }
    // 행위 결과 값이 없다면
    else
    {
        if(!outReport.strName.empty() && !outReport.strName.empty())
            return;

        outReport.strHash.append(Sample()->m_strFileHash);
        outReport.strName.append(Sample()->m_strOriginFileName);
        // 만약 위험도가 0이라면 정상파일, 0을 넘는다면 의심파일로 설정한다.
        if(outReport.nSeverity == 0)
            outReport.strDetectName.append("Normal File");
        else
            outReport.strDetectName.append("Suspicious File");
    }
}

bool CSpearCutter::SaveResultToDB(const ST_REPORT& outReport)
{
    char DBHost[] = "nospear.c9jy6dsf1qz4.ap-northeast-2.rds.amazonaws.com";
    char DBUser[] = "nospear";
    char DBPass[] = "nospear!";
    char DBName[] = "analysisResultDB";
    // char DBHost[] = "localhost";
    // char DBUser[] = "root";
    // char DBPass[] = "DBPW1234";
    // char DBName[] = "VSERVER";
    mysql_init(&connect);
    conn = mysql_real_connect(&connect, DBHost, DBUser , DBPass, DBName, 3306, (char *)NULL, 0);
    if(conn == NULL)
    {
        fprintf(stderr, "Mysql connection error : %s", mysql_error(&connect));
    }

    std::string values = MakeValue(outReport);

    std::string sql ="INSERT INTO analysisResultTable(nserverity,detectName,sha256,name,behaviors) VALUES" + values;
    if(mysql_query(conn,sql.c_str()) !=0){
        return false;
    }

    mysql_close(conn);
    return true;
}

bool CSpearCutter::AddEngine(CEngineSuper* engine)
{
    this->m_Engines.push_back(engine);
    return true;
}

ECODE CSpearCutter::Analyze(const std::tstring strFileName, const std::tstring strSampleFile, ST_REPORT& outReport, std::vector<ST_ANALYZE_FILES>& sampleFiles)
{
    // 입력과 결과관련 파라미터들을 정의
    ST_ANALYZE_PARAM input;
    ST_ANALYZE_RESULT output;

    ECODE nRet = EC_SYSTEM_ERROR;

    // 샘플 파일에 대해 각종 정보 초기화 작업 및 문서 형식에 맞는 파서를 초기화 한다.
    try{
        // 샘플 파일에 대해 각종 정보 초기화 작업
        nRet = Sample()->Init(strFileName, strSampleFile);

        // 만약 초기화에 실패했다면 예외처리 진행
        if(EC_SUCCESS != nRet)
            throw engine_Exception("SpearCutter","ss", strFileName.c_str() ," File Initialize Fail");

    }catch(const std::exception& e)
    {
        Log_Error("%s", e.what());
        this->m_Engines.clear();
        this->m_Engines.shrink_to_fit();
        output.vecExtractedFiles.push_back(std::make_pair(strSampleFile,ASF));
        output.vecExtractedFiles.push_back(std::make_pair(strFileName, ASF));
        MakeOutputReportForDB(output, outReport, true);
        output.vecExtractedFiles.pop_back();
        output.vecExtractedFiles.pop_back();
        return nRet;
    }
    // 초기화가 정상적으로 진행됬다면 시작시 분석 파일의 위치를 전달.
    input.vecInputFiles.push_back(strSampleFile);

    while(!this->m_Engines.empty())
    {
        try{
            // 현재 분석된 행위의 마지막 위치를 가르키기 위한 변수
            int behaviorLocation = output.vecBehaviors.size();
            // 다운로드된 파일이 몇개인지 확인하기 위한 변수
            int downloadFileCount = 0;

            // 현재 분석하는 엔진이 어떠한 엔진인지 출력
            std::cout << this->m_Engines[0]->getEngineType() << TCSFromMBS("Engine 시작")  <<std::endl;
            // 각 엔진의 분석 실행 및 결과 확인
            bool resultFound = this->m_Engines[0]->Analyze(&input, &output);
            
            switch (this->m_Engines[0]->GetPriority())
            {
            // CheckInternalCVEEngine
            case 0:
                if(output.vecExtractedFiles.size() != 0)
                {
                    for(int i =0; i < output.vecExtractedFiles.size(); i++)
                    {
                        ST_ANALYZE_FILES extractedSample;
                        extractedSample.strFileName = TCSFromMBS(ExtractFileNameWithoutExt(output.vecExtractedFiles[i].first));
                        extractedSample.strSampleFilePath = TCSFromMBS(output.vecExtractedFiles[i].first);
                        sampleFiles.push_back(extractedSample);
                    }
                }
                if(resultFound == true)
                {
                    this->maliciousInternalCVEFound = true;
                    this->m_Engines.clear();
                    this->m_Engines.shrink_to_fit();
                    std::cout << "CheckInternalCVE Result: " << std::endl;
                    for(int i = 0; i < output.vecBehaviors.size(); i++)
                    {
                        std::cout << "URl: " << output.vecBehaviors[i].strUrl << std::endl;
                        std::cout << "Malicious Name: " <<output.vecBehaviors[i].strName << std::endl;
                        std::cout << "Descrition: " <<output.vecBehaviors[i].strDesc << std::endl;
                        std::cout << "Severity: " << output.vecBehaviors[i].Severity << "\n" << std::endl;                    }
                    continue;
                }
                else
                    std::cout << "분석결과 정상입니다.\n" <<std::endl;
                break;
            
            /*
             * 1~5는 CheckExternalCVEEngine
            */ 
            // URLExtraction
            case 1:
                if(resultFound == true)
                {
                    this->maliciousExternalCVEFound = true;
                    // 매크로 추출엔진 삭제
                    this->m_Engines.pop_back();
                }
                else
                {
                    std::cout << "추출된 URL이 없습니다.\n" << std::endl;
                    this->m_Engines.clear();
                    this->m_Engines.shrink_to_fit();
                    this->m_Engines.push_back(new CMacroExtractionEngine());
                    this->m_Engines.push_back(new CScriptAnalyzeEngine());
                    continue;
                }
                // URL 추출엔진에서 추출된 결과를 다음엔진의 값으로 넣을 수 있게 작업
                input.vecURLs.reserve(output.vecExtractedUrls.size() + input.vecURLs.size());
                input.vecURLs.insert(input.vecURLs.end(), output.vecExtractedUrls.begin(), output.vecExtractedUrls.end());

                // url이 있으면 위험도를 3증가.
                outReport.nSeverity += 3;
                this->maliciousExternalCVEFound = true;

                if((behaviorLocation - output.vecBehaviors.size()) != 0)
                {
                    for(int i = behaviorLocation; i < output.vecBehaviors.size(); i++)
                    {
                        std::cout << "Extracted Result: " << std::endl;
                        std::cout << "URl: " << output.vecBehaviors[i].strUrl << std::endl;
                        std::cout << "Malicious Name: " <<output.vecBehaviors[i].strName << std::endl;
                        std::cout << "Descrition: " <<output.vecBehaviors[i].strDesc << std::endl;
                        std::cout << "Severity: " << output.vecBehaviors[i].Severity << "\n" << std::endl;
                    }
                }
                break;
            
            // DownloadFromURL
            case 2:
                // 추출엔진의 결과를 입력으로 제공
                input.vecExtractedFiles.reserve(output.vecExtractedFiles.size() + input.vecExtractedFiles.size());
                input.vecExtractedFiles.insert(input.vecExtractedFiles.end(), output.vecExtractedFiles.begin(), output.vecExtractedFiles.end());

                // Download된 파일의 수만큼 위험도를 증가
                downloadFileCount = output.vecExtractedFiles.size();
                // 단 Download된 파일에서 제공할 수있는 최대 위험도는 2로 설정.
                if(downloadFileCount > 2)
                    outReport.nSeverity += 2;
                else
                    outReport.nSeverity += downloadFileCount;
        
                // 다운로드된 파일 위치 정보 출력
                for(int i = 0; i < output.vecExtractedFiles.size(); i++)
                {
                    std::cout << "Downloaded File Location:" << std::endl;
                    std::cout << output.vecExtractedFiles[i].first << std::endl;
                    std::cout << "CnC URL & Downloaded File Status: " << output.vecExtractedFiles[i].second << std::endl;
                }
                std::cout << std::endl;
                break;
            
            // ScriptExtraction
            case 3:
                if(resultFound == false)
                {
                    std::cout << "현재 엔진으로부터 추출된 결과가 없습니다.\n" << std::endl;
                    this->m_Engines.clear();
                    this->m_Engines.shrink_to_fit();
                    for(int i  = 0; i < output.vecExtractedFiles.size(); i++)
                    {
                        std::string fileExt = ExtractFileExt(output.vecExtractedFiles[i].first);
                        if(fileExt == "doc" || fileExt == "xls" || fileExt == "ppt")
                        {
                            ST_ANALYZE_FILES extractedSample;
                            extractedSample.strFileName = TCSFromMBS(ExtractFileNameWithoutExt(output.vecExtractedFiles[i].first));
                            extractedSample.strSampleFilePath = TCSFromMBS(output.vecExtractedFiles[i].first);
                            sampleFiles.push_back(extractedSample);
                        }
                    }
                    continue;
                }
                input.vecScriptFIles.reserve(output.vecExtractedScript.size() + input.vecScriptFIles.size());
                input.vecScriptFIles.insert(input.vecScriptFIles.end(), output.vecExtractedScript.begin(), output.vecExtractedScript.end());
        
                for(int i = 0; i< output.vecExtractedScript.size(); i++)
                {
                    std::cout << "Extracted Script:(If VBS Maximum ouput is 30 bytes)" << std::endl;
                    if(output.vecExtractedScript[i].second.second == VBS)
                    {
                        int loop = output.vecExtractedScript[i].first.size();
                        if (loop > 30)
                            loop = 30;
                        for(int j = 0; j < loop; j++)
                            std::cout << output.vecExtractedScript[i].first[j];
                        std::cout << "\n";
                    }
                    else
                        std::cout << output.vecExtractedScript[i].first << std::endl;
                    std::cout << "Script Type is " << output.vecExtractedScript[i].second.second << std::endl;
                }
                std::cout << std::endl;
                break;
            
            // ScriptAnalyze
            case 4:
                std::cout << "Analyze Result:" << std::endl;
                if((behaviorLocation - output.vecBehaviors.size()) == 0)
                {
                    std::cout << "현재 엔진에서 탐지된 결과가 없습니다.\n" << std::endl;
                    // 매크로 플래그를 꺼준다.
                    this->maliciousMacroFound = false;
                }
                else
                    for(int i= behaviorLocation; i< output.vecBehaviors.size(); i++)
                    {
                        std::cout << "URl: " << output.vecBehaviors[i].strUrl << std::endl;
                        std::cout << "Malicious Name: " <<output.vecBehaviors[i].strName << std::endl;
                        std::cout << "Descrition: " <<output.vecBehaviors[i].strDesc << std::endl;
                        std::cout << "Severity: " << output.vecBehaviors[i].Severity << "\n" << std::endl;
                    }
                break;
            
            // MacroExtraction
            case 5:
                input.vecScriptFIles.reserve(output.vecExtractedScript.size() + input.vecScriptFIles.size());
                input.vecScriptFIles.insert(input.vecScriptFIles.end(), output.vecExtractedScript.begin(), output.vecExtractedScript.end());

                // 무조껀 매크로 플래그를 킨다.
                this->maliciousMacroFound = true;

                for(int i =0; i<output.vecExtractedScript.size(); i++)
                {
                    std::cout << "Extracted Macro(Maximun print = 30):" << std::endl;
                    int loop = output.vecExtractedScript[i].first.size();
                        if (loop > 30)
                            loop = 30;
                    for(int j = 0; j < loop; j++)
                        std::cout << output.vecExtractedScript[i].first[j];
                    std::cout << "\nScript Type is " << output.vecExtractedScript[i].second.second << std::endl;
                }
                std::cout << std::endl;
                break;
            
            default:
                throw engine_Exception("Static-Engine","s",this->m_Engines[0]->getEngineType(), "Engine은 현재 지원하지 않는 엔진입니다.");
                break;
            }
        } catch(const std::exception& e)
        {
            std::cout << "\n" << e.what() << "\n" << std::endl;
            this->m_Engines.clear();
            MakeOutputReportForDB(output, outReport, false);
            return EC_SYSTEM_ERROR;
        }
        this->m_Engines.erase(this->m_Engines.begin());
    }
    // DB에 저장할 결과를 제작
    MakeOutputReportForDB(output, outReport, false);

    return EC_SUCCESS;
}