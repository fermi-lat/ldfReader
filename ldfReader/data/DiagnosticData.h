#ifndef ldfReader_DiagnosticData_H
#define ldfReader_DiagnosticData_H

#include "TkrDiagnosticData.h"
#include "CalDiagnosticData.h"
#include <vector>

//#include "EventSummaryCommon.h"

namespace ldfReader {

    /*! @class DiagnosticData
    @brief Storage of Diagnostic EM data (trigger primitives).

    Consists of 2 vectors, one for CAl, the other for TKR
    */
    class DiagnosticData
    {
    public:
        DiagnosticData()  { clear(); }
        DiagnosticData(const DiagnosticData& d) {
            m_tkr = d.m_tkr;
            m_cal = d.m_cal;
            m_lenInBytes = d.m_lenInBytes;
        }
        ~DiagnosticData() {}

        void clear() { 
            m_tkr.clear(); m_cal.clear(); 
            m_lenInBytes = 0; 
            m_exist = false; };

        void print() const {
            if( !exist()) {
                printf("No Diagnostic Data\n");
                return;
            }
            printf("Diagnostic Data:\n");
            printf("Len: %lu\n\n", m_lenInBytes);
        }

        void addCalDiagnostic(const CalDiagnosticData &cal) { m_cal = cal; };
        const CalDiagnosticData& getCalDiagnostic() const{ return m_cal; };

        void addTkrDiagnostic(const TkrDiagnosticData &tkr) { m_tkr = tkr; };
        const TkrDiagnosticData& getTkrDiagnostic() const { return m_tkr; };


       void setExist() { m_exist = true; };
       bool exist() const { return m_exist; };

       void initLength(unsigned long length) { m_lenInBytes = length; };
       unsigned long lenInBytes() const { return m_lenInBytes; };
       
       void initPacketError(unsigned packetError) { m_packetError=packetError; };
       unsigned packetError() const { return m_packetError; };

    private:
        CalDiagnosticData m_cal;
        TkrDiagnosticData m_tkr;

        bool m_exist; 

        unsigned m_packetError;

        unsigned long m_lenInBytes;
    };
} // end namespace
#endif
