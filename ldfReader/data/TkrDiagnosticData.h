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

        TkrDiagnosticData(unsigned datum, unsigned tower, unsigned gtcc) : m_datum(datum),m_tower(tower),m_gtcc(gtcc) {}

        TkrDiagnosticData()  {}
        ~TkrDiagnosticData() {}

        void init(unsigned data, unsigned tower, unsigned gtcc) { 
             m_datum = data; m_tower=tower; m_gtcc=gtcc; };

        unsigned dataWord() const { return m_datum;};
        unsigned tower() const { return m_tower; };
        unsigned gtcc() const { return m_gtcc; };

    private:
        unsigned m_datum;
        unsigned m_tower;
        unsigned m_gtcc;
    };
} // end namespace
#endif
