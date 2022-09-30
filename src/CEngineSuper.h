#pragma once
#include <iostream>
#include <vector>

/*
 * 악성코드의 행위에 관련된 구조체로써
 * 최종 엔진에서 탐지 결과을 제공하기 위해 사용된다.
*/ 
typedef struct ST_BEHAVIOR{
    std::string strName;    // 행위 이름
    std::string strDesc;    // 행위 설명
    int nSeverity;          // 위험도
}ST_BEHAVIOR;

/*
 * 정적엔진에 입력되는 입력값들에 대한 Parameter 객체
*/
typedef struct ST_ANALYZE_PARAM{
    std::vector<std::string> vecInputFiles; // C&C에서 다운받은 파일들
    std::vector<std::string> vecURLs;       // C&C URL 리스트
} ST_ANALYZE_PARAM;

/*
 * 정적엔진의 분석결과를 제공하기 위한 객체
*/
typedef struct ST_ANALYZE_RESULT{
    std::vector<std::string> vecExtractedFiles; // C&C에서 다운받은 파일들
    std::vector<std::string> vecExtractedUrls;  // C&C URL 리스트
    std::vector<ST_BEHAVIOR> vecBehaviors;      // 정적엔진 분석 결과 리스트
} ST_ANALYZE_RESULT;

/*
 * 각 엔진의 부모 클래스로써
 * 해당 객체를 상속받아 각각의 엔진은
 * 필요한 함수 및 클래스들을 구성한다.
*/
class CEngineSuper{
private:
    int m_nPriority;
    
public:
    CEngineSuper();
    ~CEngineSuper();
    virtual ST_ANALYZE_RESULT Analyze(ST_ANALYZE_PARAM input, ST_ANALYZE_RESULT output)=0;
    int GetPriority();
};