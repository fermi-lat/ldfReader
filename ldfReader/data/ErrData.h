#ifndef ldfReader_ErrData_H
#define ldfReader_ErrData_H

#include <vector>

//#include "EventSummaryCommon.h"

namespace ldfReader {

    /** @class ErrData
      * @brief Local storage of error data
      * $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/ErrData.h,v 1.7 2005/01/27 21:57:29 heather Exp $
    */
    class ErrData {
    public:

        ErrData() { clear(); };
        ErrData(const ErrData& error) { 
            m_lenInBytes = error.m_lenInBytes; 
            m_exist = error.m_exist; };
        ~ErrData() { clear(); };

        void clear() { 
            m_lenInBytes = 0; 
            m_exist = false; };

       void print() const {
           if (!exist()) {
               printf("No Error Data\n");
               return;
           }
           printf("Error Data:\n\n");
        }


        void setExist() { m_exist = true; };
        bool exist() const { return m_exist; };

        void initLength(unsigned long length) { m_lenInBytes = length; };
        unsigned long lenInBytes() const { return m_lenInBytes; };

        void initPacketError(unsigned packetError) { m_packetError=packetError; };
        unsigned packetError() const { return m_packetError; };

    private:

        // denotes if this contribution exists for this event
        bool m_exist;

        unsigned m_packetError;
        unsigned long m_lenInBytes;
    };
} // end namespace
#endif
