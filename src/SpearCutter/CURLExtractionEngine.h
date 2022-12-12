#pragma once
#include "CEngineSuper.h"
#include "CException.h"
#include "../DocumentParser/DocumentParser.h"
#include "SampleFileInfo.h"
#include "struct.h"
#include <regex>    

/*
 * Document의 XML RELS 데이터에서
 * URL을 추출을 위한 클래스
 * CEngineSuper를 상속받았다.
*/
class CURLExtractEngine : public CEngineSuper {
private:
    std::vector<std::string> GetOOXMLFormat_ExternalURL();
    std::vector<std::string> GetCompoundFormat_ExternalURL();

public:
    CURLExtractEngine();                                                                            // 클래스 생성자
    ~CURLExtractEngine();                                                                           // 클래스 소멸자
    bool Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output);                         // 검사 함수
};