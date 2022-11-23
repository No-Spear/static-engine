#include "CXMLParsingEngine.h"
#include "CURLExtractionEngine.h"
#include "CDownloadFromUrlEngine.h"
#include "CScriptExtractionEngine.h"
#include "CScriptAnalyzeEngine.h"
#include "CMacroExtractionEngine.h"
#include "CNoSpear.h"

// 전달받은 파일의 위치에서 파일의 hash값을 추출해내는 함수
std::string extractFileHash(const std::string filepath)
{
    int slashlocation = filepath.find_last_of('/');
    int dotlocation = filepath.find_last_of('.');
    return filepath.substr(slashlocation+1, (dotlocation-slashlocation-1));
}

// 엔진에게 결과를 보내는 함수
bool sendStaticEngineResult(const char* pipe, const ST_SERVER_REPORT report)
{
    int filedes = atoi(pipe);
    if(filedes < 0)
    {
        std::cout << "failed to call fifo" << std::endl;
        return false;
    }

    int send = write(filedes, &report,sizeof(report));

    if(send<0)
    {
        std::cout << "failed to write fifo" << std::endl;
        return false;
    }
    close(filedes);
    return true;
}

// 정적엔진 CNoSpear 생성자.
CNoSpear::CNoSpear()
{
    this->macroFlag = false;
    this->xmlFlag = false;
    this->m_Engines.push_back(new CXMLParsingEngine());
}

// 정적엔진 CNoSpear 소멸자
CNoSpear::~CNoSpear()
{
    // 현재 정적엔진의 각종엔진을 제거
    this->m_Engines.clear();
    this->m_Engines.shrink_to_fit();
}

std::string CNoSpear::makeValue(const ST_REPORT& outReport)
{
    std::string values = "(" + std::to_string(outReport.nSeverity);
    values = values + "," + "'" + outReport.strDetectName + "'";
    values = values + "," + "'" + outReport.strHash + "'";
    values = values + "," + "'" + outReport.strName + "'";
    std::string vecValues = ", \"{";
    for(int i = 0; i < outReport.vecBehaviors.size(); i++){
        string tempName = replaceAll(outReport.vecBehaviors[i].strName,"\'",""); 
        tempName = replaceAll(tempName,"\"","");        
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

std::string CNoSpear::replaceAll(std::string str, const std::string from, const std::string to) {

	size_t start_pos = 0; //string처음부터 검사

	while ((start_pos = str.find(from, start_pos)) != std::string::npos)  //from을 찾을 수 없을 때까지
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // 중복검사를 피하고 from.length() > to.length()인 경우를 위해서
	}

	return str;
}

// DB에 저장하기 위한 결과를 만드는 함수
void CNoSpear::makeOutputReport(const ST_FILE_INFO sampleFile ,const ST_ANALYZE_RESULT result, ST_REPORT& outReport)
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
        // 만약 매크로 플래그가 켜있지 않다면 
        if(this->macroFlag == false & this->xmlFlag == false)
            if(totalSeverity < 2.5)
                outReport.nSeverity += 1;
            else if(totalSeverity < 5)
                outReport.nSeverity += 2;
            else if(totalSeverity < 7.5)
                outReport.nSeverity += 3;
            else
                outReport.nSeverity += 4;
        // 매크로 플래그가 켜져있고 악성행위가 있다면 위험도를 행위 판단의 평균으로 변경
        else
            outReport.nSeverity += totalSeverity;
        

        // 만약 전체 결과의 위험도가 5이하 라면 의심파일
        if(outReport.nSeverity <= 5 )
            outReport.strDetectName.append("Suspicious File");
        // 위험도가 5초과 10이하라면 악성파일로 규정
        else if(outReport.nSeverity <= 10)
        {
            if(result.vecBehaviors[0].strName.find("msdt") != std::string::npos)
                outReport.strDetectName.append("Follina");
            else if(result.vecBehaviors[0].strUrl.find("Macro") != std::string::npos)
                outReport.strDetectName.append("Malicious VBA Scirpt Macro");
            else 
                outReport.strDetectName.append("Malware File");
        }

        outReport.strHash.append(sampleFile.strFileHash);
        outReport.strName.append(sampleFile.strFileName);

        outReport.vecBehaviors.reserve(result.vecBehaviors.size() + outReport.vecBehaviors.size());
        outReport.vecBehaviors.insert(outReport.vecBehaviors.end(), result.vecBehaviors.begin(), result.vecBehaviors.end());
    }
    // 행위 결과 값이 없다면
    else
    {
        outReport.strHash.append(sampleFile.strFileHash);
        outReport.strName.append(sampleFile.strFileName);
        // 만약 위험도가 0이라면 정상파일, 0을 넘는다면 의심파일로 설정한다.
        if(outReport.nSeverity == 0)
            outReport.strDetectName.append("Normal File");
        else
            outReport.strDetectName.append("Suspicious File");
    }
}

