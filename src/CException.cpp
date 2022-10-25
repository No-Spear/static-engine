#include "CException.h"

// 예외처리를 담당하는 부모 클래스 생성자
ExceptionSuper::ExceptionSuper(std::string engine)
{
    this->exceptionEngine.append(engine);
}

// 예외처리를 담당하는 부모 클래스 소멸자
ExceptionSuper::~ExceptionSuper()
{

}

// 어떠한 엔진에서 예외처리가 발생했는지 확인하기 위한 함수
std::string ExceptionSuper::getExceptionEngine()
{
    return this->exceptionEngine;
}

// UrlExtractionEngine에서 일어나는 예외에 대한 클래스 생성자
UrlExtractionException::UrlExtractionException(const char* detail) : ExceptionSuper("UrlExtraction")
{
    this->exceptionDetail.append(detail);
}

// UrlExtractionEngine에서 일어나는 예외처리 클래스의 소멸자
UrlExtractionException::~UrlExtractionException()
{

}

// UrlExtractionEngine에서 일어나는 예외처리 내용을 돌려주는 함수
std::string UrlExtractionException::getExceptionDetail()
{
    std::string output;
    output.append(this->getExceptionEngine());
    output.append("Engine에서 다음 예외가 발생했습니다.\n");
    output.append(this->exceptionDetail);
    return output;
}

// DownloadFromUrlEngine에서 일어나는 예외처리 클래스의 생성자
DownloadFromUrlException::DownloadFromUrlException(const char* detail, int checker) : ExceptionSuper("DownloadFromUrl")
{
    this->checker = checker;
    this->exceptionDetail.append(detail);
}

// DownloadFromUrlEngine에서 일어나는 예외처리 클래스의 소멸자
DownloadFromUrlException::~DownloadFromUrlException()
{

}

// DownloadFromUrlEngine에서 일어나는 예외처리 내용을 돌려주는 함수
std::string DownloadFromUrlException::getExceptionDetail()
{
    std::string output;
    output.append(this->getExceptionEngine());
    output.append("Engine에서 다음 예외가 발생했습니다.\n");
    if(checker == 1)
        output.append(this->exceptionDetail);
    else if(checker == 2)
    {
        output.append("Mysql의 ");
        output.append(this->exceptionDetail);
    }
    return output;
}

// ScriptExtractionEngine에서 일어나는 예외처리  클래스의 생성자
ScriptExtractionException::ScriptExtractionException(const char* detail) : ExceptionSuper("ScriptExtration")
{
    this->exceptionDetail.append(detail);
}

// ScriptExtractionEngine에서 일어나는 예외처리  클래스의 소멸자
ScriptExtractionException::~ScriptExtractionException()
{

}

// ScriptExtractionEngine에서 일어나는 예외처리 내용을 돌려주는 함수
std::string ScriptExtractionException::getExceptionDetail()
{
    std::string output;
    output.append(this->getExceptionEngine());
    output.append("Engine에서 다음 예외가 발생했습니다.\n");
    output.append(this->exceptionDetail);
    return output;
}