#ifndef ldfReader_LATDATA_CXX
#define ldfReader_LATDATA_CXX

/** @file LatData.cxx
@brief Implementation of the LatData class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/data/LatData.cxx,v 1.44 2008/11/11 04:28:55 heather Exp $
*/

#include "ldfReader/data/LatData.h"
#include "../EbfDebug.h"
#include "facilities/Util.h"
#include <iostream>
#include <fstream>

namespace ldfReader {

    LatData* LatData::m_instance = 0;


    LatData::LatData() {
        clearTowers(); 
        m_acdRemapCol.clear();
        m_oldStyleRunId = false;
    }

    LatData::~LatData() {
        clearTowers();
        std::map<const std::string*, const std::string*>::const_iterator acdMapIt;
        for (acdMapIt = m_acdRemapCol.begin(); acdMapIt != m_acdRemapCol.end();
                                               acdMapIt++) {
            delete acdMapIt->first;
            delete acdMapIt->second;
        }
        m_acdRemapCol.clear();
    }

    LatData* LatData::instance() {
        if (m_instance == 0) {
            m_instance = new LatData();
        }
        return m_instance;
    }

    void LatData::print() const {
      const lsfData::RunInfo run = m_metaEvent.run();
      const lsfData::DatagramInfo dgm = m_metaEvent.datagram();
      const lsfData::GemScalers scalers = m_metaEvent.scalers();
      const lsfData::Time time = m_metaEvent.time();

      std::cout << "==========================================" 
                << std::endl << std::endl << "Event " 
                << scalers.sequence() << " context:" << std::endl;

      std::cout << time << scalers << run << dgm << std::endl
                << "Event " << scalers.sequence() << " info:" << std::endl
                << "---------------" << std::endl
                << "LSE_Info:  timeTicks = 0x" << std::uppercase << std::hex 
                << std::setfill('0') << std::setw(8) 
                << time.timeTicks() << " (" << std::dec 
                << time.timeTicks()
                << ")\nLSE_Info:  " << time.timeHack() << std::endl;

      const lsfData::Configuration *cfg = m_metaEvent.configuration();
      if (cfg) 
          cfg->print();

      m_ccsds.print();

  }


    TowerData* LatData::getTower(unsigned int id) {
        if (m_towerMap.find(id) != m_towerMap.end()) {
            return m_towerMap[id];
        }
        return 0;
    }

    AcdDigi* LatData::getAcd(const char *name) {
        const std::map<const char*, ldfReader::AcdDigi*> acdCol = getAcdCol();
        std::map<const char*, ldfReader::AcdDigi*>::const_iterator thisAcdDigi;

        std::string str(name);
        for (thisAcdDigi = acdCol.begin(); 
                       thisAcdDigi != acdCol.end(); 
                       thisAcdDigi++) {

            if (str.compare(thisAcdDigi->first) == 0)
                return (thisAcdDigi->second);
        }
        //if (m_acdCol.find(name) != m_acdCol.end()) {
       //     return m_acdCol[name];
       // }
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
        m_gem.clear();
        m_osw.clear();
        m_aem.clear();
        m_flags = 0;
        m_eventSize = 0;
        m_metaEvent.clear();
        m_ccsds.clear();
        m_eventId = 0;
        m_adfHdrTlr = false;
        m_adf.clear();
        m_contextExists = false;
        m_ignoreSegFault = false;
    }


    bool LatData::eventSeqConsistent() const {
    //Purpose and Method:  Checks that the event sequence number is consistent
    // accross all contributions.  If not false is returned... 
    // Assumes m_eventId for any errors messages

        unsigned long firstEvtSeq=0;
        bool foundFirst = false;

        if(getGem().exist()) {
            firstEvtSeq = getGem().summary().eventSequence();
            foundFirst = true;
        }


        if ( (getAem().exist()) && !foundFirst) {
            firstEvtSeq = getAem().summary().eventSequence(); 
            foundFirst = true;
        } else if ( (getAem().exist()) && (firstEvtSeq != getAem().summary().eventSequence()) ) {
            std::cout << "AEM does not match event Seq " << firstEvtSeq << " "
                      << getAem().summary().eventSequence() << std::endl;
            return false;
        }


        if ( (getOsw().exist()) && !foundFirst ){
            firstEvtSeq = getOsw().summary().eventSequence();
            foundFirst = true;
        } else if ( (getOsw().exist()) && (firstEvtSeq != getOsw().summary().eventSequence()) ){
            std::cout << "OSW does not match event Seq " << firstEvtSeq << " "
                      <<  getOsw().summary().eventSequence() << std::endl;
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
                std::cout << "TEM does not match event Seq " << firstEvtSeq 
                          << " " <<  tem.summary().eventSequence() << std::endl;
                return false;
            }
        }


