#ifndef ldfReader_AemData_H
#define ldfReader_AemData_H

#include <vector>

#include "EventSummaryCommon.h"

namespace ldfReader {

    /** @class AemData
      * @brief Local storage of AEM data
      * $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/AemData.h,v 1.1 2004/07/29 22:00:50 heather Exp $
    */
    class AemData {
    public:

        AemData() { clear(); };
        AemData(const AemData& aem) { m_summary = aem.m_summary; };
        AemData(const EventSummaryCommon &summary) { m_summary = summary; };
        ~AemData() { clear(); };

        void clear() { m_summary.setSummary(0); };

        const EventSummaryCommon& summary() const { return m_summary; };
        void initSummary(unsigned summary) { m_summary.setSummary(summary);};

    private:

        // Store the event sequence number for this contribution
        EventSummaryCommon m_summary;

    };
} // end namespace
#endif
