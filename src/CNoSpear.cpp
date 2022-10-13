#include "CURLExtractionEngine.h"
#include "CDownloadFromUrlEngine.h"
#include "CScriptExtractionEngine.h"
#include "CScriptAnalyzeEngine.h"
#include "CNoSpear.h"

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

std::string CNoSpear::makeValue(ST_REPORT& outReport)
{
    std::string values = "(" + std::to_string(outReport.nSeverity);
    values = values + "," + "'" + outReport.strDetectName + "'";
    values = values + "," + "'" + outReport.strHash + "'";
    values = values + "," + "'" + outReport.strName + "'";
    std::string vecValues = ", '{";
    for(int i = 0; i < outReport.vecBehaviors.size(); i++){
        if(i == 0){
            vecValues = vecValues + " Behavior_" + std::to_string(i) + " :" + "{";    
        }else{
        vecValues = vecValues + ", Behavior_" + std::to_string(i) + " :" + "{";    
        }
        vecValues = vecValues + "serverity : " + std::to_string(outReport.vecBehaviors[i].Severity);
        vecValues = vecValues + ", Desc : " + outReport.vecBehaviors[i].strDesc;
        vecValues = vecValues + ", Name : " + outReport.vecBehaviors[i].strName;
        vecValues = vecValues + "}";
     
    }
    vecValues = vecValues + "}'";

    values = values + vecValues + ")";

    return values;
}

bool CNoSpear::SaveResult(ST_REPORT& outReport)
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

// 전달받은 파일의 위치에서 파일의 hash값을 추출해내는 함수
std::string CNoSpear::extractFileHash(const std::string filepath)
{
    int slashlocation = filepath.find_last_of('/');
    int dotlocation = filepath.find_last_of('.');
    return filepath.substr(slashlocation+1, (dotlocation-slashlocation-1));
}

bool CNoSpear::Analyze(std::string strSampleFile, ST_REPORT& outReport)
{
    // 입력과 결과관련 파라미터들을 정의
    ST_ANALYZE_PARAM input;
    ST_ANALYZE_RESULT output;
    // 시작시 분석 파일의 위치를 전달.
    input.vecInputFiles.push_back(strSampleFile);

    for(int i =0; i < this->m_Engines.size(); i++)
        this->m_Engines[i]->Analyze(&input, &output);

    outReport.strHash.append(extractFileHash(strSampleFile));

    // NSeverity 결정
    int temp = 0;
    for(int i =0; i< output.vecBehaviors.size(); i++)
    {
        temp += output.vecBehaviors[i].Severity;
    }
    if( output.vecBehaviors.size() != 0)
        outReport.nSeverity= temp / output.vecBehaviors.size();
    else
        outReport.nSeverity = 0;

    outReport.strDetectName.append("Follina");
    
    // 결과를 전달하기 위한 코드
    outReport.vecBehaviors.reserve(output.vecBehaviors.size() + outReport.vecBehaviors.size());
    outReport.vecBehaviors.insert(outReport.vecBehaviors.end(), output.vecBehaviors.begin(), output.vecBehaviors.end());
    return true;
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

int main(int argc, char** argv)
{
    CNoSpear* staticEngine = new CNoSpear();
    std::string inputfile(argv[1]);
    ST_REPORT outReport;
    if(!staticEngine->Analyze(inputfile, outReport))
        return -1;
        

    for(int i= 0; i< outReport.vecBehaviors.size(); i++)
    {
        std::cout << outReport.vecBehaviors[i].strName << std::endl;
        std::cout << outReport.vecBehaviors[i].strDesc << std::endl;
        std::cout << outReport.vecBehaviors[i].Severity << std::endl;
    }

    ST_SERVER_REPORT report;
    strcpy(report.strHash, outReport.strHash.c_str());
    int temp = 0;
    for(int i =0; i< outReport.vecBehaviors.size(); i++)
    {
        temp += outReport.vecBehaviors[i].Severity;
    }
    
    if(outReport.vecBehaviors.size() != 0)
    {
        report.nSeverity = temp / outReport.vecBehaviors.size();
        strcpy(report.strDectName, "Follina");

        std::cout << "서버로 보낼 정보" << std::endl;
        std::cout << report.strHash << std::endl;
        std::cout << report.strDectName << std::endl;
        std::cout << report.nSeverity << std::endl;
        std::cout << std::endl;
        if(!sendStaticEngineResult(argv[2], report))
            return -1;
    }
    else{
        report.nSeverity = 0;
        strcpy(report.strDectName, "Normal");

        std::cout << "서버로 보낼 정보" << std::endl;
        std::cout << report.strHash << std::endl;
        std::cout << report.strDectName << std::endl;
        std::cout << report.nSeverity << std::endl;
        std::cout << std::endl;
        if(!sendStaticEngineResult(argv[2], report))
            return -1;

    }
    staticEngine->SaveResult(outReport);
    delete staticEngine;

    return 0;

}