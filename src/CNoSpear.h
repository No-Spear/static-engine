#pragma once
#include "CURLExtractionEngine.h"
#include "CDownloadFromUrlEngine.h"
#include "CScriptExtractionEngine.h"
#include "CScriptAnalyzeEngine.h"
#include "CMacroExtractionEngine.h"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
 * DB 결과를 전달하기 위해 사용하는 구조체
*/
typedef struct ST_REPORT{
    std::string strHash;                    // 파일의 hash값
    std::string strName;                    // 파일의 이름
    std::string strDetectName;              // 분석결과
    int nSeverity;                          // 위험도
    std::vector<ST_BEHAVIOR> vecBehaviors;  // 행위 분석 결과들
}ST_REPORT;

/*
 * 서버에게 파이프를 통해 결과를 전달하기 위해 사용하는 구조체
*/
typedef struct ST_SERVER_REPORT {
    char strName[513];                      // 파일의 이름
    char strHash[65];                       // 파일의 해쉬값
    char strDectName[40];                   // 분석 결과
    int nSeverity;                          // 위험도(행위 분석 결과 평균)
}ST_SERVER_REPORT;

/*
 * 서버에서 인자를 통해 주는 파일에 대한 정보를 담기 위해 사용하는 구조체
*/
typedef struct ST_FILE_INFO {
    std::string strFileName;                // 파일의 이름
    std::string strFileHash;                // 파일의 해쉬 값
    std::string strSampleFile;              // 파일의 위치
}ST_FILE_INFO;

/*
 * 정적분석 엔진 클래스
 * URL추출, 파일 다운로드 엔진, Script추출엔진, Script분석 엔진으로 구성되어있다.
*/
class CNoSpear {
private:
    MYSQL* conn;
    MYSQL connect;
    MYSQL_RES* result; 
    MYSQL_ROW row;
    bool macroFlag;
    bool xmlFlag;
    std::vector<CEngineSuper*> m_Engines;

    std::string makeValue(const ST_REPORT& outReport);
    void makeOutputReport(const ST_FILE_INFO sampleFile ,const ST_ANALYZE_RESULT result, ST_REPORT& outReport);
    std::string replaceAll(std::string &str, const std::string& from, const std::string& to);
    
public:
    CNoSpear();
    ~CNoSpear();
    bool SaveResult(const ST_REPORT& outReport);
    bool Analyze(const ST_FILE_INFO sampleFile ,ST_REPORT& outReport);
};