#ifndef ldfReader_LATDATA_CXX
#define ldfReader_LATDATA_CXX

/** @file LatData.cxx
@brief Implementation of the LatData class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/data/LatData.cxx,v 1.3 2004/07/29 22:01:33 heather Exp $
*/

#include "ldfReader/data/LatData.h"

namespace ldfReader {

    LatData* LatData::m_instance = 0;


    LatData::LatData() {
        clearTowers(); 
        m_diagnostic.clear();
    }

    LatData* LatData::instance() {
        if (m_instance == 0) {
            m_instance = new LatData();
        }
        return m_instance;
    }


    TowerData* LatData::getTower(unsigned int id) {
        if (m_towerMap.find(id) != m_towerMap.end()) {
            return m_towerMap[id];
        }
        return 0;
    }

    AcdDigi* LatData::getAcd(const char *name) {
        if (m_acdCol.find(name) != m_acdCol.end()) {
            return m_acdCol[name];
        }
        return 0;
    }

//   const AcdDigi* LatData::getAcd(const char *name) const {
//        if (m_acdCol.find(name) != m_acdCol.end()) {
//            return m_acdCol[name];
//        }
//        return 0;
//    }

    void LatData::clearTowers()
    {
        std::map<unsigned int, TowerData*>::iterator towerIter = m_towerMap.begin();
        while(towerIter != m_towerMap.end())
        {
            TowerData* tower = (towerIter++)->second;
            delete tower;
        }
        m_towerMap.clear();

        std::map<const char *, AcdDigi*>::iterator acdIter = m_acdCol.begin();
        while(acdIter != m_acdCol.end()) {
            AcdDigi *digi = (acdIter++)->second;
            delete digi;
        }
        m_acdCol.clear();
        m_diagnostic.clear();
        m_gem.clear();
        m_flags = 0;
    }


    bool LatData::eventSeqConsistent() const {

        unsigned long firstEvtSeq = getOsw().summary().eventSequence();
        if (firstEvtSeq != getAem().summary().eventSequence()) return false;
        if (firstEvtSeq != getErr().summary().eventSequence()) return false;
        if (firstEvtSeq != getGem().summary().eventSequence()) return false;
        if (firstEvtSeq != diagnostic()->summary().eventSequence()) return false;
        std::map<unsigned int, TowerData*>::const_iterator towerIter = m_towerMap.begin();
        while(towerIter != m_towerMap.end())
        {
            TowerData* tower = (towerIter++)->second;
            const TemData tem = tower->getTem();
            if (firstEvtSeq != tem.summary().eventSequence()) return false;
        }
        // all ok
        return true;
    }


}

#endif
