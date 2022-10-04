#include "CEngineSuper.h"
CEngineSuper::CEngineSuper(int m_nPriority)
{
    this->m_nPriority = m_nPriority;
}
CEngineSuper::~CEngineSuper()
{
    
}

int CEngineSuper::GetPriority(){
    return this->m_nPriority;
}