#include "CURLExtractionEngine.h"
#include "CDownloadFromUrlEngine.h"
#include "CScriptExtractionEngine.h"
#include "CScriptAnalyzeEngine.h"
#include "CNoSpear.h"

// 전달받은 파일의 위치에서 파일의 hash값을 추출해내는 함수
std::string extractFileHash(const std::string filepath)
{
    int slashlocation = filepath.find_last_of('/');
    int dotlocation = filepath.find_last_of('.');
    return filepath.substr(slashlocation+1, (dotlocation-slashlocation-1));
}

// DB에 저장하기 위한 결과를 만드는 함수
void makeOutputReport(const ST_FILE_INFO sampleFile ,const ST_ANALYZE_RESULT result, ST_REPORT& outReport)
{
    // 서버로 보낼 평균 위험도
    int totalSeverity = 0;
    // 분석 결과의 사이즈
    int behaviorSize = result.vecBehaviors.size();
    
    // NSeverity 결정을 위해
    for(int i =0; i< behaviorSize; i++)
        totalSeverity += result.vecBehaviors[i].Severity;

    if(behaviorSize != 0)
    {
        outReport.strHash.append(sampleFile.strFileHash);
        outReport.strName.append(sampleFile.strFileName);
        if(result.vecBehaviors[0].strName.compare("Call msdt Function") == 0)
            outReport.strDetectName.append("Follina");
        outReport.nSeverity = totalSeverity / behaviorSize;
        outReport.vecBehaviors.reserve(result.vecBehaviors.size() + outReport.vecBehaviors.size());
        outReport.vecBehaviors.insert(outReport.vecBehaviors.end(), result.vecBehaviors.begin(), result.vecBehaviors.end());
    }
    else
    {
        outReport.strHash.append(sampleFile.strFileHash);
        outReport.strName.append(sampleFile.strFileName);
        // 만약 추출된 Url은 있지만 분석 결과가 없다면
        if(result.vecExtractedUrls.size() != 0)
            outReport.strDetectName.append("SuspiciousFile");
        else
            outReport.strDetectName.append("NomalFile");
        outReport.nSeverity = 0;
    }    
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
    this->m_Engines.push_back(new CURLExtractEngine());
    this->m_Engines.push_back(new CDownloadFromUrlEngine());
    this->m_Engines.push_back(new CScriptExtractionEngine());
    this->m_Engines.push_back(new CScriptAnalyzeEngine());
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
        if(i == 0){
            vecValues = vecValues + "'Behavior_" + std::to_string(i) + "' :" + "{";    
        }else{
        vecValues = vecValues + ", 'Behavior_" + std::to_string(i) + "' :" + "{";    
        }
        vecValues = vecValues + "'serverity' : " + "'" +std::to_string(outReport.vecBehaviors[i].Severity)+ "'";
        vecValues = vecValues + ", 'Desc' : " + "'" + outReport.vecBehaviors[i].strDesc + "'";
        vecValues = vecValues + ", 'Name' : " + "'" + outReport.vecBehaviors[i].strName + "'";
        vecValues = vecValues + "}";
     
    }
    vecValues = vecValues + "}\"";

    values = values + vecValues + ")";

    return values;
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

    string sql ="INSERT INTO analysisResultTable(nseverity,detectName,sha256,name,behaviors) VALUES" + values;

    std::cout << sql << std::endl;
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

    std::cout << "URL 추출엔진 시작" << std::endl;
    // URL 추출엔진 시작
    if(!this->m_Engines[0]->Analyze(&input, &output))
        return false;
    // 분석했던 파일을 제거
    input.vecInputFiles.pop_back();
    // URL 추출엔진에서 추출된 결과를 다음엔진의 값으로 넣을 수 있게 작업
    input.vecURLs.reserve(output.vecExtractedUrls.size() + input.vecURLs.size());
    input.vecURLs.insert(input.vecURLs.end(), output.vecExtractedUrls.begin(), output.vecExtractedUrls.end());
    
    std::cout << "URL 다운로드 엔진 시작" << std::endl;
    // URL을 통한 다운로드 엔진 시작
    if(!this->m_Engines[1]->Analyze(&input, &output))
        return false;
    // 추출엔진의 결과를 입력으로 제공
    input.vecInputFiles.reserve(output.vecExtractedFiles.size() + input.vecInputFiles.size());
    input.vecInputFiles.insert(input.vecInputFiles.end(), output.vecExtractedFiles.begin(), output.vecExtractedFiles.end());
    
    std::cout << "스크립트 추출엔진 시작" << std::endl;
    // 다운받은 파일에서 스크립트 추출엔진 시작
    if(!this->m_Engines[2]->Analyze(&input, &output))
        return false;
    // 추출엔진의 결과를 입력으로 제공
    input.vecScriptFIles.reserve(output.vecExtractedScript.size() + input.vecScriptFIles.size());
    input.vecScriptFIles.insert(input.vecScriptFIles.end(), output.vecExtractedScript.begin(), output.vecExtractedScript.end());
    
    std::cout << "스크립트 분석엔진 시작" << std::endl;
    // 스크립트 분석엔진 시작
    if(!this->m_Engines[3]->Analyze(&input, &output))
        return false;

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
    std::cout << sampleFile.strFileHash << std::endl;

    CNoSpear* staticEngine = new CNoSpear();
    ST_REPORT outReport;

    // 정적엔진 분석 시작
    if(!staticEngine->Analyze(sampleFile, outReport))
        return -1;

    std::cout << "악성 행위 정보" << std::endl;
    for(int i= 0; i< outReport.vecBehaviors.size(); i++)
    {
        std::cout << outReport.vecBehaviors[i].strName << std::endl;
        std::cout << outReport.vecBehaviors[i].strDesc << std::endl;
        std::cout << outReport.vecBehaviors[i].Severity << std::endl;
    }

    // DB로 분석 결과를 전달
    staticEngine->SaveResult(outReport);

    // 서버로 분석 결과를 전달
    ST_SERVER_REPORT report;
    strcpy(report.strHash, outReport.strHash.c_str());
    strcpy(report.strDectName, outReport.strDetectName.c_str());
    report.nSeverity = outReport.nSeverity;

    std::cout << "서버로 보낼 정보" << std::endl;
    std::cout << report.strHash << std::endl;
    std::cout << report.strDectName << std::endl;
    std::cout << report.nSeverity << std::endl;
   
   if(!sendStaticEngineResult(argv[3], report))
   {
        std::cout << "Cant Send Data to Server" << std::endl;
        return -1;
   }

    delete staticEngine;

    return 0;

}