#include <iostream>
#include <string>

/*
 * 예외처리를 담당하는 부모 클래스
*/
class ExceptionSuper {
private:
    std::string exceptionEngine;

public:
    ExceptionSuper(std::string engine);                             // 생성자
    ~ExceptionSuper();                                              // 소멸자
    
    std::string getExceptionEngine();                               // 예외처리가 일어난 엔진
    virtual std::string getExceptionDetail() = 0;                   // 예외처리가 일어난 원인을 제공하는 함수
};

/*
 * 예외처리를 담당하는 부모 클래스를 상속받아 
 * UrlExtractionEngine에서 일어나는 모든 에외를 담당하는
 * 예외처리 클래스이다.
*/
class UrlExtractionException : public ExceptionSuper {
private:
    std::string exceptionDetail;                                    // 예외처리가 일어난 원인

public:
    UrlExtractionException(const char* detail);                     // 생성자
    ~UrlExtractionException();                                      // 소멸자

    std::string getExceptionDetail();                               // 예외처리가 일어난 원인을 제공하는 함수
};

class DownloadFromUrlException : public ExceptionSuper {
private:
    int checker;                                                    // 어떠한 예외가 발생했는지 확인하는 변수
    std::string exceptionDetail;                                    // 일반적인 예외로써 1에 해당한다.
    std::string mysqlExceptionDetail;                               // mysql과 관련된 예외로써 2에 해당한다.

public:
    DownloadFromUrlException(const char* detail, int checker);      // 생성자
    ~DownloadFromUrlException();                                    // 소멸자

    std::string getExceptionDetail();                               // 예외처리가 일어난 원인을 제공하는 함수
};