bool CNoSpear::SaveResult(const ST_REPORT& outReport)
{
    char DBHost[] = "nospear.c9jy6dsf1qz4.ap-northeast-2.rds.amazonaws.com";
    char DBUser[] = "nospear";
    char DBPass[] = "nospear!";
    char DBName[] = "analysisResultDB";
    mysql_init(&connect);
    conn = mysql_real_connect(&connect, DBHost, DBUser , DBPass, DBName, 3306, (char *)NULL, 0);
    if(conn == NULL)
    {
        fprintf(stderr, "Mysql connection error : %s", mysql_error(&connect));
    }

    string values = makeValue(outReport);

    string sql ="INSERT INTO analysisResultTable(nserverity,detectName,sha256,name,behaviors) VALUES" + values;
    if(mysql_query(conn,sql.c_str()) !=0){
        return false;
    }

    mysql_close(conn);
    return true;
}

bool CNoSpear::Analyze(const ST_FILE_INFO sampleFile, ST_REPORT& outReport)
{
    // 입력과 결과관련 파라미터들을 정의
    ST_ANALYZE_PARAM input;
    ST_ANALYZE_RESULT output;

    // 시작시 분석 파일의 위치를 전달.
    input.vecInputFiles.push_back(std::make_pair(sampleFile.strSampleFile, ASF));

    while(!this->m_Engines.empty())
    {
        try{
            // 현재 분석하는 엔진이 어떠한 엔진인지 출력
            std::cout << this->m_Engines[0]->getEngineType() << "Engine 시작"  <<std::endl;
            // 각 엔진의 분석 실행
            // XML엔진의 실패 성공을 위해 인수를 받는다.(예외처리가 없기 때문)
            bool failchecker = this->m_Engines[0]->Analyze(&input, &output);
            // XMLParsing == 수식편집기 취약점을 탐색
            if(this->m_Engines[0]->getEngineType() == "XMLParsing")
            {
                std::cout << std::endl;
                // 만약 수식 편집기 취약점이 없다면 기존의 엔진을 동작
                if(failchecker == false)
                {
                    this->m_Engines.push_back(new CURLExtractEngine());
                    this->m_Engines.push_back(new CDownloadFromUrlEngine());
                    this->m_Engines.push_back(new CScriptExtractionEngine());
                    this->m_Engines.push_back(new CScriptAnalyzeEngine());
                }
                // 그것이 아니면 수식편집기 취약점 플래그를 on.
                else
                {
                    this->xmlFlag = true;
                    std::cout << "Analyze Result: " << std::endl;
                    for(int i =0; i < output.vecBehaviors.size(); i++)
                    {
                        std::cout << "URl: " << output.vecBehaviors[i].strUrl << std::endl;
                        std::cout << "Malicious Name: " <<output.vecBehaviors[i].strName << std::endl;
                        std::cout << "Descrition: " <<output.vecBehaviors[i].strDesc << std::endl;
                        std::cout << "Severity: " << output.vecBehaviors[i].Severity << "\n" << std::endl;
                    }
                }
            }
            // URLExtraction엔진의 경우 
            else if(this->m_Engines[0]->getEngineType() == "URLExtraction")
            {
                // 분석했던 파일을 제거
                input.vecInputFiles.pop_back();
                // URL 추출엔진에서 추출된 결과를 다음엔진의 값으로 넣을 수 있게 작업
                input.vecURLs.reserve(output.vecExtractedUrls.size() + input.vecURLs.size());
                input.vecURLs.insert(input.vecURLs.end(), output.vecExtractedUrls.begin(), output.vecExtractedUrls.end());

                // url이 있으면 위험도를 3증가.
                outReport.nSeverity += 3;

                std::cout << "Extracted URLs:" << std::endl;
                for(int i = 0; i< output.vecExtractedUrls.size(); i++)
                    std::cout << output.vecExtractedUrls[i] << std::endl;
                std::cout << std::endl;
            }
            else if(this->m_Engines[0]->getEngineType() == "DownloadFromUrl")
            {
                // 추출엔진의 결과를 입력으로 제공
                input.vecInputFiles.reserve(output.vecExtractedFiles.size() + input.vecInputFiles.size());
                input.vecInputFiles.insert(input.vecInputFiles.end(), output.vecExtractedFiles.begin(), output.vecExtractedFiles.end());

                // Download된 파일의 수만큼 위험도를 증가
                int downloadSize = output.vecExtractedFiles.size();
                // 단 Download된 파일에서 제공할 수있는 최대 위험도는 2로 설정.
                if(downloadSize > 2)
                    outReport.nSeverity += 2;
                else
                    outReport.nSeverity += downloadSize;
        
                // 다운로드된 파일 위치 정보 출력
                for(int i = 0; i < output.vecExtractedFiles.size(); i++)
                {
                    std::cout << "Downloaded File Location:" << std::endl;
                    std::cout << output.vecExtractedFiles[i].first << std::endl;
                    std::cout << "CnC URL & Downloaded File Status: " << output.vecExtractedFiles[i].second << std::endl;
                }
                std::cout << std::endl;
            }
            else if(this->m_Engines[0]->getEngineType() == "ScriptExtraction")
            {
                input.vecScriptFIles.reserve(output.vecExtractedScript.size() + input.vecScriptFIles.size());
                input.vecScriptFIles.insert(input.vecScriptFIles.end(), output.vecExtractedScript.begin(), output.vecExtractedScript.end());
        
                for(int i = 0; i< output.vecExtractedScript.size(); i++)
                {
                    std::cout << "Extracted Script:(If VBS Maximum ouput is 30 bytes)" << std::endl;
                    if(output.vecExtractedScript[i].second.second == VBS)
                    {
                        for(int j = 0; j < 30; j++)
                            std::cout << output.vecExtractedScript[i].first[j];
                    }
                    else
                        std::cout << output.vecExtractedScript[i].first << std::endl;
                    std::cout << "\nScript Type is " << output.vecExtractedScript[i].second.second << std::endl;
                }
                std::cout << std::endl;
            }
            else if(this->m_Engines[0]->getEngineType() == "ScriptAnalyze")
            {                
                std::cout << "Analyze Result:" << std::endl;
                for(int i= 0; i< output.vecBehaviors.size(); i++)
                {
                    std::cout << "URl: " << output.vecBehaviors[i].strUrl << std::endl;
                    std::cout << "Malicious Name: " <<output.vecBehaviors[i].strName << std::endl;
                    std::cout << "Descrition: " <<output.vecBehaviors[i].strDesc << std::endl;
                    std::cout << "Severity: " << output.vecBehaviors[i].Severity << "\n" << std::endl;
                }
                
            }
            else if(this->m_Engines[0]->getEngineType() == "MacroExtraction")
            {
                input.vecScriptFIles.reserve(output.vecExtractedScript.size() + input.vecScriptFIles.size());
                input.vecScriptFIles.insert(input.vecScriptFIles.end(), output.vecExtractedScript.begin(), output.vecExtractedScript.end());

                // VBA Sciprt가 있다면 MacroFlag를 설정한다.
                this->macroFlag = true;

                for(int i =0; i<output.vecExtractedScript.size(); i++)
                {
                    std::cout << "Extracted Macro(Maximun print = 30):" << std::endl;
                    for(int j = 0; j < 30; j++)
                        std::cout << output.vecExtractedScript[i].first[j];
                    std::cout << "\nScript Type is " << output.vecExtractedScript[i].second.second << std::endl;
                }
                std::cout << std::endl;
            }
            else
                throw engine_Exception("Static-Engine","s",this->m_Engines[0]->getEngineType(), "Engine은 현재 지원하지 않는 엔진입니다.");

        } catch(const std::exception& e)
        {
            std::cout << "\n" << e.what() << "\n" << std::endl;
            if(strstr(e.what(),"Url이 없습니다."))
            {
                // 현재 배열에 등록된 엔진 객체 소멸
                this->m_Engines.clear();
                // vector가 가진 메모리 회수
                this->m_Engines.shrink_to_fit();
                // 매크로 추출엔진 삽입.
                this->m_Engines.push_back(new CMacroExtractionEngine());
                // 스크립트 분석엔진 삽입.
                this->m_Engines.push_back(new CScriptAnalyzeEngine());
                // this->m_Engines.push_back(new COLE);
                // this->m_Engines.push_back(new CDDE);
                continue;
            }
            makeOutputReport(sampleFile, output, outReport);
            return false;
        }
        this->m_Engines.erase(this->m_Engines.begin());
    }
    // DB에 저장할 결과를 제작
    makeOutputReport(sampleFile, output, outReport);

    return true;
}

