#pragma once
#include <iostream>
#include <vector>

/*
 * 악성코드의 행위에 관련된 구조체로써
 * 최종 엔진에서 탐지 결과을 제공하기 위해 사용된다.
*/ 
typedef struct ST_BEHAVIOR{
    std::string strUrl;     // 행위를 검출한 url 주소
    std::string strName;    // 행위 이름
    std::string strDesc;    // 행위 설명
    int Severity;           // 위험도
}ST_BEHAVIOR;

/*
 * 정적엔진에 입력되는 입력값들에 대한 Parameter 객체
*/
typedef struct ST_ANALYZE_PARAM{
    std::vector<std::pair<std::string, int> >vecInputFiles;                     // 검사할 문서 또는 다운로드 파일
    std::vector<std::string> vecURLs;                                           // C&C URL 리스트
    std::vector<std::pair<std::string, std::pair<int, int>> >vecScriptFIles;    // 문서파일에서 추출한 스크립트 파일, URL 벡터 위치, 스크립트 타입을 담고 있다.
} ST_ANALYZE_PARAM;

/*
 * 정적엔진의 분석결과를 제공하기 위한 객체
*/
typedef struct ST_ANALYZE_RESULT{
    std::vector<std::pair<std::string, int> > vecExtractedFiles;                // C&C에서 다운받은 파일들
    std::vector<std::string> vecExtractedUrls;                                  // C&C URL 리스트
    std::vector<std::pair<std::string, std::pair<int, int>> >vecExtractedScript;// 문서 파일에서 추출된 스크립트 파일, URL 벡터 위치, 스크립트 타입을 담고 있다.
    std::vector<ST_BEHAVIOR> vecBehaviors;                                      // 정적엔진 분석 결과 리스트
} ST_ANALYZE_RESULT;

/*
 * 엔진을 통해 다운 받은 파일에 대한 상태를 의미한다.
*/
enum downloadFileStatus
{
    CONF,   // CnC On New File
    COOF,   // CnC On Old File   
    CCNF,   // CnC Close New File
    CCOF,   // CnC Close Old File
    ASF     // Analyze Sample File
};

/*
 * 엔진을 통해 추출된 스크립트의 타입을 의미한다.
*/
enum extractedScriptType
{
    JS,     // JavaScript
    PS,     // PowerShell
    VBS     // Visual Basic Script
};

/*
 * 각 엔진의 부모 클래스로써
 * 해당 객체를 상속받아 각각의 엔진은
 * 필요한 함수 및 클래스들을 구성한다.
*/
class CEngineSuper{
private:
    std::string engineType;
    int m_nPriority;
    
public:
    CEngineSuper(int m_nPriority, std::string engineType);
    ~CEngineSuper();
    virtual bool Analyze(const ST_ANALYZE_PARAM *input, ST_ANALYZE_RESULT *output)=0;
    int GetPriority();
    std::string getEngineType(void);
};