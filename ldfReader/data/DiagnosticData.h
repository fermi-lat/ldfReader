#ifndef ldfReader_DiagnosticData_H
#define ldfReader_DiagnosticData_H

#include "TkrDiagnosticData.h"
#include "CalDiagnosticData.h"
#include <vector>

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

        void clear() { m_tkrVec.clear(); m_calVec.clear(); };

        void addCalDiagnostic(const CalDiagnosticData &cal) { m_calVec.push_back(cal); };
        const CalDiagnosticData& getCalDiagnosticByIndex(unsigned int index) const{ return m_calVec[index]; };
        int getNumCalDiagnostic() const { return m_calVec.size(); };

        void addTkrDiagnostic(const TkrDiagnosticData &tkr) { m_tkrVec.push_back(tkr); };
        const TkrDiagnosticData& getTkrDiagnosticByIndex(unsigned int index) const { return m_tkrVec[index]; };
        int getNumTkrDiagnostic() const { return m_tkrVec.size(); };

    private:
        std::vector<CalDiagnosticData> m_calVec;
        std::vector<TkrDiagnosticData> m_tkrVec;
    };
} // end namespace
#endif