int main(int argc, char** argv)
{
    if(argc != 4)
    {
        std::cout << "syntax : CStatic-Engine <FileName> <FileLocation> <UnnamedPipe>" << std::endl;
	    std::cout << "sample:  CStatic-Engine test.docx ./temp/testdocx 3" << std::endl;
        return -1;
    }

    // 분석하기 위한 파일에 대한 정보를 설정
    ST_FILE_INFO sampleFile;
    sampleFile.strFileName = std::string(argv[1]);
    sampleFile.strSampleFile = std::string(argv[2]);
    sampleFile.strFileHash = extractFileHash(sampleFile.strSampleFile);

    CNoSpear* staticEngine = new CNoSpear();
    ST_REPORT outReport;
    // initialize nServerity
    outReport.nSeverity = 0;

    // 정적엔진 분석 시작
    if(!staticEngine->Analyze(sampleFile, outReport))
        std::cout << "예외 루틴이 발생하였으나 처리 완료\n" << std::endl;

    // DB로 분석 결과를 전달
    staticEngine->SaveResult(outReport);

    // 서버로 분석 결과를 전달
    ST_SERVER_REPORT report;
    strcpy(report.strName, outReport.strName.c_str());
    strcpy(report.strHash, outReport.strHash.c_str());
    strcpy(report.strDectName, outReport.strDetectName.c_str());
    report.nSeverity = outReport.nSeverity;

    std::cout << "서버로 보낼 정보" << std::endl;
    std::cout << report.strName << std::endl;
    std::cout << report.strHash << std::endl;
    std::cout << report.strDectName << std::endl;
    std::cout << report.nSeverity << std::endl;
    std::cout << std::endl;
   
   if(!sendStaticEngineResult(argv[3], report))
   {
        std::cout << "Cant Send Data to Server" << std::endl;
        return -1;
   }

    delete staticEngine;

    return 0;
}