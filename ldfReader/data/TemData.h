#ifndef ldfReader_TemData_H
#define ldfReader_TemData_H

#include <vector>

#include "EventSummaryCommon.h"

namespace ldfReader {

    /** @class TemData
      * @brief Local storage of TEM data
      * $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/TemData.h,v 1.1 2004/08/25 22:34:14 heather Exp $
    */
    class TemData {
    public:

        TemData() { clear(); };
        TemData(const TemData& tem) { 
            m_summary = tem.m_summary; 
            m_lenInBytes = tem.m_lenInBytes; };
        TemData(const EventSummaryCommon& summary) {m_summary = summary; };
        ~TemData() { clear(); };

        void clear() { 
            m_summary.setSummary(0); 
            m_lenInBytes = 0; };

        void print() const { 
             printf("TemData:\n");
             printf("contribution length = %d Bytes", m_lenInBytes);
        };

        const EventSummaryCommon& summary() const { return m_summary; };
        void initSummary(unsigned summary) { m_summary.setSummary(summary);};

        void initLength(unsigned long length) { m_lenInBytes = length; };
        unsigned long lenInBytes() const { return m_lenInBytes; };

    private:

        // Store the event sequence number for this contribution
        EventSummaryCommon m_summary;

        unsigned long m_lenInBytes;

    };
} // end namespace
#endif