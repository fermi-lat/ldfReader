#ifndef ldfReader_SocketParser_CXX
#define ldfReader_SocketParser_CXX

/** @file SocketParser.cxx
@brief Implementation of the SocketParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/SocketParser.cxx,v 1.8 2008/12/03 15:43:00 heather Exp $
*/

#include "ldfReader/SocketParser.h"
//#include "iterators/EbfDatagramParser.h"
//#include "iterators/TkrParser.h"
#include "EbfDebug.h"
#include "ldfReader/LdfException.h"
#include <exception>
#include <sstream>
#include <iostream>
#include <math.h> // for floor
#include <csignal> // For signal handling

#ifndef WIN32 // Linux UDP sockets
#include <sys/socket.h> // for sockets!
#include <unistd.h>
#include <netdb.h> // for gethostbyname
//#include <arpa/inet.h> //inet_addr
#include <errno.h>
#endif

#include "facilities/Timestamp.h"
#include "facilities/Util.h"
#include "astro/JulianDate.h"

namespace ldfReader {

// pulled from HEAD of LATTE/tools/DataDistributor.py
const unsigned SocketParser::PORT = 54321;

const unsigned SocketParser::BufferSize = 64*1024;

const std::string SocketParser::GROUP = "239.255.1.";

SocketParser::SocketParser(unsigned int server) {
    try {
#ifdef WIN32
        throw LdfException("No Windows support for sockets yet!");
#else
        clear();
        // Set up a UDP client
        if ( (m_handle = socket(AF_INET, SOCK_DGRAM, 0) ) < 0) 
            throw LdfException("Failed to receive handle from socket"); 
        bzero(&m_client_addr, sizeof(m_client_addr));
        m_client_addr.sin_family = AF_INET;
        m_client_addr.sin_port = htons(PORT);
        m_client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        unsigned int yes = 1;
        if (setsockopt(m_handle,SOL_SOCKET,SO_REUSEADDR,&yes, sizeof(yes)) < 0)
            throw LdfException("reuse addr");

        if (bind(m_handle, (struct sockaddr*)&m_client_addr, 
                  sizeof(m_client_addr)) < 0) 
            throw LdfException("failed bind");

        unsigned int bufSize = BufferSize;
        if (setsockopt(m_handle, SOL_SOCKET, SO_RCVBUF, &bufSize, sizeof(bufSize) ) < 0)
            throw LdfException("failed to set RCVBUF");

        struct ip_mreq mreq;
        bzero(&mreq,sizeof(struct ip_mreq));
        // Create group string using input server number
        std::string hostName = GROUP;
        std::string outStr;
        hostName += facilities::Util::itoa((server % 100), outStr);
        struct hostent *group;
        if ((group = gethostbyname(hostName.c_str())) == (struct hostent *)0)
            throw LdfException("failed to gethostbyname");
        std::cout << "ldfReader::SocketParser  Joining group: " << hostName << std::endl;
        struct in_addr ia;
        bcopy((void*)group->h_addr, (void*)&ia, group->h_length);
        bcopy(&ia, &mreq.imr_multiaddr.s_addr, sizeof(struct in_addr));

        // Request group membership
        mreq.imr_interface.s_addr=htonl(INADDR_ANY);
        if (setsockopt(m_handle,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,
                       sizeof(mreq)) < 0) 
            throw LdfException("sockopt");


        // Request the first event be loaded
        if (nextEvent() < 0) 
            throw LdfException("Failed to load first event via socket");
#endif

    } catch( LdfException& e) {
        std::cerr << "Caught LdfException: " << e.what() << std::endl;
        throw;
    } catch(...) {
        std::cerr << "Error Setting up SocketParser" << std::endl;
        throw;
    }
}


    SocketParser::~SocketParser() {
#ifndef WIN32
        close(m_handle);
#endif
        clear();
    }

    void SocketParser::clear() {
        m_runId = 0;
        m_eventId = 0;
        m_eventSize = 0;
        m_ebfSize = 0;
    }


