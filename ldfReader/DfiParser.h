#ifndef DfiParser_H
#define DfiParser_H 1

#include "ldfReader/EbfParser.h"
#include <string>
//#include "DFC/EBF_fileIn.h"
#include "data/LatData.h"
#include "EBFevent.h"
//#include "LATdatagram.h"
//#include "../src/iterators/EbfDatagramParser.h"

#include "eventRet/RetDef.h"
#include "eventRet/LSE_Context.h"
#include "eventRet/LSE_Info.h"
#include "eventRet/EBF_Data.h"
#include "eventRet/LPA_Merger.h"

#include <vector>

/** @class DfiParser
@brief Provides access to the EBF parsing routines and is the gateway to
filling the LatData structure.

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/DfiParser.h,v 1.3 2005/04/18 17:36:16 heather Exp $
*/

namespace ldfReader {
    class DfiParser : public EbfParser {
    public:

        DfiParser();
        DfiParser(const std::string &arch, int scid, 
                  std::vector<eventRet::ApidSpan*> &spans);

        DfiParser(const std::string &filename);

        virtual ~DfiParser();

        virtual void clear();

        /// Load data for the current event in the EBF file
        virtual int loadData();

        /// Moves event pointer to the next event in the EBF file
        virtual int nextEvent();

        /// Turn on or off debug output.  Returns old value of flag
        //bool setDebug(bool on);

        unsigned int runId() { return m_runId; };

        unsigned int eventId() { return m_eventId; };

        unsigned long eventSize() { return m_eventSize; };

//        bool end();

        // local exception class
        //class Exception{ };

    private:

        eventRet::RetDef *m_retDef; // packet retrieval interface
        eventRet::LPA_Merger *m_eventMerger; // Interface to merging library

        //long            m_evtCount;

        std::string     m_instrument;

        unsigned int m_runId;
        unsigned int m_eventId;
        unsigned long m_eventSize;

        EBFevent *m_end, *m_start;

        eventRet::LSE_Context m_context;
        eventRet::EBF_Data m_ebf;
        eventRet::LPA_Info m_info;


    };
}
#endif
