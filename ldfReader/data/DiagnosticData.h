#ifndef ldfReader_DiagnosticData_H
#define ldfReader_DiagnosticData_H

#include "TkrDiagnosticData.h"
#include "CalDiagnosticData.h"
#include <vector>

#include "EventSummaryCommon.h"

namespace ldfReader {

    /*! @class DiagnosticData
    @brief Storage of Diagnostic EM data (trigger primitives).

    Consists of 2 vectors, one for CAl, the other for TKR
    */
    class DiagnosticData
    {
    public:
        DiagnosticData()  { clear(); }
        ~DiagnosticData() {}

        void clear() { 
            m_tkrVec.clear(); m_calVec.clear(); 
            m_lenInBytes = 0; 
            m_exist = false; };

        void addCalDiagnostic(const CalDiagnosticData &cal) { m_calVec.push_back(cal); };
        const CalDiagnosticData& getCalDiagnosticByIndex(unsigned int index) const{ return m_calVec[index]; };
        int getNumCalDiagnostic() const { return m_calVec.size(); };

        void addTkrDiagnostic(const TkrDiagnosticData &tkr) { m_tkrVec.push_back(tkr); };
        const TkrDiagnosticData& getTkrDiagnosticByIndex(unsigned int index) const { return m_tkrVec[index]; };
        int getNumTkrDiagnostic() const { return m_tkrVec.size(); };


        const EventSummaryCommon& summary() const { return m_summary; }
        void setSummary(unsigned summary) {m_summary.setSummary(summary); };

       void setExist() { m_exist = true; };
       bool exist() const { return m_exist; };

       void initLength(unsigned long length) { m_lenInBytes = length; };
       unsigned long lenInBytes() const { return m_lenInBytes; };
       
       void initPacketError(unsigned packetError) { m_packetError=packetError; };
       unsigned packetError() const { return m_packetError; };

    private:
        std::vector<CalDiagnosticData> m_calVec;
        std::vector<TkrDiagnosticData> m_tkrVec;

        // Store the event sequence number for this contribution
        EventSummaryCommon m_summary;
        bool m_exist; 

        unsigned m_packetError;

        unsigned long m_lenInBytes;
    };
} // end namespace
#endif