    int SocketParser::nextEvent() {
        try {
#ifndef WIN32
            static char buffer[BufferSize+1];
            unsigned int ibuf;
            for (ibuf = 0; ibuf < BufferSize+1; ibuf++)
                buffer[ibuf] = 0;
            if (EbfDebug::getDebug())
                std::cout << "ldfReader::SocketParser calling recvfrom" << std::endl;
            int addrLen = sizeof(m_client_addr);
            int stat = recvfrom(m_handle, buffer, BufferSize, 0, 
                 (struct sockaddr *)&m_client_addr, &addrLen);  
            if (stat < 0 ) {
                std::cout << "recvfrom failed " << stat << " "
                          << strerror(errno) << std::endl;
                return -1;
            } else if (EbfDebug::getDebug())
                std::cout << "read " << stat << " bytes" << std::endl;

            m_buffer = buffer;
            //if (!DFC_BIG_ENDIAN)
            //    EBF_swap32_lclXbigN((unsigned*)buffer, stat / sizeof (unsigned));

            //m_start = (LATdatagram*)buffer;
            //m_end = (LATdatagram*)(&buffer[stat]);
            m_eventSize = stat;
            //m_datagram = m_start;
#endif
            return 0;
    
      } catch(LdfException &e) {

        std::cerr << "LdfException caught: " << e.what() << std::endl;
        throw;
      } catch(...) {

        std::cerr << "Unknown Exception caught " << std::endl;
        throw;
      }

    }

int SocketParser::loadData() {
    // Purpose and Method:  This routine loads the data from one event
    // The current event in the EBF file.

    static long eventSeqNum = -1;
    // From Ric Claus Aug, 2004
    const long maxEventSeqNum = 131071;


    try {
        // First clear the LatData
        ldfReader::LatData::instance()->clearTowers();
        ldfReader::LatData::instance()->setRunId(m_runId);
        ldfReader::LatData::instance()->setEventSizeInBytes(m_eventSize);

        if (ldfReader::LatData::instance()->ignoreSegFault()) 
            ignoreSegFault(true);

//        if (m_datagram >= m_end) return -1;

////        EbfDatagramParser ldf(m_start, m_end);
//        ldf.iterate();
//        if (ldf.status()) {
        //Parse the data
        //m_dataParser.iterate(m_buffer,m_eventSize);
        /*unsigned int count =*/ m_dataParser.iterate2(m_buffer, m_eventSize, false);  // not sure how to set swap by default
        EBFeventIterator *ebfIt = dynamic_cast<EBFeventIterator*>(&m_dataParser);
        //unsigned int status = m_dataParser.status();
        unsigned int status = ebfIt->status();

        if (status) {
            std::ostringstream errMsg;
            errMsg.str("LDF EBFeventParser reported a bad status 0x");
            errMsg << std::hex << status << " = " << std::dec
                   << status << " EventId: " 
                   << ldfReader::LatData::instance()->getOsw().evtSequence();
            std::cout << errMsg << std::endl;
            ldfReader::LatData::instance()->setBadLdfStatusFlag();
            //throw LdfException(
            //"LDF LatDatagramIterator reported a bad status");
         }
   

        // Store run Id locally if we found a header or trailer
        if (ldfReader::LatData::instance()->adfHdrTlr()) 
            m_runId = ldfReader::LatData::instance()->runId();

        if (ldfReader::LatData::instance()->ignoreSegFault()) 
            ignoreSegFault(false);

        // Only do this check on the event sequence if we have a recent
        // enough file..  I believe we want one where they started to store the
        // event summary in each contribution separately
        if (ldfReader::LatData::instance()->getFormatIdentity() >= 
            EbfDataParser::ID_WITH_OSW) {

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

     ldfReader::LatData::instance()->setTimeInSecTds(timeForTds());
     ldfReader::LatData::instance()->setEventId(ldfReader::LatData::instance()->getOsw().evtSequence());

     ldfReader::LatData::instance()->checkTemError();
     ldfReader::LatData::instance()->checkPacketError();
     ldfReader::LatData::instance()->checkTrgParityError();
     ldfReader::LatData::instance()->checkAemError();
     ldfReader::LatData::instance()->checkCalReadout();


      } catch (LdfException& e) {
          std::cerr << "Caught LdfException: " << e.what() << std::endl;
          throw;
      } catch(...) {
          std::cerr << "Caught Exception" << std::endl;
          throw;
      }
      return 0;
} 


//    bool SocketParser::end() {
//        return (m_datagram < m_end);
//    }



double SocketParser::timeForTds() {
    unsigned upperPpcTime = ldfReader::LatData::instance()->summaryData().upperPpcTimeBaseWord();
    unsigned lowerPpcTime = ldfReader::LatData::instance()->summaryData().lowerPpcTimeBaseWord();
    const double sixteenMHz = 16000000.;
    // To handle th 64-bit value - we separate the computation
    // The upper 32 bits would have to be shifted by 31 (or multiplied)
    // by 2^32 - we divide this by 16000000 to get the upperMultiplier
    const double upperMultiplier = 268.435456;
    double lower = lowerPpcTime / sixteenMHz;
    double upper = upperPpcTime * upperMultiplier;
    double ppcSeconds =  upper + lower;
    float ppcWholeSeconds = floor(ppcSeconds);
    double frac = ppcSeconds - ppcWholeSeconds;
    int ppcNanoSec = (int)floor(frac / 0.000000001);

    // To eliminate duplicate times, we make use of the PPC time as suggested
    // by Jim Panetta.  Here we ignore the fractional seconds from the "real"
    // absolute time and add in the fractional seconds as computed by the PPC
    // word.  This should produce a unique time stamp for each event.
    facilities::Timestamp facTimeStamp(
             ldfReader::LatData::instance()->summaryData().timeSec(),
             ppcNanoSec);
    double julTimeStamp = facTimeStamp.getJulian();
    astro::JulianDate julDate(julTimeStamp);
    // Find number of seconds since missionStart
    double tdsTimeInSeconds = julDate.seconds() - astro::JulianDate::missionStart().seconds();

    return tdsTimeInSeconds;
}


void SocketParser::ignoreSegFault(bool value)  {
    // Purpose and Method:  Allows Client to ignore segmentation faults
    // Not using POSIX for now
    if (value)
        signal(SIGSEGV, SIG_IGN);
    else
        signal(SIGSEGV, SIG_DFL);
}

} //namespace
#endif
