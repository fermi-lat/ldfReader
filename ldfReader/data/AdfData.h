#ifndef ldfReader_AdfData_H
#define ldfReader_AdfData_H
#include <stdio.h>
namespace ldfReader {

    /** @class AdfData
      * @brief Local storage of Ancillary data from beamtest 2006
      * $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/AdfData.h,v 1.1 2006/07/24 20:07:41 heather Exp $
    */
    class AdfData {
    public:

        AdfData() { clear(); };
        AdfData(const AdfData& aem);

        ~AdfData() { clear(); };

        void clear() { 
            m_lenInBytes = 0; 
            m_exist = false;
            m_evtNum = 0;
            m_packetError=0; 
            if (m_buf)  delete [] m_buf;
            m_buf = 0;};

        void print() const {
            if (!exist()) {
                printf("No ADF\n");
                return;
            }
            printf("ADF: \n");
            printf("EvtNum: %lu\n", m_evtNum);    
            printf("Len:  %lu\n", m_lenInBytes);
         }

       void setExist() { m_exist = true; };
       bool exist() const { return m_exist; };

       void initBuffer(const unsigned char* buf, unsigned int len);
       const unsigned char* buffer() const { return m_buf; }

       void initLength(unsigned long length) { m_lenInBytes = length; };
       unsigned long lenInBytes() const { return m_lenInBytes; };

       void initEventNum(unsigned long evtNum) { m_evtNum = evtNum;}
       unsigned long evtNum() const { return m_evtNum; }

       void initPacketError(unsigned packetError) { m_packetError=packetError; };
       unsigned packetError() const { return m_packetError; };

    private:

        bool m_exist;
        unsigned long m_lenInBytes; 
        unsigned long m_evtNum;
        unsigned char* m_buf;
        unsigned m_packetError;


    };
} // end namespace
#endif
