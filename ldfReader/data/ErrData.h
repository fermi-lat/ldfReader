#ifndef ldfReader_ErrData_H
#define ldfReader_ErrData_H

#include <vector>

#include "EventSummaryCommon.h"

namespace ldfReader {

    /** @class ErrData
      * @brief Local storage of error data
      * $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/ErrData.h,v 1.1 2004/08/25 22:34:14 heather Exp $
    */
    class ErrData {
    public:

        ErrData() { clear(); };
        ErrData(const ErrData& error) { 
            m_summary = error.m_summary; 
            m_lenInBytes = error.m_lenInBytes; };
        ErrData(const EventSummaryCommon &summary) { m_summary = summary; }
        ~ErrData() { clear(); };

        void clear() { 
            m_summary.setSummary(0); 
            m_lenInBytes = 0; };

        const EventSummaryCommon& summary() const { return m_summary; };
        void setSummary(unsigned summary) { m_summary.setSummary(summary);};

        void initLength(unsigned long length) { m_lenInBytes = length; };
        unsigned long lenInBytes() const { return m_lenInBytes; };

    private:

        // Store the event sequence number for this contribution
        EventSummaryCommon m_summary;

        unsigned long m_lenInBytes;
    };
} // end namespace
#endif
