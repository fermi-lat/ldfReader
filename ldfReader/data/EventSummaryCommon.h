/* @class EventSummaryCommon
* @brief Class for accessing fields in the event summary longword.
* $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/EventSummaryCommon.h,v 1.3 2005/01/27 21:57:29 heather Exp $
*/

#ifndef ldfReaderEventSummaryCommon_H
#define ldfReaderEventSummaryCommon_H

#include "eventSummary.h"

namespace ldfReader {

    class EventSummaryCommon : public EventSummary
    {
    public:

        EventSummaryCommon():EventSummary(0) { m_summary=0; };
        EventSummaryCommon(unsigned summary) : EventSummary(0) {
            m_summary = summary;  };

        EventSummaryCommon(const EventSummaryCommon& summary):EventSummary(0) {
            m_summary = summary.m_summary;
        }

        virtual ~EventSummaryCommon() {}
          
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



    unsigned calStrobe() const { return EventSummary::calStrobe(m_summary);};
    unsigned tag() const { return EventSummary::tag(m_summary); };
    unsigned TACK() const { return EventSummary::TACK(m_summary); };
    unsigned readout4() const { return EventSummary::readout4(m_summary); };
    unsigned zeroSuppress() const { return EventSummary::zeroSuppress(m_summary); };
    unsigned marker() const { return EventSummary::marker(m_summary); };
    unsigned error() const { return EventSummary::error(m_summary); };
    unsigned diagnostic() const { return EventSummary::diagnostic(m_summary); };
    unsigned eventNumber() const { return EventSummary::eventNumber(m_summary); };
    unsigned trgParityError() const { return EventSummary::trgParityError(m_summary); };


    private:
        unsigned m_summary;
    };
}
#endif
