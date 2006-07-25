#ifndef ldfReader_AdfData_CXX
#define ldfReader_AdfData_CXX

#include "ldfReader/data/AdfData.h"
#include <string.h>

namespace ldfReader {
        AdfData::AdfData(const AdfData& aem) {
           clear();
           m_lenInBytes = aem.m_lenInBytes;
           m_exist = aem.m_exist;
           m_evtNum = aem.m_evtNum;
           m_packetError = aem.m_packetError;
           m_buf = new unsigned char[m_lenInBytes];
            memcpy(m_buf, aem.m_buf, m_lenInBytes);
                };

    void AdfData::initBuffer(const unsigned char* buf, unsigned int len) { 
         m_buf = new unsigned char[len];
         memcpy(m_buf, buf, len);
    }

} // end namespace
#endif
