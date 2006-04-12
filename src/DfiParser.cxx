#ifndef ldfReader_DfiParser_CXX
#define ldfReader_DfiParser_CXX


/** @file DfiParser.cxx
@brief Implementation of the DfiParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/DfiParser.cxx,v 1.14 2006/04/12 07:19:54 heather Exp $
*/

#include "ldfReader/DfiParser.h"
#include "iterators/EbfEventParser.h"
#include "EbfDebug.h"
#include "ldfReader/LdfException.h"
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>

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

    lsfData::MetaEvent *metaEvent = ldfReader::LatData::instance()->getMetaEventPtr();

    metaEvent->set(m_meta.run(), m_meta.datagram(), m_meta.scalers(),
                   m_meta.time(), *(m_meta.configuration()));
 
    lsfData::LsfCcsds* ccsdsData = ldfReader::LatData::instance()->getCcsdsPtr();
    ccsdsData->initialize(m_ccsds.getScid(), m_ccsds.getApid(), m_ccsds.getUtc());

    ldfReader::LatData::instance()->setTimeInSecTds(timeForTds(m_ccsds.getUtc()));
    ldfReader::LatData::instance()->setEventId(m_meta.scalers().sequence());
 
    if (EbfDebug::getDebug()) ccsdsData->print();
    return 0;
}

double DfiParser::timeForTds(double utc) {

    long wholeSeconds = long(floor(utc));
    double frac = utc - wholeSeconds;
    int nanoSec = int(frac/0.000000001);
    facilities::Timestamp facTimeStamp(wholeSeconds, nanoSec);
    double julTimeStamp = facTimeStamp.getJulian();
    astro::JulianDate julDate(julTimeStamp);
    // Find number of seconds since missionStart
    double tdsTimeInSeconds = julDate.seconds() - astro::JulianDate::missionStart().seconds();
    return tdsTimeInSeconds;

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

        if (ldf.status()) {
            std::ostringstream errMsg;
            errMsg.str("LDF EBFeventParser reported a bad status ");
            errMsg << apid;
            throw LdfException(errMsg.str());
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
