#ifndef ldfReader_ErrData_H
#define ldfReader_ErrData_H

#include <vector>

//#include "EventSummaryCommon.h"

namespace ldfReader {

    /** @class ErrData
      * @brief Local storage of error data
      * $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/ErrData.h,v 1.6 2005/01/25 09:20:26 heather Exp $
    */
    class ErrData {
    public:

        ErrData() { clear(); };
        ErrData(const ErrData& error) { 
            //m_summary = error.m_summary; 
            m_lenInBytes = error.m_lenInBytes; 
            m_exist = error.m_exist; };
        //ErrData(const EventSummaryCommon &summary) { clear();  }
        ~ErrData() { clear(); };

        void clear() { 
            //m_summary.setSummary(0); 
            m_lenInBytes = 0; 
            m_exist = false; };

       void print() const {
           if (!exist()) {
               printf("No Error Data\n");
               return;
           }
           printf("Error Data:\n");
           printf("Len: %lu\n\n", m_lenInBytes);
        }

        //const EventSummaryCommon& summary() const { return m_summary; };
        //void setSummary(unsigned summary) { m_summary.setSummary(summary);};

        void setExist() { m_exist = true; };
        bool exist() const { return m_exist; };

        void initLength(unsigned long length) { m_lenInBytes = length; };
        unsigned long lenInBytes() const { return m_lenInBytes; };

        void initPacketError(unsigned packetError) { m_packetError=packetError; };
        unsigned packetError() const { return m_packetError; };

    private:

        // Store the event sequence number for this contribution
        //EventSummaryCommon m_summary;
        // denotes if this contribution exists for this event
        bool m_exist;

        unsigned m_packetError;
        unsigned long m_lenInBytes;
    };
} // end namespace
#endif
