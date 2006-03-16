#ifndef ldfReader_DfiParser_CXX
#define ldfReader_DfiParser_CXX


/** @file DfiParser.cxx
@brief Implementation of the DfiParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/DfiParser.cxx,v 1.8 2006/03/07 07:23:39 heather Exp $
*/

#include "ldfReader/DfiParser.h"
#include "iterators/EbfEventParser.h"
#include "EbfDebug.h"
#include "ldfReader/LdfException.h"
#include <exception>
#include <iostream>
#include <stdexcept>

#include "facilities/Timestamp.h"
#include "astro/JulianDate.h"

#include "lsfData/LsfTime.h"
#include "./ccsds2lsf.h"

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
       m_file = new eventFile::LPA_File(filename, eventFile::LPA_File::Read);
       std::cout << "Created eventFile" << std::endl;
       m_runId = m_file->runid();
       m_more = m_file->read(m_context, m_info, m_ebf);
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
}



int DfiParser::nextEvent() {
    try {
        bool m_more = m_file->read(m_context, m_info, m_ebf);
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

    lsfData::TimeTone current;
    m_cnv.timeToneCnv(m_context.current, current);
    lsfData::TimeTone previous;
    m_cnv.timeToneCnv(m_context.previous, previous);

    lsfData::GemTime gemTime(m_info.timeHack.hacks, m_info.timeHack.tics);
    lsfData::Time lsfTime(current, previous, gemTime, m_info.timeTics);
    lsfTime.set(current,previous, gemTime, m_info.timeTics);
    metaEvent->setTime(lsfTime);

    lsfData::RunInfo run;
    m_cnv.runInfoCnv(m_context.run, run);
    metaEvent->setRun(run);

    lsfData::DatagramInfo datagramInfo;
    m_cnv.datagramInfoCnv(m_context.open, m_context.close, datagramInfo);
    metaEvent->setDatagram(datagramInfo);

    lsfData::GemScalers scalers;
    m_cnv.scalerCnv(m_context.scalers, scalers);
    metaEvent->setScalers(scalers);

    lsfData::LpaConfiguration config(m_info.hardwareKey, m_info.softwareKey);
    metaEvent->setConfiguration(config);

    lsfData::LsfCcsds* ccsdsData = ldfReader::LatData::instance()->getCcsdsPtr();
    ccsdsData->initialize(m_context.ccsds.scid, m_context.ccsds.apid, 
                          m_context.ccsds.utc);

    ldfReader::LatData::instance()->setTimeInSecTds(timeForTds(m_context.ccsds.utc));
    ldfReader::LatData::instance()->setEventId(scalers.sequence());
 
    return 0;
}

double DfiParser::timeForTds(double utc) {

    long wholeSeconds = floor(utc);
    double frac = utc - wholeSeconds;
    int nanoSec = frac/0.000000001;
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

        if (ldf.status()) 
            throw LdfException("LDF EBFeventParser reported a bad status");
   

        // Only do this check on the event sequence if we have a recent
        // enough file..  I believe we want one where they started to store the
        // event summary in each contribution separately
//        if (ldfReader::LatData::instance()->getFormatIdentity() >= 
//            LatComponentParser::ID_WITH_OSW) {

//            if (!ldfReader::LatData::instance()->eventSeqConsistent()) {
//                printf("Event Sequence numbers are not consistent within all contributions\n");
//                printf("Setting event flag\n");
//                ldfReader::LatData::instance()->setBadEventSeqFlag();
//                return 0;
//            }

//        }

        ldfReader::LatData::instance()->checkTemError();
        ldfReader::LatData::instance()->checkPacketError();
        ldfReader::LatData::instance()->checkTrgParityError();

    } catch (LdfException& e) {
       std::cerr << "Caught LdfException: " << e.what() << std::endl;
       throw;
    } catch(std::runtime_error e ) {
        std::cerr << e.what() << std::endl;
        throw(e);
    } catch(...) {
       std::cerr << "Caught Exception" << std::endl;
       throw;
    }
    return 0;
}

//bool DfiParser::setDebug(bool on) {
//    return EbfDebug::setDebug(on);
//}




}
#endif
