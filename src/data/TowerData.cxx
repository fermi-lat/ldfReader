#ifndef ldfReader_TOWERDATA_CXX
#define ldfReader_TOWERDATA_CXX

/** @file TowerData.cxx
@brief Implementation of the TowerData class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/data/TowerData.cxx,v 1.1.1.1 2004/04/15 20:02:22 heather Exp $
*/


#include "ldfReader/data/TowerData.h"

namespace ldfReader {

    TkrLayer* TowerData::getTkrLayer(unsigned int id) {
        if (m_tkrMap.find(id) != m_tkrMap.end()) {
            return m_tkrMap[id];
        }
        return 0;
    }

    CalDigi* TowerData::getCalDigi(unsigned int layer, unsigned col) {
        unsigned int translatedId = layer * 100 + col;
        if (m_calDigiCol.find(translatedId) != m_calDigiCol.end()) {
            return m_calDigiCol[translatedId];
        }
        return 0;
    }

    void TowerData::clear() { 
        std::map<unsigned int, TkrLayer*>::iterator tkrIter = m_tkrMap.begin();
        while(tkrIter != m_tkrMap.end())
        {
            TkrLayer* tkr = (tkrIter++)->second;
            delete tkr;
        }

        m_tkrMap.clear();

        std::map<unsigned int, CalDigi*>::iterator calIter = m_calDigiCol.begin();
        while(calIter != m_calDigiCol.end())
        {
            CalDigi* cal = (calIter++)->second;
            delete cal;
        }

        m_calDigiCol.clear();
    }

    void TowerData::print() const {
        m_tem.print();
    }

} // end namespace

#endif
