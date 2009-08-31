#ifndef DfiParser_H
#define DfiParser_H 1

#include "ldfReader/EbfParser.h"
#include <string>
#include "data/LatData.h"
#include "EBFevent.h"

#include "lsfData/LSFReader.h"
#include "lsfData/LsfMetaEvent.h"
#include "lsfData/LsfCcsds.h"
#include "eventFile/EBF_Data.h"

//#include "../src/iterators/LdfDataParser.h"
//#include "../src/iterators/EbfDataParser.h"
#include "ldfReader/iterators/EbfDataParser.h"

#include <vector>

/** @class DfiParser
@brief Provides access to the EBF parsing routines and is the gateway to
filling the LatData structure.

$Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/ldfReader/ldfReader/DfiParser.h,v 1.13 2008/11/11 04:28:55 heather Exp $
*/

namespace ldfReader {
    class DfiParser : public EbfParser {
    public:

        DfiParser();

        DfiParser(const std::string &filename);

        virtual ~DfiParser();

        virtual void printHeader() const;

        virtual void clear();

        virtual unsigned long long eventCount() const { 
            return m_file->evtcnt(); 
        }

        /// Load data for the current event in the EBF file
        virtual int loadData();

        /// Moves event pointer to the next event in the EBF file
        virtual int nextEvent();

        /// Turn on or off debug output.  Returns old value of flag
        //bool setDebug(bool on);

        unsigned int runId() { return m_runId; };

        unsigned int eventId() { return m_eventId; };

        unsigned long eventSize() { return m_eventSize; };

        virtual void setIgnoreSegFault() { } ;

        // local exception class
        //class Exception{ };

    private:
        double timeForTds(double utc);

        int readContextAndInfo();

        unsigned char* rePacketizeEbf(const unsigned char* data, int dataLen, int& packetLen);

        lsfData::LSFReader *m_file;

        //long            m_evtCount;

        std::string     m_instrument;

        unsigned int m_runId;
        unsigned int m_eventId;
        unsigned long m_eventSize;

        EBFevent *m_end, *m_start;

        EbfDataParser m_dataParser;

        lsfData::LsfCcsds   m_ccsds;
        lsfData::MetaEvent  m_meta;
        eventFile::EBF_Data m_ebf;
        unsigned char       mybuff[128*1024];
        unsigned char*      m_ebfPkts;
        int                 m_ebfPktsLen;

        bool m_more;

  //      ldfReader::ccsds2lsf m_cnv;

    };
}
#endif
