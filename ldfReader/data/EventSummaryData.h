/* @class EventSummaryData
* @brief Class for accessing fields in the event summary longword.
* $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/EventSummaryData.h,v 1.2 2004/07/29 22:00:50 heather Exp $
*/

#ifndef ldfReaderEventSummaryData_H
#define ldfReaderEventSummaryData_H

#include "eventSummary.h"

namespace ldfReader {

    class EventSummaryData
    {
    public:
        EventSummaryData(unsigned summary, long timeSec=0, long timeNano=0){ 
            m_summary=summary;
            m_timeSec = timeSec;
            m_timeNanoSec = timeNano;
        }

        EventSummaryData() {m_summary =0; m_timeSec = 0; m_timeNanoSec = 0;
        m_upperPpcTimeBase = 0; m_lowerPpcTimeBase = 0;}
        ~EventSummaryData() {}

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
        void setTime(long sec, long nanosec) { m_timeSec = sec; m_timeNanoSec = nanosec; };
        void setPpcTimeBase(unsigned upperWord, unsigned lowerWord) { 
            m_upperPpcTimeBase = upperWord; m_lowerPpcTimeBase = lowerWord; 
        };

        long timeSec() { return m_timeSec; }
        long timeNanoSec() { return m_timeNanoSec; }
        unsigned summary() {return m_summary;}
        unsigned upperPpcTimeBaseWord() { return m_upperPpcTimeBase; };
        unsigned lowerPpcTimeBaseWord() { return m_lowerPpcTimeBase; };
        long timeSec() const { return m_timeSec; }
        long timeNanoSec() const {return m_timeNanoSec; }
        unsigned summary() const {return m_summary;}
        unsigned upperPpcTimeBaseWord() const { return m_upperPpcTimeBase; };
        unsigned lowerPpcTimeBaseWord() const { return m_lowerPpcTimeBase; };

        /// returning a signed long - makes it easier to do check for 
        /// monotonically increasing sequence numbers and the rollover
        /// value is less than 140000, so sign shouldn't matter
        long eventSequence() const {
            unsigned eventNumber = EventSummary::eventNumber(m_summary);
            unsigned tag = EventSummary::tag(m_summary);
            return ((eventNumber << 2) | tag);
        }

    private:
        unsigned m_summary;
        long m_timeSec;
        long m_timeNanoSec;
 
        /// These two members form a 64 bit word that represents a "clock cycle number" from
        /// the PPC registers - this value divided by 16MHz is approximately the time since 
        /// the VxWorks box was turned on
        unsigned m_upperPpcTimeBase;
        unsigned m_lowerPpcTimeBase;
    };
}
#endif
