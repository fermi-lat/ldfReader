/* @class EventSummaryData
* @brief Class for accessing fields in the event summary longword.
* $Header$
*/

#ifndef ldfReaderEventSummaryData_H
#define ldfReaderEventSummaryData_H

#include "eventSummary.h"

namespace ldfReader {

    class EventSummaryData
    {
    public:
        EventSummaryData(unsigned summary, unsigned timeSec=0, unsigned timeNano=0){ 
            m_summary=summary;
            m_timeSec = timeSec;
            m_timeNanoSec = timeNano;
        }
        EventSummaryData() {m_summary =0; m_timeSec = 0; m_timeNanoSec = 0;
        m_upperPpcTimeBase = 0; m_lowerPpcTimeBase = 0;}
        ~EventSummaryData() {}

        void setSummary(unsigned summary) { m_summary = summary; }
        void setTime(unsigned sec, unsigned nanosec) { m_timeSec = sec; m_timeNanoSec = nanosec; };
        void setPpcTimeBase(unsigned upperWord, unsigned lowerWord) { 
            m_upperPpcTimeBase = upperWord; m_lowerPpcTimeBase = lowerWord; 
        };

        unsigned timeSec() { return m_timeSec; }
        unsigned timeNanoSec() {return m_timeNanoSec; }
        unsigned summary() {return m_summary;}
        unsigned upperPpcTimeBaseWord() { return m_upperPpcTimeBase; };
        unsigned lowerPpcTimeBaseWord() { return m_lowerPpcTimeBase; };
        unsigned timeSec() const { return m_timeSec; }
        unsigned timeNanoSec() const {return m_timeNanoSec; }
        unsigned summary() const {return m_summary;}
        unsigned upperPpcTimeBaseWord() const { return m_upperPpcTimeBase; };
        unsigned lowerPpcTimeBaseWord() const { return m_lowerPpcTimeBase; };

    private:
        unsigned m_summary;
        unsigned m_timeSec;
        unsigned m_timeNanoSec;

        /// These two members form a 64 bit word that represents a "clock cycle number" from
        /// the PPC registers - this value divided by 16MHz is approximately the time since 
        /// the VxWorks box was turned on
        unsigned m_upperPpcTimeBase;
        unsigned m_lowerPpcTimeBase;
    };
}
#endif
