#ifndef ldfReader_OswData_H
#define ldfReader_OswData_H

#include <vector>

#include "EventSummaryCommon.h"

namespace ldfReader {

    /** @class OswData
      * @brief Local storage of OSW data
      * $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/OswData.h,v 1.1 2004/07/29 22:00:50 heather Exp $
    */
    class OswData {
    public:

        OswData() { clear(); };
        OswData(const OswData& osw) { m_summary = osw.m_summary; };
        OswData(const EventSummaryCommon& summary) { m_summary = summary; };
        ~OswData() { clear(); };

        void clear() { m_summary.setSummary(0); };

        const EventSummaryCommon& summary() const { return m_summary; };
        void initSummary(unsigned summary) { m_summary.setSummary(summary);};

    private:

        // Store the event sequence number for this contribution
        EventSummaryCommon m_summary;

    };
} // end namespace
#endif
