#ifndef ldfReader_LdfParser_CXX
#define ldfReader_LdfParser_CXX


/** @file LdfParser.cxx
@brief Implementation of the LdfParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/LdfParser.cxx,v 1.2 2004/08/25 22:37:11 heather Exp $
*/

#include "ldfReader/LdfParser.h"
#include "iterators/EbfDatagramParser.h"
#include "iterators/TkrParser.h"
#include "EbfDebug.h"
#include <iostream>

#include "fitsio.h" 
// so that we can use longer, more descriptive names for FITS routines
#include "longnam.h"

namespace ldfReader {
    LdfParser::LdfParser() {
        clear();
    }

    LdfParser::LdfParser(std::string fileName, bool fitsWrap, 
        const std::string& instrument) :
    m_fileName(fileName), m_fitsWrap(fitsWrap), m_fitsfile(0),
        m_maxRow(0), m_currentRow(0), m_maxHdu(0), m_currentHdu(0),
        m_rowBuf(0), m_maxSize(0), m_evtCount(0), m_instrument(instrument)
    {
        try {
            // MyTkrIterator::setInstrument(instrument);
            if (fitsWrap) {
                fitsfile* ffile;
                int status = 0;
                // Do fits open on file (ffopen)
                fits_open_file(&ffile, fileName.c_str(), READONLY,
                    &status);
                m_fitsfile = (void *) ffile;
                fits_get_num_hdus(ffile, &m_maxHdu, &status); // ffthdu

                if ((status != 0) || (m_maxHdu < 2)) {
                    ;  // complain
                    throw Exception();
                }

                // Move to Primary Header
                int hduType;
                m_currentHdu = 1;
                fits_movabs_hdu(ffile, m_currentHdu, &hduType, &status); // ffmahd

                // Get the RunId from the Primary Header
                char* comment=0;
                char runId[15];
                fits_read_keyword(ffile, "KEY_0001", runId, comment, &status);
                if (status == 0) {
                    std::string runIdStr(runId);
                    std::string tempStr("'RunId   '");
                    if (runIdStr == tempStr) {
                        fits_read_key(ffile, TULONG, "VAL_0001", &m_runId, comment, &status);
                        if (status != 0) m_runId = 0;
                    }
                } else 
                    status = 0;

                // Go to 2nd HDU
                m_currentHdu = 2;
                fits_movabs_hdu(ffile, m_currentHdu, &hduType, &status); // ffmahd
                if ((status != 0) || (hduType != BINARY_TBL)) {
                    ;  // complain
                    throw Exception();
                }
                // Find out total # of rows; initialize counter
                fits_get_num_rows(ffile, &m_maxRow, &status);   // ffgnrw
                if (status != 0) {
                    ;  // complain
                    throw Exception();
                }

                // Get value of keyword "EVTCOUNT".  The 4th argument could be
                // a pointer to char if we cared about the comment string.
                fits_read_key_lng(ffile, "EVTCOUNT", &m_evtCount, 0, &status);
                if (m_evtCount < 0) throw Exception();


                // Believe that first row in FITS table is called 1.
                m_currentRow = 1;

                long nbytes = getRow();

                // start and end are meaningless in FITS case since EBF 
                // library does not get us the next event
                m_datagramParser = new EbfDatagramParser(0, 0);
            }
            else  { // For EBF or ARCH files
                m_ebf  = EBF_open(m_fileName.c_str(), 0);
                EBF_read(m_ebf);
                EBF_close(m_ebf);

                LATdatagram* start = (LATdatagram*)EBF_edata(m_ebf);
                LATdatagram* end   = (LATdatagram*)(&((char*)start)[EBF_size(m_ebf)]);

                m_datagram = start;
                m_datagramParser = new EbfDatagramParser(start, end);

                m_start = start;
                m_end = end;
            }
        } catch(...) {
            std::cerr << "Error setting up LdfParser" << std::endl;
            throw;
        }
    }


    LdfParser::~LdfParser() {
        clear();
        fitsfile* ffile = (fitsfile *) m_fitsfile;
        delete ffile;
    }

    void LdfParser::clear() {
        //if (m_datagram) delete m_datagram;
        //m_datagram = 0;
        m_runId = 0;
        m_eventId = 0;
        m_eventSize = 0;
        m_ebfSize = 0;
        if (m_ebf) EBF_free(m_ebf);
        m_ebf = 0;
        //m_evt = 0;
    }

