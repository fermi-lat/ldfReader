#ifndef ldfReader_OswData_H
#define ldfReader_OswData_H

#include <vector>

#include "EventSummaryCommon.h"

namespace ldfReader {

    /** @class OswData
      * @brief Local storage of OSW data
      * $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/OswData.h,v 1.2 2004/09/21 17:57:19 heather Exp $
    */
    class OswData {
    public:

        OswData() { clear(); };
        OswData(const OswData& osw) { 
            m_summary = osw.m_summary; 
            m_lenInBytes = osw.m_lenInBytes; 
            m_exist = osw.m_exist; };
        OswData(const EventSummaryCommon& summary) { m_summary = summary; };
        ~OswData() { clear(); };

        void clear() { 
            m_summary.setSummary(0); 
            m_lenInBytes = 0; 
            m_exist = false;};

        const EventSummaryCommon& summary() const { return m_summary; };
        void initSummary(unsigned summary) { m_summary.setSummary(summary);};

       void setExist() { m_exist = true; };
       bool exist() const { return m_exist; };

        void initLength(unsigned long length) { m_lenInBytes = length; };
        unsigned long lenInBytes() const { return m_lenInBytes; };

    private:

        // Store the event sequence number for this contribution
        EventSummaryCommon m_summary;
        bool m_exist;

        unsigned long m_lenInBytes;

    };
} // end namespace
#endif
