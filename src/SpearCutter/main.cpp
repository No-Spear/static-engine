#include "CNoSpear.h"
#include "HelpFunc.h"

int main(int argc, char** argv)
{
    if(argc != 4)
    {
        std::cout << "syntax : spearCutter <FileName> <FileLocation> <UnnamedPipe>" << std::endl;
	    std::cout << "sample:  spearCutter test.docx ./temp/testdocx 3" << std::endl;
        return -1;
    }

    std::vector<ST_ANALYZE_FILES> sampleFiles;
    ST_ANALYZE_FILES sampleFile;
    sampleFile.strFileName = TCSFromMBS(std::string(argv[1]));
    sampleFile.strSampleFilePath = TCSFromMBS(std::string(argv[2]));
    sampleFiles.push_back(sampleFile);

    CNoSpear spearCutter = CNoSpear();
    ST_REPORT outReport;

    std::cout << "SpearCutter Engine 분석 시작" << std::endl;
    // 분석할 파일이 없을 때까지 반복
    while(!sampleFiles.empty())
    {
        // 각종 플래그를 모두 초기화
        spearCutter.InitializeFlags();
        spearCutter.AddEngine(new CCheckInternalCVEEngine());
        spearCutter.AddEngine(new CURLExtractEngine());
        spearCutter.AddEngine(new CDownloadFromUrlEngine());
        spearCutter.AddEngine(new CScriptExtractionEngine());
        spearCutter.AddEngine(new CScriptAnalyzeEngine());
        spearCutter.AddEngine(new CMacroExtractionEngine());
    
        std::cout << "현재 분석하는 파일: " << sampleFiles.front().strFileName << std::endl;
        // 엔진에게 파일의 이름과 위치와 결과를 저장할 값을 넘겨준다.
        if(spearCutter.Analyze(sampleFiles.front().strFileName, sampleFiles.front().strSampleFilePath, outReport, sampleFiles) != EC_SUCCESS)
            std::cout << "예외 루틴이 발생하였으나 처리 완료\n" << std::endl;

        sampleFiles.erase(sampleFiles.begin());
    }

    // DB로 분석 결과를 전달
    spearCutter.SaveResultToDB(outReport);

    if(!sendResultToServer(argv[3], outReport))
    {
        std::cout << "Cant Send Data to Server" << std::endl;
        return -1;
    }
    
    return 0;
}