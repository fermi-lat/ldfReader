#ifndef ldfReader_LATDATA_CXX
#define ldfReader_LATDATA_CXX

/** @file LatData.cxx
@brief Implementation of the LatData class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/data/LatData.cxx,v 1.6 2004/09/23 05:17:40 heather Exp $
*/

#include "ldfReader/data/LatData.h"

namespace ldfReader {

    LatData* LatData::m_instance = 0;


    LatData::LatData() {
        clearTowers(); 
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
        m_osw.clear();
        m_aem.clear();
        m_err.clear();
        m_flags = 0;
    }


    bool LatData::eventSeqConsistent() const {
    //Purpose and Method:  Checks that the event sequence number is consistent
    // accross all contributions.  If not false is returned... 

        unsigned long firstEvtSeq;
        bool foundFirst = false;

        if(getGem().exist()) {
            firstEvtSeq = getGem().summary().eventSequence();
            foundFirst = true;
        }

        if ( (getAem().exist()) && !foundFirst) {
            firstEvtSeq = getAem().summary().eventSequence(); 
            foundFirst = true;
        } else if ( (getAem().exist()) && (firstEvtSeq != getAem().summary().eventSequence()) ) {
            printf("AEM does not match event Seq %d \n", getAem().summary().eventSequence());
            return false;
        }

        if ( (getErr().exist()) && !foundFirst ) {
            firstEvtSeq = getErr().summary().eventSequence(); 
            foundFirst = true;
        } else if ( (getErr().exist()) && (firstEvtSeq != getErr().summary().eventSequence()) ) {
            printf("ERR does not match event Seq %d \n", getErr().summary().eventSequence());
            return false;
        }

        if ( (getOsw().exist()) && !foundFirst ){
            firstEvtSeq = getOsw().summary().eventSequence();
            foundFirst = true;
        } else if ( (getOsw().exist()) && (firstEvtSeq != getOsw().summary().eventSequence()) ){
            printf("OSW does not match event Seq %d \n", getOsw().summary().eventSequence());
            return false;
        }

        if ((diagnostic()->exist()) && !foundFirst ){
            firstEvtSeq = diagnostic()->summary().eventSequence();
            foundFirst = true;
        } else if ((diagnostic()->exist()) && (firstEvtSeq != diagnostic()->summary().eventSequence()) ){
            printf("DIAG does not match event Seq %d \n", getGem().summary().eventSequence());
            return false; 
        }

        std::map<unsigned int, TowerData*>::const_iterator towerIter = m_towerMap.begin();
        while(towerIter != m_towerMap.end())
        {
            TowerData* tower = (towerIter++)->second;
            const TemData tem = tower->getTem();
            if ( (tem.exist()) && !foundFirst) {
                firstEvtSeq = tem.summary().eventSequence();
                foundFirst = true;
            } else if ( (tem.exist()) && (firstEvtSeq != tem.summary().eventSequence())) {
                printf("TEM does not match event Seq %d \n", tem.summary().eventSequence());
                return false;
            }
        }
        // all ok
        return true;
    }


}

#endif
