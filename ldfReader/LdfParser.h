#ifndef LdfParser_H
#define LdfParser_H 1

#include "ldfReader/EbfParser.h"
#include <string>
//#include "DFC/EBF_fileIn.h"
#include "data/LatData.h"
#include "EBFevent.h"
#include "LATdatagram.h"
#include "../src/iterators/EbfDatagramParser.h"


/** @class LdfParser
@brief Provides access to the EBF parsing routines and is the gateway to
filling the LatData structure.

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/LdfParser.h,v 1.8 2006/05/25 23:59:26 heather Exp $
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

        /// Turn on or off debug output.  Returns old value of flag
        //bool setDebug(bool on);

        unsigned int runId() { return m_runId; };

        unsigned int eventId() { return m_eventId; };

        unsigned long eventSize() { return m_eventSize; };

        //bool end(EBFevent *evt);
        bool end();

        FILE* file_initialize(const char* filename);
        unsigned from_file(FILE *fpevents, char* buffer);
        void file_finalize(FILE *fpevents);

        unsigned* evtsize(char* buffer);
        unsigned* evtremaining(char* buffer);

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
        LATdatagram *m_end, *m_start, *m_datagram;
        EbfDatagramParser *m_datagramParser;

        /** Local method which attempts to read row from FITS table,
        update LATdatagram pointer.  Swaps data if necessary.
        If successful will advance m_currentRow pointer.
        @returns  #bytes read
        */
        long getRow();

    };
}
#endif
