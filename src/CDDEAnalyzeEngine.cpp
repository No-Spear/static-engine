#include "CDDEAnalyzeEngine.h"

// DDE분석엔진 생성자
CDDEAnalyzeEngine::CDDEAnalyzeEngine() : CEngineSuper(5, "DDEAnalyze")
{

}

// DDE분석엔진 소멸자
CDDEAnalyzeEngine::~CDDEAnalyzeEngine()
{

}

// 파일에 대한 타입을 확인하기 위한 함수
std::string CDDEAnalyzeEngine::extractFileExetoPath(const std::string docpath)
{
    // 파일 형식자를 찾기위해 마지막 .의 위치를 찾는다.
    int location = docpath.find_last_of('.');
    // 해당 파일형식자를 새로운 스트링에 담는다.
    std::string doctype(docpath.substr(location+1));
    // 해당 파일 형식자를 vector에 넣어둔다.
    return doctype;
}

// 파일의 시그니처를 통해 compound, OOXML형식인지 확인하기 위한 함수
std::string CDDEAnalyzeEngine::extractFileSignature(const std::string docpath)
{
    std::string signature;
    std::stringstream signaturestream;
    // 분석의뢰파일을 바이너리 형태로 데이터를 읽는다.
    std::ifstream document(docpath, std::ios::binary);

    // 만약 파일을 열지 못했다면
    if (document.fail()) 
        throw engine_Exception("DDEAnalyze", "s", "분석파일을 열고 파일의 시그니처를 확인할 수 없습니다.");

    // 파일의 시그니처 값에 해당하는 4byte를 읽어온다.
    for(int i =0; i<4; i++)
    {
        int sig = document.get();
        signaturestream << std::hex << sig;
    }
    signature = signaturestream.str();
    return signature;
}

std::vector<std::pair<std::string, std::string> >CDDEAnalyzeEngine::getXMLData(const char* path)
{
    // 현재 분석을 진행할 파일이 정상적으로 읽히는지 확인
    this->OOXML = zip_open(path, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
    if(this->OOXML == NULL)
        throw engine_Exception("DDEAnalyze", "s", "분석을 의뢰한 파일을 열 수 없습니다.");

    std::vector<std::pair<std::string, std::string> >xml;
    for(int i =0; i < zip_entries_total(this->OOXML); i++)
    {
        try{
            // 모든 엔트리를 연다.
            zip_entry_openbyindex(this->OOXML, i);
            std::string name(zip_entry_name(this->OOXML));
            if(extractFileExetoPath(name) == "xml")
            {
                void* data;
                size_t datasize;
                if(zip_entry_read(this->OOXML, &data, &datasize) < 0)
                    throw engine_Exception("DDEAnalyze", "s", "문서파일에서 해당 데이터를 읽을 수 없습니다.");
                
                std::string sData((char*)data);
                xml.push_back(std::make_pair(name, sData));
            }
            else
            {
                zip_entry_close(this->OOXML);
                continue;
            }
        } catch(std::exception& e)
        {
            std::cout << "다음의 오류가 발생했습니다.\n" << e.what() << std::endl;
            continue;
        }
    }
    return xml;
}

bool CDDEAnalyzeEngine::checkDDEKeyword(const std::string location, const std::string xmlData, std::vector<ST_BEHAVIOR>& vecBehaviors)
{
    // 탐지가 됬는지 확인하기 위한 변수
    int count = 0;
    std::smatch match;

    std::regex dde0(R"([\s]*DDE[\s]*)");
    if(std::regex_search(xmlData, match, dde0))
    {
        ST_BEHAVIOR DDE;
        DDE.strUrl.append(location);
        DDE.Severity = 8;
        DDE.strName="Call DDE Keyword in XML Data";
        DDE.strDesc="Dynamic Data Exchange에서 사용되는 DDE 키워드 호출";
        vecBehaviors.push_back(DDE);
        count++;
    }

    std::regex dde1(R"(DDEAUTO)");
    if(std::regex_search(xmlData, match, dde1))
    {
        ST_BEHAVIOR DDEAUTO;
        DDEAUTO.strUrl.append(location);
        DDEAUTO.Severity = 8;
        DDEAUTO.strName="Call DDEAUTO Keyword in XML Data";
        DDEAUTO.strDesc="Dynamic Data Exchange에서 사용되는 DDEAUTO 키워드 호출";
        vecBehaviors.push_back(DDEAUTO);
        count++;
    }

    if(count > 0)
        return true;
    return false;
}

// 분석에 대한 결과를 돌려주는 함수
bool CDDEAnalyzeEngine::Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output)
{
    // 파일에 대한 시그니처와 형식을 뽑아온다.
    std::string documentSignature = extractFileSignature(input->vecInputFiles[0].first);
    std::string documentType = extractFileExetoPath(input->vecInputFiles[0].first);

    // 파일 형식이 OOXML인지 확인
    if(documentSignature != "504b34")
        throw engine_Exception("DDEAnalyze", "s", "DynamicDataExchange를 확인할 수 있는 파일 형식이 아닙니다.");
    
    // 파일의 이름과 데이터를 set으로 저장한다.
    std::vector<std::pair<std::string, std::string> > xmlData = getXMLData(input->vecInputFiles[0].first.c_str());

    for(int i = 0; i < xmlData.size(); i++)
    {
        std::cout << "현재 탐색하는 xml: " << xmlData[i].first << std::endl;
        if(checkDDEKeyword(xmlData[i].first, xmlData[i].second, output->vecBehaviors))
            std::cout << "해당 xml내에 DDE Keyword가 있습니다." << std::endl;
        else
            std::cout << "해당 파일은 정상입니다." << std::endl;
    }

    // if(output->vecBehaviors.size() == 0)
    //     throw engine_Exception("DDEAnalyze", "s", "분석 결과가 없습니다.");
    
    return true;
}