    long LdfParser::getRow() {
        int status = 0;
        //  get next row length and starting address
        long nbytes;
        long heapAddr;

        fitsfile* ffile = (fitsfile*) m_fitsfile;

        fits_read_descript(ffile, 1, m_currentRow, &nbytes,     // ffgdes
            &heapAddr, &status);

        if (!m_rowBuf) { // just allocate what we need for this event 
            m_maxSize = nbytes;
            m_rowBuf = new unsigned char[nbytes];
        }
        else if (nbytes > m_maxSize) {
            delete [] m_rowBuf;
            while (nbytes > m_maxSize) {
                m_maxSize *= 2;
            }
            m_rowBuf = new unsigned char[m_maxSize];
        }

        //  read in the row              
        // fits_read_col_byt(fileptr, column, firstrow, firstarrayix,
        //                   narrayelt, nulval, outputarray, anynulPtr, statusptr)
        // What's "anynul" ?
        fits_read_col_byt(ffile, 1, m_currentRow, 1, nbytes,    // ffgcvb
            0, m_rowBuf, 0, &status);
        m_currentRow++;

        //  pass to EBF stuff, letting it swap if necessary
        if (status != 0) {
            ; //complain
            m_datagram = 0;
            return 0;    // no bytes read
        }
        m_datagram = LATdatagram::create(m_rowBuf, nbytes - sizeof(LATdatagram));

        // This will do swapping if needed on event data.

        return nbytes;
    }


    int LdfParser::nextEvent() {
        if (m_fitsWrap) {
            int status = 0;
            int hduType;
            fitsfile* ffile = (fitsfile *) m_fitsfile;

            if (m_currentRow > m_evtCount) {
                if (m_currentHdu == m_maxHdu) {
                    // end of file
                    m_datagram = 0;
                    return -1;
                }
                // Otherwise move on to next HDU
                m_currentHdu++;
                fits_movrel_hdu(ffile, 1, &hduType, &status);   // ffmrhed
                if (status != 0) {
                    m_datagram = 0;

                    // complain
                    return -1;
                }
                else if (hduType != BINARY_TBL) {
                    m_datagram = 0;

                    // complain
                    return -1; 
                }

                fits_get_num_rows(ffile, &m_maxRow, &status);  // ffgnrw
                if (status != 0) {
                    m_datagram = 0;
                    // complain
                    return -1;  
                }

                // Get value of keyword "EVTCOUNT".  The 4th argument could be
                // a pointer to char if we cared about the comment string.
                fits_read_key_lng(ffile, "EVTCOUNT", &m_evtCount, 0, &status);

                m_currentRow = 1;
            }
            long nbytes = getRow();
            // nbytes should never be 0, but just in case, leave this check in
            return (nbytes > 0) ? 0 : -1;
        }
        else {  // raw ebf
            m_datagram = m_datagram->next();
            return 0;
        }
    }

    int LdfParser::loadData() {
        // Purpose and Method:  This routine loads the data from one event
        // The current event in the EBF file.

        static long eventSeqNum = -1;
        // From Ric Claus Aug, 2004
        const long maxEventSeqNum = 131071;

        // First clear the LatData
        ldfReader::LatData::instance()->clearTowers();
        ldfReader::LatData::instance()->setRunId(m_runId);

        if ( (m_fitsWrap)  && (m_datagram == 0) ) {
            return -1;
        }
        else if ((!m_fitsWrap) && (m_datagram >= m_end)) return -1;

        if (m_datagramParser->process(m_datagram) != 0)
        {
            // An error occured
            return -1;
        }

        if (!ldfReader::LatData::instance()->eventSeqConsistent()) {
            printf("Event Sequence numbers are not consistent within all contributions\n");
            printf("Setting bad event flag\n");
            ldfReader::LatData::instance()->setBadEventSeqFlag();
            return 0;
        }

        // Now check to see that the event sequences are monotonically increasing
        if (ldfReader::LatData::instance()->summaryData().eventSequence() < eventSeqNum) {
            printf("Setting Bad Event Flag\n");
            ldfReader::LatData::instance()->setBadEventSeqFlag();
            return 0;
        } else {
            eventSeqNum = ldfReader::LatData::instance()->summaryData().eventSequence();
        }

       // reset the stored event sequence number when we hit the LDF's rollover
       // value
       if (eventSeqNum == maxEventSeqNum) eventSeqNum = -1;

     
        return 0;
    }

    bool LdfParser::setDebug(bool on) {
        return EbfDebug::setDebug(on);
    }

    bool LdfParser::end() {
        return (m_datagram < m_end);
    }

}
#endif
