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
        CalDiagnosticData(unsigned datum, unsigned tower, unsigned layer) 
            : m_datum(datum),m_tower(tower),m_layer(layer) {}

        CalDiagnosticData()  {clear();}
        CalDiagnosticData(const CalDiagnosticData &c) { 
            m_datum = c.m_datum;
            m_tower = c.m_tower;
            m_layer = c.m_layer;
        }

        ~CalDiagnosticData() {}
 
        void clear() {
            m_datum = 0;
            m_tower = 0;
            m_layer = 0;  
        }
 
        void init(unsigned data, unsigned tower,unsigned layer) { 
             m_datum = data; 
             m_tower = tower; 
             m_layer = layer; };

        unsigned dataWord() const { return m_datum; };
        unsigned tower() const { return m_tower; };
        unsigned layer() const { return m_layer; };

    private:
        unsigned m_datum;
        unsigned m_tower;
        unsigned m_layer;
    };
} // end namespace
#endif
