#pragma once
#include <iostream>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFONAME "./fifo"

/*
 * 서버로 결과를 전달하기 위해 사용하는 구조체
*/
typedef struct ST_REPORT{
    std::string strHash;                    // 파일의 hash값
    std::string strName;                    // 파일의 이름
    std::string strDetectName;              // ?
    int nSeverity;                          // 위험도
    std::vector<ST_BEHAVIOR> vecBehaviors;  // 행위 분석 결과들
}ST_REPORT;

/*
 * 서버에게 파이프를 통해 결과를 전달하기 위해 사용하는 구조체
*/
typedef struct ST_SERVER_REPORT {
    char strHash[64];                       // 파일의 위치
    char strDectName[40];                   // 분석 결과
    int Serverity;                          // 위험도(행위 분석 결과 평균)
}ST_SERVER_REPORT;

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
    std::vector<CEngineSuper*> m_Engines;

    std::string makeValue(ST_REPORT& outReport);
    std::string extractFileHash(std::string filepath);

public:
    CNoSpear();
    ~CNoSpear();
    bool SaveResult(ST_REPORT& outReport);
    bool Analyze(std::string strSampleFile, ST_REPORT& outReport);
};