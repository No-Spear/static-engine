#include "CEngineSuper.h"

CEngineSuper::CEngineSuper(int m_nPriority, std::string engineType)
{
    this->m_nPriority = m_nPriority;
    this->engineType.append(engineType);
}
CEngineSuper::~CEngineSuper()
{
    
}

int CEngineSuper::GetPriority(){
    return this->m_nPriority;
}

std::string CEngineSuper::getEngineType(void)
{
    return this->engineType;
}