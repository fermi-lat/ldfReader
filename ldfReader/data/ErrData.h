#ifndef ldfReader_ErrData_H
#define ldfReader_ErrData_H

#include <vector>

#include "EventSummaryCommon.h"

namespace ldfReader {

    /** @class ErrData
      * @brief Local storage of error data
      * $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/ErrData.h,v 1.4 2004/10/01 06:22:30 heather Exp $
    */
    class ErrData {
    public:

        ErrData() { clear(); };
        ErrData(const ErrData& error) { 
            m_summary = error.m_summary; 
            m_lenInBytes = error.m_lenInBytes; 
            m_exist = error.m_exist; };
        ErrData(const EventSummaryCommon &summary) { clear(); m_summary = summary; }
        ~ErrData() { clear(); };

        void clear() { 
            m_summary.setSummary(0); 
            m_lenInBytes = 0; 
            m_exist = false; };

        const EventSummaryCommon& summary() const { return m_summary; };
        void setSummary(unsigned summary) { m_summary.setSummary(summary);};

        void setExist() { m_exist = true; };
        bool exist() const { return m_exist; };

        void initLength(unsigned long length) { m_lenInBytes = length; };
        unsigned long lenInBytes() const { return m_lenInBytes; };

        void initPacketError(unsigned packetError) { m_packetError=packetError; };
        unsigned packetError() const { return m_packetError; };

    private:

        // Store the event sequence number for this contribution
        EventSummaryCommon m_summary;
        // denotes if this contribution exists for this event
        bool m_exist;

        unsigned m_packetError;
        unsigned long m_lenInBytes;
    };
} // end namespace
#endif
