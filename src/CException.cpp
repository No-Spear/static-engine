#include "CException.h"

// 각 엔진에서 일어나는 예외처리 사항들을 다루는 클래스 생성자
engine_Exception::engine_Exception(const char* engine, const char* pszFormat, ...)
{
    // 어떠한 엔진에서 예외가 발생했는지 저장
    this->exceptionDetail.append(engine);
    this->exceptionDetail.append("Engine의 동작 중 다음과 같은 예외처리 루틴이 동작 하였습니다.\n");
    // 가변 매개변수를 처리하기 위해 선언
    va_list detail;
    int count = 0;
    // 스택 상의 첫 가변인수의 위치를 구해 detail에 대입
    va_start(detail, pszFormat);
    
    while(pszFormat[count] != '\0')
    {
        this->exceptionDetail.append(va_arg(detail, char*));
        count++;
    }
    va_end(detail);
}

// 각 엔진에서 일어나는 예외처리 사항들을 다루는 클래스 소멸자
engine_Exception::~engine_Exception()
{

}

// 예외처리 내용에 대해 돌려주는 함수
const char* engine_Exception::what() const noexcept
{
    return this->exceptionDetail.c_str();
}