#ifndef ldfReader_TkrDiagnosticData_H
#define ldfReader_TkrDiagnosticData_H

#include "TKRdiagnostic.h"

namespace ldfReader {

    /* @class TkrDiagnosticData
    @brief Stores TKR trigger primitives
    */
    class TkrDiagnosticData
    {
    public:

        TkrDiagnosticData(unsigned datum) : m_datum(datum) {}

        TkrDiagnosticData()  {}
        ~TkrDiagnosticData() {}

        void init(unsigned data) { m_datum = data; };

        unsigned dataWord() const { return m_datum;};

    private:
        unsigned m_datum;
    };
} // end namespace
#endif
