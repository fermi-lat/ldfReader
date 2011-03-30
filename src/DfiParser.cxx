#ifndef ldfReader_DfiParser_CXX
#define ldfReader_DfiParser_CXX


/** @file DfiParser.cxx
@brief Implementation of the DfiParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/DfiParser.cxx,v 1.39.68.2 2011/03/29 04:42:38 heather Exp $
*/

#include "ldfReader/DfiParser.h"
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

#include "EDS/io/EBF_evts.h"

namespace ldfReader {

DfiParser::DfiParser():m_dataParser("") {
    clear();
}


DfiParser::DfiParser(const std::string &filename) : m_dataParser(""), m_ebfPkts(0), m_indexCounter(0) {
    //Purpose and Method:  Ctor using LPA file as input 
    // Open input file and read in first event
    //Inputs:  filename 
    try {
       clear();
       m_file = NULL;
       m_file = new lsfData::LSFReader(filename);
       std::cout << "Created eventFile" << std::endl;
       m_runId = m_file->runid();
       m_more = m_file->read(m_ccsds, m_meta, m_ebf);
       if (!m_more) {
           std::cout << "No events in input file" << std::endl;
           throw;
       }
       m_indexCounter++;

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

    if (m_ebfPkts) 
    {
        delete m_ebfPkts;
        m_ebfPkts = 0;
    }
}

void DfiParser::clear() {
    m_runId = 0;
    m_eventId = 0;
    m_eventSize = 0;
    m_ccsds.clear();
    m_meta.clear();
    m_gemIdSkipList.clear();
    m_eventIndexSkipList.clear();

    if (m_ebfPkts) 
    {
        delete m_ebfPkts;
        m_ebfPkts = 0;
    }
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
        m_indexCounter++;
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

    metaEvent->setCompressionLevel(m_meta.compressionLevel());
    metaEvent->setCompressedSize(m_meta.compressedSize());

    //metaEvent->setHandlerCol(m_meta.lpaHandlerCol());
    if (m_meta.gammaFilter()) metaEvent->addGammaHandler(*(m_meta.gammaFilter())); 
    if (m_meta.mipFilter()) metaEvent->addMipHandler(*(m_meta.mipFilter())); 
    if (m_meta.hipFilter()) metaEvent->addHipHandler(*(m_meta.hipFilter())); 
    if (m_meta.dgnFilter()) metaEvent->addDgnHandler(*(m_meta.dgnFilter())); 
    if (m_meta.passthruFilter()) metaEvent->addPassthruHandler(*(m_meta.passthruFilter())); 
    if (m_meta.lpaHandler()) metaEvent->addLpaHandler(*(m_meta.lpaHandler())); 
 
    lsfData::LsfCcsds* ccsdsData = ldfReader::LatData::instance()->getCcsdsPtr();
    ccsdsData->initialize(m_ccsds.getScid(), m_ccsds.getApid(), m_ccsds.getUtc());

    // Do this after reading LDF data
    //ldfReader::LatData::instance()->setTimeInSecTds(timeForTds(m_ccsds.getUtc()));
    ldfReader::LatData::instance()->setEventId(m_meta.scalers().sequence());
    ldfReader::LatData::instance()->setEventIndex(m_indexCounter);
 
    if (EbfDebug::getDebug()==EbfDebug::ALL) ccsdsData->print();
    return 0;
}


double DfiParser::timeForTds(double utc) {

// Code from Anders July, 2006. With many updates :-)
// Update April 2008: See JIRA https://jira.slac.stanford.edu/browse/GRINF-46
//                    We now also have the sourceGps() flag instead of missingGps().
//                    Also note that the lsb for the double precision for the full time stamp is 30 ns.            

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

    // UDL does averaging for us when there is no GPS lock.
    // Also accepting early events.
    // If there are no 1-PPS at CPU or LAT level we are probably hosed.
    // Will use 20 MHz in that case.
    if (!(metaEvent.time().current().missingCpuPps()) &&
        !(metaEvent.time().current().missingLatPps()) &&
	 // That was the current Timetone. Now the previous one. 
        !(metaEvent.time().previous().missingCpuPps()) &&
        !(metaEvent.time().previous().missingLatPps()) &&
	// Avoid 1/0 error: This could probably be replaced by using 'RollOver' instead  
        // because of the subsequent requirement.   
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
     
      // To turn on: EventSelector.EbfDebugLevel = 1;
      if (EbfDebug::getDebug()>=EbfDebug::ERRMSG)
	std::cout << "ldfReader::DfiParser - Using fixed 20 MHz!" << std::endl;
      
    }

    // Event time stamp:
    return timestamp;
}

unsigned char* DfiParser::rePacketizeEbf(const unsigned char* data, int dataLen, int& packetLen)
{
    // As the name implies, this method takes the raw ebf data buffer read back and 
    // re-packetizes it so that FSW running on the ground can eat it. 
    // First version, July 4th weekend, 2008

    // Set up to start looping through the data buffer
    // What follows here will create the EBF_pkts object in C++ (as opposed to C)
    unsigned int* dataPtr = (unsigned int*)data;

    // Keep track of total packet length
    packetLen = 0;

    // Use a temporary buffer for storing the packetized data. Will then copy to 
    // an allocated buffer once the length is known
    unsigned char  tempBuffer[128*1024];
    unsigned int*  packetBuf = (unsigned int*)tempBuffer;

    // Preserve the gem contribution (necessary for multi-packet events)
    EBF_ebw* gem = (EBF_ebw*)dataPtr;

    // Set up to loop over data
    bool firstPkt = true;
    int  seq      = 0;

    while(dataLen > 0)
    {
        // First data contribution (best be gem word)
        EBF_ebw* ctb = (EBF_ebw*)dataPtr;

        // Determine max possible size of a packet
        int maxSize = (1 << 10) * sizeof(int) - 1;

        // Special case, must leave room for restart header if not first packet
        if (!firstPkt) maxSize -= 4 * sizeof(int);

        // Size of the packet we'll actually work with (tbd)
        int pktSize = 0;

        // Ok if remaining length is larger than max size allowed, we must search
        // for the last contributor that would fit
        if (dataLen > maxSize)
        {
            // Loop through contributions look for last in packet
            while(maxSize > 0)
            {
                // size of current contribution
                int ebwSize = EBF_EBW_LEN_TO_BYTES(ctb->bf.len);

                // Keep track of remaining room in this packet
                maxSize -= ebwSize;

                // If room remaining, advance pointer and increment packet size
                if (maxSize >= 0)
                {
                    pktSize += ebwSize;
                    ctb      = (EBF_ebw*)((unsigned char*)ctb + ebwSize);
                }
            }
        }
        // Otherwise, we can just set the length to the dataLen
        else pktSize = dataLen;

        // Pointer to the data packet
        EBF_ebw  ebw  = *gem;
        EBF_edw  edw;

        edw.ui = 0;

        // If first (or only) packet, then handle here
        if (firstPkt)
        {
            int nWords = (pktSize + sizeof(EBF_pktHdr)) / sizeof(int);

            // Total packet length
            packetLen += nWords * sizeof(int);

            // Copy this packet to the buffer
            memcpy(&packetBuf[8], dataPtr, pktSize);

            // Pointer to the packet header so we can fill in the information
            EBF_pkt* newPkt = (EBF_pkt*)packetBuf;

            // ebw word
            ebw.bf.seq     = seq++;
            newPkt->ebw.ui = ebw.ui;

            // edw word
            edw.bf.len     = pktSize / sizeof(int);
            edw.bf.rstatus = dataLen - pktSize > 0 
                           ? EBF_EDW_RSTATUS_K_PACKET_TRUNCATED
                           : EBF_EDW_RSTATUS_K_SUCCESS;

            newPkt->hdr.undef[7] = edw.ui;

            // Advance the buffer pointer to next potential location
            packetBuf += nWords;

            firstPkt = false;
        }
        // Otherwise, we need to tack on the restart header
        else
        {
            // Size of this packet in words
            int nWords = (pktSize + sizeof(EBF_pktRestartHdr)) / sizeof(int);

            // Total packet length
            packetLen += nWords * sizeof(int);

            // Copy the data in this packet
            memcpy(&packetBuf[12], dataPtr, pktSize);

            // Pointer to the restart header
            EBF_pktRestartHdr* restart = (EBF_pktRestartHdr*)packetBuf;

            // ebw word
            ebw.bf.seq     = seq++;

            // edw word(s)
            edw.bf.len     = (pktSize + 4 * sizeof(int)) / sizeof(int);
            edw.bf.rstatus = dataLen - pktSize > 0 
                           ? EBF_EDW_RSTATUS_K_PACKET_TRUNCATED
                           : EBF_EDW_RSTATUS_K_SUCCESS;
            edw.bf.xstatus = EBF_EDW_XSTATUS_K_SUCCESS;

            // Update restart header
            restart->hdr.undef[7] = edw.ui;
            restart->cell.ebw.ui  = ebw.ui;
            restart->cell.mbz[0]  = 0;
            restart->cell.mbz[1]  = 0;
            restart->cell.mbz[2]  = 0;

            // Advance the buffer pointer to next potential location
            packetBuf += nWords;
        }

        // At this point we can advance position in buffer
        dataPtr += pktSize / sizeof(int);
        dataLen -= pktSize;
    }

    // Ok, now have size of packetized data so allocate buffer and copy data
    unsigned char* pktData = new unsigned char[packetLen];

    memcpy(pktData, tempBuffer, packetLen);

    return pktData;
}

int DfiParser::loadData() {
// Purpose and Method:  This routine loads the data from one event
// The current event in the EBF file.


    try {
        // First clear the LatData
        ldfReader::LatData::instance()->clearTowers();

        ldfReader::LatData::instance()->setEventSizeInBytes(m_eventSize);

        readContextAndInfo();

        if (ldfReader::LatData::instance()->oldStyleRunId())
            ldfReader::LatData::instance()->setRunId(m_runId);
        else{
            const lsfData::MetaEvent& meta = ldfReader::LatData::instance()->getMetaEvent();
            ldfReader::LatData::instance()->setRunId(meta.run().startTime());
        }

/* Disable this for now, while sorting out need to insert an empty event
   in output ROOT files

        /// Before proceeding, check to see if this an event we are to skip
        if (m_gemIdSkipList.size() > 0) {
            if (std::find( m_gemIdSkipList.begin(),m_gemIdSkipList.end(),ldfReader::LatData::instance()->eventId() ) != m_gemIdSkipList.end()) return 0;
        }

        if (m_eventIndexSkipList.size() > 0) {
            if (std::find(m_eventIndexSkipList.begin(),m_eventIndexSkipList.end(),m_indexCounter) != m_eventIndexSkipList.end()) {
                std::cout << "Skipping Event Index " << m_indexCounter 
                          << " per JO Request" << std::endl;
                return 0;
            }
        }
*/


        memset(mybuff,0,128*1024);
        memcpy(mybuff, m_ebf.data()+8, m_ebf.size()-8);
//        ldfReader::LatData::instance()->setEbf((char*)mybuff, m_ebf.size()-8);

        // Clean up from last time through
        // In the end, maybe this is better done with a static buffer?
        if (m_ebfPkts) 
        {
            delete m_ebfPkts;
            m_ebfPkts = 0;
        }

        // Ok, re-packetize the ebf data
        m_ebfPkts = rePacketizeEbf(m_ebf.data()+8, m_ebf.size()-8, m_ebfPktsLen);
        ldfReader::LatData::instance()->setEbf((char*)m_ebfPkts, m_ebfPktsLen);


        //m_dataParser.iterate(m_start, m_end);
        //int status = m_dataParser.EBF(m_start, m_end);
        unsigned int count = m_dataParser.iterate2(m_ebf.data(), m_ebf.size(), false);

        unsigned long long eventId = ldfReader::LatData::instance()->eventId();
        int apid = ldfReader::LatData::instance()->getCcsds().getApid();

        if (EbfDebug::getDebug()==EbfDebug::ALL) 
            std::cout << "Event: " << eventId << " APID: " << apid << std::endl;
         fflush(0);
        
        ldfReader::LatData::instance()->setTimeInSecTds(timeForTds(ldfReader::LatData::instance()->getCcsds().getUtc()));


        //if (m_dataParser.status()) {
        EBFeventIterator *ebfIt = dynamic_cast<EBFeventIterator*>(&m_dataParser);
        //unsigned int status = m_dataParser.status();
        unsigned int status = ebfIt->status();
        if (status) {
            std::ostringstream errMsg;
            errMsg << "LDF EBFeventParser reported a bad status 0x";
            //errMsg << std::hex << m_dataParser.status() << " = " << std::dec 
        //           << m_dataParser.status() << " Event: " << eventId 
            errMsg << std::hex << status << " = " << std::dec 
                   << status << " Event: " << eventId 
                   << " APID: " << apid;
            std::cout << errMsg.str() << std::endl;
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
        // These are now handled by handleError callbacks
        //ldfReader::LatData::instance()->checkPacketError();
        //ldfReader::LatData::instance()->checkTrgParityError();
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



}
#endif
