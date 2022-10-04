#include "CURLExtractionEngine.h"
#include "CDownloadFromUrlEngine.h"

int main(int argc, char* argv[])
{
    ST_ANALYZE_PARAM input;
    ST_ANALYZE_RESULT output;

    std::string inputfile(argv[1]);
    input.vecInputFiles.push_back(inputfile);

    CURLExtractEngine* url = new CURLExtractEngine();
    url->Analyze(&input, &output);
    for(int i = 0; i< input.vecURLs.size(); i++)
        std::cout << input.vecURLs[i] << std::endl;

    // const std::string url = string("https://4nul.org:3000/download");
    // ST_ANALYZE_PARAM * param = (ST_ANALYZE_PARAM *)malloc(sizeof(ST_ANALYZE_PARAM));
    // ST_ANALYZE_RESULT * result = (ST_ANALYZE_RESULT *)malloc(sizeof(ST_ANALYZE_RESULT));
    // param->vecURLs.push_back(url);
    CDownloadFromUrlEngine fileDownloader;

    if(!fileDownloader.Analyze(&input, &output)){
        return 0;
    }

    for(int i =0; i < output.vecExtractedFiles.size(); i++)
        std::cout << output.vecExtractedFiles[i] << std::endl;
    
    return 0;
}

   