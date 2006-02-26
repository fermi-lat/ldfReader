#ifndef ldfReader_DfiParser_CXX
#define ldfReader_DfiParser_CXX


/** @file DfiParser.cxx
@brief Implementation of the DfiParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/DfiParser.cxx,v 1.4 2006/02/25 08:30:51 heather Exp $
*/

#include "ldfReader/DfiParser.h"
#include "iterators/EbfEventParser.h"
#include "EbfDebug.h"
#include "ldfReader/LdfException.h"
#include <exception>
#include <iostream>
#include <stdexcept>

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
       m_more = m_file->read(m_context, m_info, m_ebf);
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
 
    return 0;
}

int DfiParser::loadData() {
// Purpose and Method:  This routine loads the data from one event
// The current event in the EBF file.

    static long eventSeqNum = -1;
    // From Ric Claus Aug, 2004
    const long maxEventSeqNum = 131071;


    try {
        // First clear the LatData
        ldfReader::LatData::instance()->clearTowers();

 // FIXME -  need a run id
//      ldfReader::LatData::instance()->setRunId(m_runId);

        ldfReader::LatData::instance()->setEventSizeInBytes(m_eventSize);

        readContextAndInfo();


        EbfEventParser ldf;
        ldf.iterate(m_start, m_end);

        if (ldf.status()) throw LdfException("LDF EBFeventParser reported a bad status");
   

        // Only do this check on the event sequence if we have a recent
        // enough file..  I believe we want one where they started to store the
        // event summary in each contribution separately
        if (ldfReader::LatData::instance()->getFormatIdentity() >= 
            LatComponentParser::ID_WITH_OSW) {

            if (!ldfReader::LatData::instance()->eventSeqConsistent()) {
                printf("Event Sequence numbers are not consistent within all contributions\n");
                printf("Setting event flag\n");
                ldfReader::LatData::instance()->setBadEventSeqFlag();
                return 0;
            }

            // Now check to see that the event sequences are monotonically increasing
            if (ldfReader::LatData::instance()->summaryData().eventSequence() < eventSeqNum) {
                static bool warn = false;
                if (!warn) {
                    printf("WARNING Event Seq # is not monotonically increasing  please check log to see if this was a prescaled/filtered run  ");
                    printf("Last EventSeqNum %lu, current %lu\n", eventSeqNum,
                    ldfReader::LatData::instance()->summaryData().eventSequence());
                    warn = true;
                }
                eventSeqNum = ldfReader::LatData::instance()->summaryData().eventSequence();
                // Feb 2, 2005 HMK
                // Don't set bad event flag..until we have a way to tell
                // if prescaling or filtering is on..
                //printf("Setting Bad Event Flag\n");
                //ldfReader::LatData::instance()->setBadEventSeqFlag();
                return 0;
            } else {
                eventSeqNum = ldfReader::LatData::instance()->summaryData().eventSequence();
            }

        // reset the stored event sequence number when we hit the LDF's rollover
        // value
        if (eventSeqNum == maxEventSeqNum) eventSeqNum = -1;
        }

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
