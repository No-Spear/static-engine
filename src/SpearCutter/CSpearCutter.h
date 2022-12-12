#pragma once
#include "CException.h"
#include "CCheckInternalCVEEngine.h"
#include "CURLExtractionEngine.h"
#include "CDownloadFromUrlEngine.h"
#include "CScriptExtractionEngine.h"
#include "CScriptAnalyzeEngine.h"
#include "CMacroExtractionEngine.h"
#include <iostream>
#include <stdio.h>
#include <mysql/mysql.h>
#include <sys/types.h>

typedef struct ST_ANALYZE_FILES {
    std::tstring strFileName;           // 파일의 원본이름
    std::tstring strSampleFilePath;         // 파일의 위치
}ST_ANALYZE_FILES;

/*
 * DB 결과를 전달하기 위해 사용하는 구조체
*/
typedef struct ST_REPORT{
    std::string strHash;                    // 파일의 hash값
    std::string strName;                    // 파일의 이름
    std::string strDetectName;              // 분석결과
    int nSeverity = 0;                      // 위험도
    std::vector<ST_BEHAVIOR> vecBehaviors;  // 행위 분석 결과들
}ST_REPORT;

/*
 * 정적분석 엔진 클래스
 * URL추출, 파일 다운로드 엔진, Script추출엔진, Script분석 엔진으로 구성되어있다.
*/
class CSpearCutter {
private:
    MYSQL* conn;
    MYSQL connect;
    MYSQL_RES* result; 
    MYSQL_ROW row;

    bool maliciousMacroFound;
    bool maliciousExternalCVEFound;
    bool maliciousInternalCVEFound;
    std::vector<CEngineSuper*> m_Engines;

    std::string MakeValue(const ST_REPORT& outReport);
    std::string ReplaceAll(std::string str, const std::string from, const std::string to);
    void MakeOutputReportForDB(const ST_ANALYZE_RESULT result, ST_REPORT& outReport, bool fileError);

public:
    CSpearCutter();
    ~CSpearCutter();
    void InitializeFlags();
    bool SaveResultToDB(const ST_REPORT& outReport);
    bool AddEngine(CEngineSuper* engine);
    ECODE Analyze(const std::tstring strFileName, const std::tstring strSampleFile, ST_REPORT& outReport, std::vector<ST_ANALYZE_FILES>& sampleFiles);
};