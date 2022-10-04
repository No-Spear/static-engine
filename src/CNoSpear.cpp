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
    if(!staticEngine->Analyze(inputfile, &outReport))
        return -1;
    return 0;
}