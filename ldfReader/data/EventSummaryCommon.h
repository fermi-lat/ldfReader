/* @class EventSummaryCommon
* @brief Class for accessing fields in the event summary longword.
* $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/EventSummaryCommon.h,v 1.1 2004/08/25 22:34:14 heather Exp $
*/

#ifndef ldfReaderEventSummaryCommon_H
#define ldfReaderEventSummaryCommon_H

#include "eventSummary.h"

namespace ldfReader {

    class EventSummaryCommon : public EventSummary
    {
    public:

        EventSummaryCommon() { m_summary =0; };
        EventSummaryCommon(unsigned summary) : m_summary(summary) { };
        ~EventSummaryCommon() {}
          
        void clear() { m_summary = 0; }

        void print(const char *pfx="") const {
            printf("Summary         = 0x%08x\n", m_summary);
            printf("%s     calStrobe = %d\n", pfx, EventSummary::calStrobe     (m_summary));
            printf("%s           tag = %d\n", pfx, EventSummary::tag           (m_summary));
            printf("%s          TACK = %d\n", pfx, EventSummary::TACK          (m_summary));
            printf("%s      readout4 = %d\n", pfx, EventSummary::readout4      (m_summary));
            printf("%s  zeroSuppress = %d\n", pfx, EventSummary::zeroSuppress  (m_summary));
            printf("%s        marker = %d\n", pfx, EventSummary::marker        (m_summary));
            printf("%s         error = %d\n", pfx, EventSummary::error         (m_summary));
            printf("%s    diagnostic = %d\n", pfx, EventSummary::diagnostic    (m_summary));
            printf("%s   eventNumber = %d\n", pfx, EventSummary::eventNumber   (m_summary));
            printf("%strgParityError = %d\n", pfx, EventSummary::trgParityError(m_summary));

        };

        void setSummary(unsigned summary) { m_summary = summary; }
        unsigned summary() {return m_summary;}
        unsigned summary() const {return m_summary;}

        unsigned long eventSequence() const {
            unsigned eventNumber = EventSummary::eventNumber(m_summary);
            unsigned tag = EventSummary::tag(m_summary);
            return ((eventNumber << 2) | tag);
        }


    private:
        unsigned m_summary;
    };
}
#endif
