#include "CCheckInternalCVEEngine.h"
#include "CVE_2017_11882_COM.h"
#include "CVE_2017_11882_OOXML.h"


// CCheckInternalCVEEngine 생성자
CCheckInternalCVEEngine::CCheckInternalCVEEngine() : CEngineSuper(0, "CheckInternalCVE")
{
    this->m_mapCheckLists.insert({ TEXT("CVE-2017-11882-OOXML"), new CCVE_2017_11882_OOXML()});
    this->m_mapCheckLists.insert({ TEXT("CVE-2017-11882-COM"), new CCVE_2017_11882_COM()});
}

// CCheckInternalCVEEngine 소멸자
CCheckInternalCVEEngine::~CCheckInternalCVEEngine()
{

}

bool CCheckInternalCVEEngine::Analyze(const ST_ANALYZE_PARAM* input, ST_ANALYZE_RESULT* output)
{
    std::vector<std::tstring> vecExtractFiles;
    int behaviorCount = output->vecBehaviors.size();

    ECODE nRet = EC_SYSTEM_ERROR;
    try{
        for(auto& iter : this->m_mapCheckLists)
        {
            std::cout << iter.first << "관련 취약점을 확인합니다." << std::endl;
            nRet = iter.second->Analyze(vecExtractFiles, output);
            if(nRet != EC_SUCCESS)
            {
                Log_Warn(TEXT("Checking %s Failure, %d"), iter.first.c_str(), nRet);
				    continue;
            }
        }
    }catch(const std::exception& e)
    {
        Log_Error("%s", e.what());
		return false;
    }
    for(int i = 0; i< output->vecBehaviors.size(); i++)
    {
        for(auto& iter : this->m_mapCheckLists)
        {
            if(output->vecBehaviors[i].strName == (iter.first.substr(0, iter.first.find_last_of("-"))))
                return true;
        }
    }
    // if(output->vecBehaviors.size() - behaviorCount > 0)
    //     return true;
    return false;
}