        // Assuming ADF will not be the only or first contribution found
        // ADF event number will be one more than EBF contributions
        // See Ric's email Aug 1, 2006
        if ( (getAdf().exist()) && (firstEvtSeq != (getAdf().evtNum()+1)) ) {
            std::cout << "ADF does not match event Seq " << firstEvtSeq << " "
                      << (getAdf().evtNum()+1) << std::endl;
            return false;
        }

        // all ok
        return true;
    }


    unsigned LatData::checkPacketError() {
        // Returns true if OR of all is TRUE
        // Returns false if OR of all packetError words is FALSE
        // Assumes m_eventId has been set for any error messages

        unsigned orAll = 0;

        if(getGem().exist()) {
            orAll |= getGem().packetError();
        }

        if (getAem().exist()) {
            orAll |= getAem().packetError();
         }

        if ( getOsw().exist()) {
            orAll |= getOsw().packetError();
         }

        std::map<unsigned int, TowerData*>::const_iterator towerIter = m_towerMap.begin();
        while(towerIter != m_towerMap.end())
        {
            TowerData* tower = (towerIter++)->second;
            const TemData tem = tower->getTem();
            if (tem.exist()){
                orAll |= tem.packetError();
            }
        }
        if (orAll != 0) {
            std::cout << "Setting Packet Error Flag, Event: " 
                      << m_eventId << " Apid: "
                      << getCcsds().getApid() << std::endl;
            setPacketErrorFlag();
        }
        return (orAll);
    }

    unsigned LatData::checkTemError() {
        // Assumes m_eventId has been set for any error messages
        // Also checks the diagnostic bit across all TEMs

        unsigned orAll = 0;
        unsigned int temOrAll = 0;
        unsigned int diagOrAll = 0;

        std::map<unsigned int, TowerData*>::const_iterator towerIter = m_towerMap.begin();
        while(towerIter != m_towerMap.end())
        {
            TowerData* tower = (towerIter++)->second;
            const TemData tem = tower->getTem();
            if (tem.exist()) {
                temOrAll |= tem.summary().error();
                //tem.summary().print();
                //printf("tem error %d\n", tem.summary().error());
                orAll |= tem.summary().error();
                diagOrAll |= tem.summary().diagnostic();
                //printf("orAll = %d\n", orAll);
            }
        }

        if (getAem().exist()) 
            diagOrAll |= getAem().summary().diagnostic();

        if (diagOrAll != 0) 
            setDiagnostic();
 
        if ( getOsw().exist()) {
            orAll |= getOsw().summary().error();
            if (temOrAll != getOsw().summary().error()) 
                std::cout << "OSW error summary bit does not match OR of all " 
                          << "error bits across all TEM contributions, "
                          << "event Num: " << m_eventId  
                          << " Apid: " << getCcsds().getApid() << std::endl;
        }
        if (orAll != 0) {
            std::cout << "Event Summary Error Flag Set, Event:  "
                      <<  m_eventId << " Apid: " 
                      << getCcsds().getApid() << std::endl;
            setTemErrorFlag();
            setError();
        }
        return (orAll);
    }


    unsigned LatData::checkTrgParityError() {
        // Assumes m_eventId has been set for any error messages

        unsigned orAll = 0;

        if(getGem().exist()) 
            orAll |= getGem().summary().trgParityError();

        if (getAem().exist()) 
            orAll |= getAem().summary().trgParityError();

        unsigned int temOrAll = 0;
        std::map<unsigned int, TowerData*>::const_iterator towerIter = m_towerMap.begin();
        while(towerIter != m_towerMap.end())
        {
            TowerData* tower = (towerIter++)->second;
            const TemData tem = tower->getTem();
            if (tem.exist()) {
                temOrAll |= tem.summary().trgParityError();
                //tem.summary().print();
                //printf("tem error %d\n", tem.summary().trgParityError());
                orAll |= tem.summary().trgParityError();
                //printf("orAll = %d\n", orAll);
            }
        }
        if ( getOsw().exist()) {
            orAll |= getOsw().summary().trgParityError();
            if (temOrAll != getOsw().summary().trgParityError()) 
                std::cout << "OSW error summary bit does not match OR of all "
                          << "error bits across all TEM contributions, "
                          << getOsw().summary().trgParityError() 
                          << " Event: "
                          <<  m_eventId << " Apid: " 
                          << getCcsds().getApid() << std::endl;
        }
        if (orAll != 0) {
            std::cout << "Trg Parity Error Flag Set, Event: " 
                      <<  m_eventId << " Apid: " 
                      << getCcsds().getApid() << std::endl;
            setTrgParityErrorFlag();
        }
        return (orAll);
    }

