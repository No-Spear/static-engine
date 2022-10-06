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
    values = values + "," + "'" + outReport.strDetectName + "'";
    values = values + "," + "'" + outReport.strDetectName + "'";
    std::string vecValues = ", '{";
    for(int i = 0; i < outReport.vecBehaviors.size(); i++){
        if(i == 0){
            vecValues = vecValues + " [" + std::to_string(i) + "]Behavior :" + "{";    
        }else{
        vecValues = vecValues + ", [" + std::to_string(i) + "]Behavior :" + "{";    
        }
        vecValues = vecValues + "serverity : " + std::to_string(outReport.vecBehaviors[i].nSeverity);
        vecValues = vecValues + "Desc : " + outReport.vecBehaviors[i].strDesc;
        vecValues = vecValues + "Name : " + outReport.vecBehaviors[i].strName;
        vecValues = vecValues + "}";
     
    }
    vecValues = vecValues + "}'";

    values = values + vecValues + ")";

    return values;
}

bool CNoSpear::SaveResult(ST_REPORT& outReport)
{
    char DBHost[] = "localhost";
    char DBUser[] = "root";
    char DBPass[] = "DBPW1234";
    char DBName[] = "VSERVER";
    mysql_init(&connect);
    conn = mysql_real_connect(&connect, DBHost, DBUser , DBPass, DBName, 3306, (char *)NULL, 0);
    if(conn == NULL)
    {
        fprintf(stderr, "Mysql connection error : %s", mysql_error(&connect));
    }

    string values = makeValue(outReport);

    string sql ="INSERT INTO ResultTB(serverity,detectName,sha256,name,behaviors) VALUES" + values;

    std::cout << sql << std::endl;
    if(mysql_query(conn,sql.c_str()) !=0){
        return false;
    }

    result = mysql_store_result(conn);
    if ((row = mysql_fetch_row(result)) == NULL){
        return false;
    }
    mysql_close(conn);
    return true;



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
    
    // 결과를 전달하기 위한 코드
    outReport.vecBehaviors.reserve(output.vecBehaviors.size() + outReport.vecBehaviors.size());
    outReport.vecBehaviors.insert(outReport.vecBehaviors.end(), output.vecBehaviors.begin(), output.vecBehaviors.end());
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
        std::cout << outReport.vecBehaviors[i].nSeverity << std::endl;

    }
    return 0;
}