#ifndef ldfReader_DfiParser_CXX
#define ldfReader_DfiParser_CXX


/** @file DfiParser.cxx
@brief Implementation of the DfiParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/DfiParser.cxx,v 1.27 2007/04/11 21:38:25 borgland Exp $
*/

#include "ldfReader/DfiParser.h"
#include "iterators/EbfEventParser.h"
#include "EbfDebug.h"
#include "ldfReader/LdfException.h"
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <math.h> // for floor

#include "facilities/Timestamp.h"
#include "astro/JulianDate.h"

#include "lsfData/LsfTime.h"
//#include "./ccsds2lsf.h"

namespace ldfReader {

DfiParser::DfiParser() {
    clear();
}


DfiParser::DfiParser(const std::string &filename) {
    //Purpose and Method:  Ctor using LPA file as input 
    // Open input file and read in first event
    //Inputs:  filename 
    try {
       clear();
       m_file = NULL;
       m_file = new eventFile::LSFReader(filename);
       std::cout << "Created eventFile" << std::endl;
       m_runId = m_file->runid();
       m_more = m_file->read(m_ccsds, m_meta, m_ebf);
       std::cout<< "After first read " << std::endl;
       if (!m_more) {
           std::cout << "No events in input file" << std::endl;
           throw;
       }

       m_start = const_cast<EBFevent*>(m_ebf.start());
       m_end = const_cast<EBFevent*>(m_ebf.end());
    } catch(std::runtime_error e ) {
        std::cerr << e.what() << std::endl;
        throw(e);
    } catch (...) {
        std::cerr << "Unknown Exception caught while creating DfiParser" 
                  << std::endl;
        throw;
    }

}


DfiParser::~DfiParser() {
    if (m_file) delete m_file;
    m_file = 0;
}

void DfiParser::clear() {
    m_runId = 0;
    m_eventId = 0;
    m_eventSize = 0;
    m_ccsds.clear();
    m_meta.clear();
}

void DfiParser::printHeader() const {
    // Purpose and Method: print the header summary information

    printf( "retrieved %llu events for run %09u\n", 
                                m_file->evtcnt(), m_file->runid());
    printf( "first Spacecraft clock value = %u\n", m_file->begSec() );
    printf( "last  Spacecraft clock value = %u\n", m_file->endSec() );
    printf( "first GEM sequence counter value = %llu\n", m_file->begGEM() );
    printf( "last  GEM sequence counter value = %llu\n", m_file->endGEM() );
    for ( int i=0; i<LSEHEADER_MAX_APIDS; i++ ) {
        std::pair<unsigned, unsigned> err = m_file->seqErr( i );
        if ( err.first > 0 ) {
            printf( "apid %04u had %10u sequence errors\n", 
                                                 err.first, err.second );
        }
    }
    for ( int i=0; i<LSEHEADER_MAX_APIDS; i++ ) {
        std::pair<unsigned, unsigned> err = m_file->dfiErr( i );
        if ( err.first > 0 ) {
            printf( "apid %04u had %10u DFI errors\n", err.first, err.second );
        }
    }
    printf( "\n" );
}

int DfiParser::nextEvent() {
    try {
        m_ccsds.clear();
        m_meta.clear();
        bool m_more = m_file->read(m_ccsds, m_meta, m_ebf);
        if (m_more == false) return -1;

        m_start = const_cast<EBFevent*>(m_ebf.start());
        m_end = const_cast<EBFevent*>(m_ebf.end());

        m_eventSize = m_ebf.size();

        return 0;
    } catch(LdfException &e) {

        std::cerr << "LdfException caught: " << e.what() << std::endl;
        throw;
    } catch(std::runtime_error e ) {
        std::cerr << e.what() << std::endl;
        throw(e);
    } catch(...) {

        std::cerr << "Unknown Exception caught in DfiParser::nextEvent" 
                  << std::endl;
        throw;
    }

}

int DfiParser::readContextAndInfo() {

    ldfReader::LatData::instance()->setContextExists(true);
    lsfData::MetaEvent *metaEvent = ldfReader::LatData::instance()->getMetaEventPtr();

    metaEvent->set(m_meta.run(), m_meta.datagram(), m_meta.scalers(),
                   m_meta.time(), *(m_meta.configuration()), *(m_meta.keys()));
 
    lsfData::LsfCcsds* ccsdsData = ldfReader::LatData::instance()->getCcsdsPtr();
    ccsdsData->initialize(m_ccsds.getScid(), m_ccsds.getApid(), m_ccsds.getUtc());

    // Do this after reading LDF data
    //ldfReader::LatData::instance()->setTimeInSecTds(timeForTds(m_ccsds.getUtc()));
    ldfReader::LatData::instance()->setEventId(m_meta.scalers().sequence());
 
    if (EbfDebug::getDebug()) ccsdsData->print();
    return 0;
}


double DfiParser::timeForTds(double utc) {

// Code from Anders July, 2006. With many updates :-)
// Right now the missing GPS lock is always set so can't use 
// "incomplete" Time Tone flag! Need to check each flag explicitly - 
// can't rely on correct redundancy behaviour right now.
// After integration with the space craft in December 2006 we no longer have 
// the missing GPS lock error. However, we now occasionally have an incomplete 
// time tone without any other error flags set.
//


    // Time stamp of the event in seconds (and fractions thereof) since 01.01.2001 UTC:
    double timestamp;

    // LAT nominal system clock: 20MHz
    double LATSystemClock = 20000000.0;

    // Roll over offset for the 25 bit GEM counter: 2**25
    double RollOver = 33554432.0;

    // Get it:
    const lsfData::MetaEvent& metaEvent = ldfReader::LatData::instance()->getMetaEvent();

    // Number of ticks between the current event and the current 1-PPS:
    double clockTicksEvt1PPS = double (metaEvent.time().timeTicks()) - 
                               double (metaEvent.time().current().timeHack().ticks());

    // Rollover? 
    if (clockTicksEvt1PPS < 0) {
      clockTicksEvt1PPS = clockTicksEvt1PPS + RollOver;
    }

    // Verify that the two TimeTones are OK:
    if (!(metaEvent.time().current().incomplete()) &&
        !(metaEvent.time().current().flywheeling()) &&
        !(metaEvent.time().current().missingCpuPps()) &&
        !(metaEvent.time().current().missingLatPps()) &&
        !(metaEvent.time().current().missingTimeTone()) &&
        !(metaEvent.time().previous().incomplete()) &&
        !(metaEvent.time().previous().flywheeling()) &&
        !(metaEvent.time().previous().missingCpuPps()) &&
        !(metaEvent.time().previous().missingLatPps()) &&
        !(metaEvent.time().previous().missingTimeTone()) &&
	// Avoid 1/0 error:
        (metaEvent.time().current().timeHack().ticks() != 
                      metaEvent.time().previous().timeHack().ticks()) &&  
        // If there is more than a second between 1-PPS I can 
	// only use the nominal value for the LAT clock anyway!
        ((metaEvent.time().current().timeSecs() -
                      metaEvent.time().previous().timeSecs()) == 1)) {

      // Then use full formula for correcting system clock drift 
      // using the last two TimeTones i.e. extrapolation!

      // Number of ticks between the current and the previous time tone:
      double clockTicksDelta1PPS = double (metaEvent.time().current().timeHack().ticks())
                                 - double (metaEvent.time().previous().timeHack().ticks());

      // Rollover? 
      if (clockTicksDelta1PPS < 0) {
        clockTicksDelta1PPS = clockTicksDelta1PPS + RollOver;
      }

      // Timestamp:
      timestamp = double (metaEvent.time().current().timeSecs()) 
                  + (clockTicksEvt1PPS/clockTicksDelta1PPS);
    } else {

      // Cannot use TimeTone(s) - will assume nominal value for the LAT system clock:
      timestamp = double (metaEvent.time().current().timeSecs())
                  + (clockTicksEvt1PPS/LATSystemClock);
    }

    // Event time stamp:
    return timestamp;
}


int DfiParser::loadData() {
// Purpose and Method:  This routine loads the data from one event
// The current event in the EBF file.


    try {
        // First clear the LatData
        ldfReader::LatData::instance()->clearTowers();

        ldfReader::LatData::instance()->setRunId(m_runId);

        ldfReader::LatData::instance()->setEventSizeInBytes(m_eventSize);

        readContextAndInfo();


        EbfEventParser ldf;
        ldf.iterate(m_start, m_end);

        unsigned long long eventId = ldfReader::LatData::instance()->eventId();
        int apid = ldfReader::LatData::instance()->getCcsds().getApid();

        if (EbfDebug::getDebug()) 
            std::cout << "Event: " << eventId << " APID: " << apid << std::endl;
        
        ldfReader::LatData::instance()->setTimeInSecTds(timeForTds(ldfReader::LatData::instance()->getCcsds().getUtc()));

        if (ldf.status()) {
            std::ostringstream errMsg;
            errMsg.str("LDF EBFeventParser reported a bad status 0x");
            errMsg << std::hex << ldf.status() << " = " << std::dec 
                   << ldf.status() << " Event: " << eventId 
                   << " APID: " << apid;
            std::cout << errMsg << std::endl;
            ldfReader::LatData::instance()->setBadLdfStatusFlag();
            //throw LdfException(errMsg.str());
        }
   

        if (!ldfReader::LatData::instance()->eventSeqConsistent()) {
            printf("Event Sequence numbers are not consistent within");
            printf(" all contributions\n");
            printf("Setting event flag for event: %llu apid: %u\n",
                eventId, apid);
            ldfReader::LatData::instance()->setBadEventSeqFlag();
            return 0;
         }


        ldfReader::LatData::instance()->checkTemError();
        ldfReader::LatData::instance()->checkPacketError();
        ldfReader::LatData::instance()->checkTrgParityError();
        ldfReader::LatData::instance()->checkAemError();
        ldfReader::LatData::instance()->checkCalReadout();

    } catch (LdfException& e) {
       std::cerr << "Caught LdfException: " << e.what() << " Apid: "  
                 << ldfReader::LatData::instance()->getCcsds().getApid()  
                 << std::endl;
       throw;
    } catch(std::runtime_error e ) {
        std::cerr << e.what() << " Apid: " 
                  << ldfReader::LatData::instance()->getCcsds().getApid()  
                  << std::endl;
        throw(e);
    } catch(...) {
       std::cerr << "Caught Exception Apid:" 
                 << ldfReader::LatData::instance()->getCcsds().getApid()  
                 << std::endl;
       throw;
    }
    return 0;
}

//bool DfiParser::setDebug(bool on) {
//    return EbfDebug::setDebug(on);
//}




}
#endif
