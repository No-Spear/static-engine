#pragma once
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdarg>

/*
 * 각 엔진에서 일어나는 예외처리 사항을 다루기 위한 클래스
*/
class engine_Exception : public std::exception {
private:
    std::string exceptionDetail;                                         // 예외처리가 일어난 원인

public:
    engine_Exception(const char* engine, const char* pszFormat, ...);   // 생성자
    ~engine_Exception();                                                // 소멸자
    
    const char* what() const noexcept;                                  // 예외처리가 일어난 원인을 제공하는 함수
};