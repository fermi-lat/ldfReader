#ifndef ldfReader_AemData_H
#define ldfReader_AemData_H

#include <vector>

#include "EventSummaryCommon.h"

namespace ldfReader {

    /** @class AemData
      * @brief Local storage of AEM data
      * $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/AemData.h,v 1.1 2004/08/25 22:34:13 heather Exp $
    */
    class AemData {
    public:

        AemData() { clear(); };
        AemData(const AemData& aem) { 
           m_summary = aem.m_summary; 
           m_lenInBytes = aem.m_lenInBytes; };
        AemData(const EventSummaryCommon &summary) { m_summary = summary; };
        ~AemData() { clear(); };

        void clear() { 
            m_summary.setSummary(0); 
            m_lenInBytes = 0; };

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
