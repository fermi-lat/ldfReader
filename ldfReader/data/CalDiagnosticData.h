#ifndef ldfReader_CalDiagnosticData_H
#define ldfReader_CalDiagnosticData_H

#include "CALdiagnostic.h"

namespace ldfReader {

   /* @class CalDiagnosticData
    *  @brief Stores trigger primitives for CAL
    */
    class CalDiagnosticData
    {
    public:
        CalDiagnosticData(unsigned datum) 
            : m_datum(datum) {}

            CalDiagnosticData()  {}
            ~CalDiagnosticData() {}

            void init(unsigned data) { m_datum = data; };

            unsigned dataWord() const { return m_datum; };

    private:
        unsigned m_datum;
    };
} // end namespace
#endif