bool LatData::checkAemError() {

    bool err = false;
    const std::map<const char*, ldfReader::AcdDigi*> acdCol = getAcdCol();
    std::map<const char*, ldfReader::AcdDigi*>::const_iterator thisAcdDigi;

    for (thisAcdDigi = acdCol.begin(); 
                       thisAcdDigi != acdCol.end(); 
                       thisAcdDigi++) {
        const char *tileName = thisAcdDigi->second->getTileName();

        const std::vector<ldfReader::AcdDigi::AcdPmt> readoutCol = thisAcdDigi->
second->getReadout();
        std::vector<ldfReader::AcdDigi::AcdPmt>::const_iterator curReadout;
        for (curReadout = readoutCol.begin(); curReadout != readoutCol.end();
             ++curReadout) {

        if (curReadout->getParityError() == AcdDigi::ERROR) {
            std::cout << "AEM Parity Error tile " << tileName << " Side: "
                      << curReadout->getSide()
                      << " Event: "
                      <<  m_eventId << " Apid: " 
                      << getCcsds().getApid() << std::endl;
            err = true;
        }
  
    
        if (curReadout->getHeaderParity() == AcdDigi::ERROR) {
            std::cout << "AEM Header Parity Error tile " << tileName
                      << " Side: " << curReadout->getSide()
                      << " Event: "
                      <<  m_eventId << " Apid: " 
                      << getCcsds().getApid() << std::endl;
            err = true;
        }
      }

   }
   return err;
}


unsigned LatData::checkCalReadout() {
    // Returns true if OR of all is TRUE ..all is ok
    // Returns false if the incorrect number of readouts is available for 
    // ALLRANGE 
    // Assumes m_eventId has been set for any error messages

    unsigned orAll = 0;

    std::map<unsigned int, TowerData*>::const_iterator towerIter = 
                                                          m_towerMap.begin();
    while(towerIter != m_towerMap.end())
    {
        TowerData* tower = (towerIter++)->second;

        const std::map<unsigned int, CalDigi*>& calCol = tower->getCalDigiCol(); 
        std::map<unsigned int, CalDigi*>::const_iterator calIt = calCol.begin();
        while (calIt != calCol.end()) {
            CalDigi *cal = (calIt++)->second;
            if (cal->getMode() != CalDigi::ALLRANGE) continue;
            if (cal->getNumReadouts() != 4) ++orAll;
        }

    }
    if (orAll != 0) {
        std::cout << "Setting Missing Cal Readout Flag, "
                  << orAll << " CalDigis are missing readouts for Event: " 
                  << m_eventId << " Apid: "
                  << getCcsds().getApid() << std::endl;
        setCalReadoutFlag();
    }
    return (orAll);
}

int LatData::setAcdRemap(const std::string& filename) {
    if (filename == "") {
        m_acdRemapCol.clear();
        return -1;
    }
    std::string fileNameStr = filename.c_str();
    facilities::Util::expandEnvVar(&fileNameStr);
    std::ifstream infile(fileNameStr.c_str());
    if (!infile.good()) return -1;
    std::string fromStr, toStr;
    while (infile.good()) {
        infile >> fromStr;
        infile >> toStr;
        //char *fromChar = new char[fromStr.length()+1];
        //char *toChar = new char[toStr.length()+1];
        //fromChar = fromStr.c_str();
        //toChar = toStr.c_str();
        //m_acdRemapCol[fromChar] = toChar;
        //m_acdRemapCol[fromStr.c_str()] = toStr.c_str();
        m_acdRemapCol[new std::string(fromStr)] = new std::string(toStr);
        if (EbfDebug::getDebug())
            std::cout <<" Mapping " << fromStr << " to " << toStr << std::endl;
    }
    if (EbfDebug::getDebug()) {
        //std::map<const char*, const char*>::const_iterator acdMapIt;
        std::map<const std::string*, const std::string*>::const_iterator acdMapIt;
        std::cout << "Map Contains:" << std::endl;
        for (acdMapIt = m_acdRemapCol.begin(); acdMapIt != m_acdRemapCol.end(); acdMapIt++) 
            std::cout << *(acdMapIt->first) << " " << *(acdMapIt->second) << std::endl;
    }

    return 0; 
}


}  // end namespace


#endif
