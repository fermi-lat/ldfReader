#ifndef LdfParser_H
#define LdfParser_H 1

#include "ldfReader/EbfParser.h"
#include <string>
#include "data/LatData.h"
//#include "../src/iterators/LdfDataParser.h"
#include "ldfReader/iterators/LdfDataParser.h"


/** @class LdfParser
@brief Provides access to the EBF parsing routines and is the gateway to
filling the LatData structure.

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/LdfParser.h,v 1.10.100.1 2011/03/07 19:52:59 heather Exp $
*/

namespace ldfReader {
    class LdfParser : public EbfParser {
    public:

        LdfParser();
        LdfParser(const std::string &fileName, bool fitsWrap = false,
            const std::string& instrument="LAT");

        virtual ~LdfParser();

        virtual void clear();

        /// Load data for the current event in the EBF file
        virtual int loadData();

        /// Moves event pointer to the next event in the EBF file
        virtual int nextEvent();

        virtual void printHeader() const { };

        virtual unsigned long long eventCount() const { return m_evtCount; }

        void ignoreSegFault(bool value);


        /// Do nothing for LdfParser case
        virtual void setGemIdSkipList(const std::vector<unsigned long long> &gemList) { };
        virtual void setEventIndexSkipList(const std::vector<unsigned long long> &eventList) { }; 

        /// Turn on or off debug output.  Returns old value of flag
        //bool setDebug(bool on);

        unsigned int runId() { return m_runId; };

        unsigned int eventId() { return m_eventId; };

        unsigned long eventSize() { return m_eventSize; };

        //bool end(EBFevent *evt);
        //bool end();

        FILE* file_initialize(const char* filename);
        unsigned from_file(FILE *fpevents, unsigned char* buffer, bool *swap =0);
        void file_finalize(FILE *fpevents);

        unsigned* evtId(unsigned char* buffer);
        unsigned* evtsize(unsigned char* buffer);
        unsigned* evtremaining(unsigned char* buffer);

        // local exception class
        //class Exception{ };

    private:
        double timeForTds();

        std::string m_fileName;
        bool        m_fitsWrap;

        FILE   *m_ebf;

        // This is really of type (fitsfile *) but don't want to include all of
        // fitsio.h just for that!  Means we have to cast and copy a bunch.
        void          *m_fitsfile;

        long           m_maxRow;
        long           m_currentRow;
        int            m_maxHdu;
        int            m_currentHdu;

        // buffer for the event
        unsigned char  *m_rowBuf;   // could just use m_datagram
        long            m_maxSize;
        long            m_evtCount;

        std::string     m_instrument;

        unsigned int m_runId;
        unsigned int m_eventId;
        unsigned long m_eventSize;

        int m_ebfSize;

        unsigned char* m_buffer;   // probably obsolete

        static const unsigned BufferSize;

        //    EBFevent *m_evt, *m_end;
        //LATdatagram *m_end, *m_start, *m_datagram;
        //EbfDatagramParser *m_datagramParser;
        LdfDataParser m_dataParser;

        bool m_swap;  // Flag denoting if byte swapping is necessary

        /** Local method which attempts to read row from FITS table,
        update LATdatagram pointer.  Swaps data if necessary.
        If successful will advance m_currentRow pointer.
        @returns  #bytes read
        */
        long getRow();

    };
}
#endif
