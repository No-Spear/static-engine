#pragma once
#include "CEngineSuper.h"
#include <fstream>
#include <string>
#include <sstream>
#include <regex>    // 정규표현식을 위한 라이브러리
#include <zip.h>    // zip
// export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/arad4228/Desktop/zip/build
// 다이나믹 라이브러리 추가

/*
 * 문서파일의 포맷 형식에 대한 부모 클래스로써
 * 이를 상속받아 Compound, OOXML 형식의 문서
 * 파일의 분석을 정의한다.
*/
class ContainerParserSuper
{
public:
    ContainerParserSuper();                                                 // 생성자
    ~ContainerParserSuper();                                                // 소멸자

    virtual bool open(const char* pszFile) = 0;                             // 문서파일을 여는 함수
    virtual bool close(void) =  0;                                          // 문서파일을 닫는 함수
    virtual char* getStreamData(const char* location) = 0;                  // 문서파일에서 XML 데이터를 가져오는 함수
};

/*
 * Compound 형식의 문서 파일 포맷 분석을 위한 클래스
*/
class CompoundParser : public ContainerParserSuper
{
protected:
    std::string parsingUrl(std::string combinedUrlData);

public:
    CompoundParser();
    ~CompoundParser();
};

/*
 * OOXML 형식의 문서 파일 포맷 분석을 위한 클래스
*/
class OOXMLParser : public ContainerParserSuper
{
private:
    zip_t* OOXML;                                                           // 분석하려는 문서 파일
    void* buffer;                                                           // 문서파일의 스트림 데이터
    size_t bufsize;                                                         // 문서파일의 스트림 데이터 크기

public:
    OOXMLParser();                                                          // 생성자
    ~OOXMLParser();                                                         // 소멸자

    bool open(const char* pszFile);                                         // 문서파일을 열고 스트림 데이터를 저장하는 함수
    bool close(void);                                                       // 열었던 문서파일을 닫고 메모리 정리하는 함수
    char* getStreamData(const char* location);                              // 문서파일의 하위 스트림 데이터를 전달해주는 함수
    // 스트림 구조의 패스를 입력. 객체의 패스를 입력해서 streamData를 돌려준다.
};

/*
 * 문서 파일의 형식에 대한 부모 클래스
 * 이를 상속받아 word, excel, ppt 형식의 문서
 * 파일을 분석을 정의한다.
*/
class DocumentParserSuper
{
protected:
    ContainerParserSuper* container;                                        // 문서 파일의 포맷 형식 변수

public:
    DocumentParserSuper(ContainerParserSuper* pContainer);                  // 생성자
    ~DocumentParserSuper();                                                 // 소멸자

    virtual std::vector<std::string> getUrlList(std::string samplePath) = 0;// UrlList를 찾아 돌려주는 함수
};

/*
 * Word 문서 파일의 분석을 위한 클래스
*/
class WordParser : public DocumentParserSuper
{
private:
    std::string parsingUrl(const std::string input);                        // 정제되지 않은 Url 데이터에서 Url을 돌려주는 함수

public:
    WordParser(ContainerParserSuper* pContainer);                           // 생성자
    ~WordParser();                                                          // 소멸자

    std::vector<std::string> getUrlList(std::string samplePath);            // 문서파일에 내장된 스트림 데이터에서 외부 객체 Url을 돌려주는 함수
};

/*
 * Excel 문서 파일의 분석을 위한 클래스
*/
class ExcelParser : public DocumentParserSuper
{
private:
    std::string parsingUrl(const std::string input);                        // 정제되지 않은 Url 데이터에서 Url을 돌려주는 함수
    std::string parsingContentxml(const std::string input);                 // 정제되지 않은 상위 stream에서 contentxml을 돌려주는 함수
    std::vector<std::string> getContenxmlList(const char* highStream);      // 상위 스트림의 데이터에서 하위 스트림 리스트를 돌려주는 함수

public:
    ExcelParser(ContainerParserSuper* pConrainer);                          // 생성자
    ~ExcelParser();                                                         // 소멸자

    std::vector<std::string> getUrlList(std::string samplePath);            // 문서파일에 내장된 스트림 데이터에서 외부 객체 Url을 돌려주는 함수
};

/*
 * Ppt 문서 파일의 분석을 위한 클래스
*/
class PowerPointParser : public DocumentParserSuper
{
private:
    std::string parsingUrl(const std::string input);                        // 정제되지 않은 Url 데이터에서 Url을 돌려주는 함수
    std::vector<std::string> getContenxmlList(const char* highStream);      // 상위 스트림의 데이터에서 하위 스트림 리스트를 돌려주는 함수

public:
    PowerPointParser(ContainerParserSuper* pConrainer);                     // 생성자
    ~PowerPointParser();                                                    // 소멸자

    std::vector<std::string> getUrlList(std::string samplePath);            // 문서파일에 내장된 스트림 데이터에서 외부 객체 Url을 돌려주는 함수

};

/*
 * Document의 XML 데이터에서
 * URL을 추출을 위한 클래스
 * CEngineSuper를 상속받았다.
*/
class CURLExtractEngine : public CEngineSuper {
private:
    DocumentParserSuper* sampleDocument;                                                            // 문서파일

    std::string extractFileExetoPath(const std::string docpath);                                    // 문서의 위치로부터 문서 타입을 가져오기 위한 함수
    std::string extractFileExetoSignature(const std::string docpath);                               // 문서의 시그니처로부터 문서 타입을 가져오는 함수
    bool urlParsing(std::string input, std::string doctype, std::vector<std::string>& output);      // URL을 파싱하기 위한 함수

public:
    CURLExtractEngine();                                                                            // 클래스 생성자
    ~CURLExtractEngine();                                                                           // 클래스 소멸자
    bool Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output);                         // 검사 함수
};