#include "HelpFunc.h"
#include "CSpearCutter.h"

/*
 * 서버에게 파이프를 통해 결과를 전달하기 위해 사용하는 구조체
*/
typedef struct ST_SERVER_REPORT {
    char strName[513];                      // 파일의 이름
    char strHash[65];                       // 파일의 해쉬값
    char strDectName[40];                   // 분석 결과
    int nSeverity;                          // 위험도(행위 분석 결과 평균)
}ST_SERVER_REPORT;


// 엔진에게 결과를 보내는 함수
bool sendResultToServer(const char* pipe, const ST_REPORT outReport)    
{
    ST_SERVER_REPORT report;
    strcpy(report.strName, outReport.strName.c_str());
    strcpy(report.strHash, outReport.strHash.c_str());
    strcpy(report.strDectName, outReport.strDetectName.c_str());
    report.nSeverity = outReport.nSeverity;

    std::cout << "서버로 보내는 정보: " << std::endl;
    std::cout << "FileName: " <<  report.strName << std::endl;
    std::cout << "FileHash: " << report.strHash << std::endl;
    std::cout << "DecteName: " << report.strDectName << std::endl;
    std::cout << "Severity: " << report.nSeverity << "\n" << std::endl;

    int filedes = atoi(pipe);
    if(filedes < 0)
    {
        std::cout << "failed to call fifo" << std::endl;
        return false;
    }

    int send = write(filedes, &report,sizeof(report));

    if(send<0)
    {
        std::cout << "failed to write fifo" << std::endl;
        return false;
    }
    close(filedes);
    return true;
}

// base64 디코더
std::string base64_decoder(const std::string& input)
{
    static const std::string b = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
 
    std::string result;
    std::vector<int> T(256, -1);
 
    for (int i = 0; i < 64; i++)
        T[b[i]] = i;
 
    int val = 0;
    int valb = -8;
 
    for (u_char c : input) 
    {
        if (T[c] == -1)
            break;
 
        val = (val << 6) + T[c];
        valb += 6;
 
        if (valb >= 0) 
        {
            result.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return result;
}
