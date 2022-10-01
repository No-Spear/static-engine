#pragma once
#include "CEngineSuper.h"
#include <regex>    // 정규표현식을 위한 라이브러리
#include <string>
#include <sstream>
#include <zip.h>    // zip
// export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/arad4228/Desktop/zip/build
// 다이나믹 라이브러리 추가

/*
 * OOXML 객체의 공통 클래스
*/
class OOXml{
private:
    const char* contentxml = "word/_rels/document.xml.rels";    // Docx XML파일의 위치
    std::string docname;                                        // 문서의 이름
    std::string doctype;                                        // 문서 타입
    zip_t* document;                                            // zip 파일 형식으로 파일을 변환

    std::string parsing(std::string input);                     // 파싱한 문자열에서 URL을 추출한다.

public:
    OOXml(const char* docpath);                                 // 클래스 생성자
    ~OOXml();                                                   // 클래스 소멸자
    std::string getDocType();                                   // 문서 타입을 반환
    bool getUrlData(std::vector<std::string>& output);          // 파일에서 C&C URL을 가져온다.
};

// Class 나누기는 xlsx를 진행할때 진행
/*
 *  OOXML객체중 Docx에 대한 객체
*/
// class Docx : public OOXml {

// };

/*
 * Document의 XML 데이터에서
 * URL을 추출을 위한 클래스
 * CEngineSuper를 상속받았다.
*/
class CURLExtractEngine : public CEngineSuper {
private:
    OOXml* document;                                                        // 검사를 원하는 문서
    
    bool urlParsing(std::string input, std::vector<std::string>& output);   // URL을 파싱하기 위한 함수

public:
    CURLExtractEngine();                                                    // 클래스 생성자
    ~CURLExtractEngine();                                                   // 클래스 소멸자
    bool Analyze(ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output);       // 검사 함수
};