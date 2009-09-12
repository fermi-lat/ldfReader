#ifndef ldfReader_LdfParser_CXX
#define ldfReader_LdfParser_CXX


/** @file LdfParser.cxx
@brief Implementation of the LdfParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/LdfParser.cxx,v 1.37 2008/12/03 15:42:59 heather Exp $
*/

#include "ldfReader/LdfParser.h"
#include "EbfDebug.h"
#include "ldfReader/LdfException.h"
#include <exception>
#include <sstream>
#include <iostream>
#include <math.h> // for floor
#include <csignal> // For signal handling
#include <errno.h>

#include "facilities/Timestamp.h"
#include "astro/JulianDate.h"

#include "fitsio.h" 
// so that we can use longer, more descriptive names for FITS routines
#include "longnam.h"

namespace ldfReader {

const unsigned LdfParser::BufferSize = 64*1024;

    LdfParser::LdfParser() {
        clear();
    }

    LdfParser::LdfParser(const std::string &fileName, bool fitsWrap, 
        const std::string& instrument) :
    m_fileName(fileName), m_fitsWrap(fitsWrap), m_ebf(0), m_fitsfile(0), 
        m_maxRow(0), m_currentRow(0), m_maxHdu(0), m_currentHdu(0),
        m_rowBuf(0), m_maxSize(0), m_evtCount(0), m_instrument(instrument),
        m_runId(0), m_eventSize(0), m_dataParser()
    {
        try {
            // MyTkrIterator::setInstrument(instrument);
            if (fitsWrap) {
                fitsfile* ffile;
                int status = 0;
                // Do fits open on file (ffopen)
                fits_open_file(&ffile, fileName.c_str(), READONLY,
                    &status);
                if (status != 0)
                   throw LdfException("Failed to open FITS file)");
                m_fitsfile = (void *) ffile;
                fits_get_num_hdus(ffile, &m_maxHdu, &status); // ffthdu

                if ((status != 0) || (m_maxHdu < 2)) {
                    ;  // complain
                    throw LdfException("FITS HDUs");
                }

                // Move to Primary Header
                int hduType;
                m_currentHdu = 1;
                fits_movabs_hdu(ffile, m_currentHdu, &hduType, &status); // ffmahd

                if (status != 0) 
                    throw LdfException("Could not move to primary HDU");

                // Get the RunId from the Primary Header
                int numKeys, moreKeys;
                fits_get_hdrspace(ffile, &numKeys, &moreKeys, &status);

                int iKey;
                char* comment=0;
                char keyName[15], value[50];
                for (iKey = 1; iKey <= numKeys; iKey++) {
                    fits_read_keyn(ffile, iKey, keyName, value, comment, &status);
                    if (status == 0) {
                        std::string runIdStr(value);
                        std::string tempStr("'RunId   '");
                        if (runIdStr == tempStr) {
                            fits_read_keyn(ffile, iKey+1, keyName, value, comment, &status);

                            if (status != 0) { 
                                printf("Failed to retrieve RunId, setting to zero\n");
                                m_runId = 0;
                            } else {
                                fits_read_key(ffile, TULONG, keyName, &m_runId, comment, &status);
                                if (status != 0) {
                                    printf("Failed to retrieve RunId, setting to zero\n");      
                                    m_runId = 0;
                                }
 

                            }
                            break;
                        }
                    } else {
                        status = 0;
                    }
                 }


                // put us back at the begining
                fits_read_keyn(ffile, 0, keyName, value, comment, &status);

                printf("RunId = %u\n", m_runId);

                // Go to 2nd HDU
                m_currentHdu = 2;
                fits_movabs_hdu(ffile, m_currentHdu, &hduType, &status); // ffmahd
                if ((status != 0) || (hduType != BINARY_TBL)) {
                    ;  // complain
                    throw LdfException("Unable to retrieve Binary Table in FITS");
                }
                // Find out total # of rows; initialize counter
                fits_get_num_rows(ffile, &m_maxRow, &status);   // ffgnrw
                if (status != 0) {
                    ;  // complain
                    throw LdfException("Unable to get rows from FITS");
                }

                // Get value of keyword "EVTCOUNT".  The 4th argument could be
                // a pointer to char if we cared about the comment string.
                fits_read_key_lng(ffile, "EVTCOUNT", &m_evtCount, 0, &status);
                if (m_evtCount <= 0) throw LdfException("Zero or Negative event count in FITS File");


                // Believe that first row in FITS table is called 1.
                m_currentRow = 1;

                m_eventSize = getRow();

            }
            else  { // For LDF or ARCH files
                m_ebf  = file_initialize(m_fileName.c_str());

                static unsigned char buffer[BufferSize];
                bool swap;
                unsigned size = from_file(m_ebf, buffer, &swap);
                if (size <= 0) throw LdfException("Could not read from LDF file");
                m_eventSize = size;
                m_swap = swap;

                m_maxSize = sizeof(buffer);
                m_rowBuf = buffer;

            }


        } catch( LdfException& e) {
            std::cerr << "Caught LdfException: " << e.what() << std::endl;
            throw;
        } catch(...) {
            std::cerr << "Error setting up LdfParser" << std::endl;
            throw;
        }
    }


    LdfParser::~LdfParser() {
        if (!m_fitsWrap) file_finalize(m_ebf);
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
        //if (m_ebf) EBF_free(m_ebf);
        //m_ebf = 0;
        //m_evt = 0;
    }

    long LdfParser::getRow() {
       try {
        int status = 0;
        //  get next row length and starting address
        long nbytes;
        long heapAddr;

        fitsfile* ffile = (fitsfile*) m_fitsfile;

        fits_read_descript(ffile, 1, m_currentRow, &nbytes,     // ffgdes
            &heapAddr, &status);

        if (status != 0) {
            printf("FITSIO Status Code: %d curRow = %ld\n", status, m_currentRow);
            throw LdfException("Failed to read row description");
        }

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
        fits_read_col_byt(ffile, 1, m_currentRow, 1, nbytes,    
                          0, m_rowBuf, 0, &status);
        m_currentRow++;

        //  pass to EBF stuff, letting it swap if necessary
        if (status != 0) {
            ; //complain
            return 0;    // no bytes read
        }

        // This will do swapping if needed on event data.

        return nbytes;
        } catch( LdfException& e) {
            std::cerr << "Caught LdfException: " << e.what() << std::endl;
            throw;
        } catch(...) {
            std::cerr << "Unknown exception in LdfParser::getRow" << std::endl;
            throw;
        }
    }


    int LdfParser::nextEvent() {
      try {
        if (m_fitsWrap) {
            int status = 0;
            int hduType;
            fitsfile* ffile = (fitsfile *) m_fitsfile;

            if (m_currentRow > m_evtCount) {
                if (m_currentHdu == m_maxHdu) {
                    // end of file
                    //m_datagram = 0;
                    return -1;
                }
                bool DONE = false;
                while ( (m_currentHdu < m_maxHdu) && (!DONE) ) {
                    // Otherwise move on to next HDU
                    m_currentHdu++;
                    fits_movrel_hdu(ffile, 1, &hduType, &status);   // ffmrhed
                    if (status != 0) {
                        //m_datagram = 0;

                        // complain
                       return -1; 
                    } else { // check that this HDU is event data
                        char name[50];
                        fits_read_key_str(ffile, "EXTNAME", name, 0, &status);
                        if (strcmp("Event Data", name) == 0) {
                           DONE = true;
                        }
                    }
                }
                if (!DONE) {
                    printf("No more Event Data HDUs found\n");
                    return -1;
                }

                fits_get_num_rows(ffile, &m_maxRow, &status);  // ffgnrw
                if (status != 0) {
                    // complain
                    return -1;
                }

                // Get value of keyword "EVTCOUNT".  The 4th argument could be
                // a pointer to char if we cared about the comment string.
                fits_read_key_lng(ffile, "EVTCOUNT", &m_evtCount, 0, &status);

                m_currentRow = 1;
            }
            long nbytes = getRow();
            m_eventSize = nbytes;
            // nbytes should never be 0, but just in case, leave this check in
            return (nbytes > 0) ? 0 : -1;
        }
        else {  // raw ldf
            bool swap;
            unsigned size = from_file(m_ebf, m_rowBuf, &swap);
            m_eventSize = size;
            if (size <= 0) {
              return -1;
            }
            m_swap = swap;
          
            return 0;
        }
      } catch(LdfException &e) {

        std::cerr << "LdfException caught: " << e.what() << std::endl;
        throw;
      } catch(...) {

        std::cerr << "Unknown Exception caught " << std::endl;
        throw;
      }

    }

    int LdfParser::loadData() {
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

        // Parse the data buffer
        //m_dataParser.iterate(m_rowBuf, m_maxSize);

        // Parse the data buffer
        unsigned int count = m_dataParser.iterate2(m_rowBuf, m_maxSize, m_swap);

        EBFeventIterator *ebfIt = dynamic_cast<EBFeventIterator*>(&m_dataParser);
        //unsigned int status = m_dataParser.status();
        unsigned int status = ebfIt->status();

        if (status) {
            std::ostringstream errMsg;
            errMsg.str("LDF dataParser reported a bad status 0x");
            errMsg << std::hex << status << " = " << std::dec
                   << status << " EventId: "
                   << ldfReader::LatData::instance()->getOsw().evtSequence();
            std::cout << errMsg << std::endl;
            ldfReader::LatData::instance()->setBadLdfStatusFlag();
            throw LdfException("LDF dataParser reported a bad status");

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



    FILE* LdfParser::file_initialize(const char* filename) {
        FILE* fpevents = fopen(filename, "rb");
        if (!fpevents) {
            fprintf(stderr, "*** LatSimOes: cannot open input data file '%s': %s\n", filename, strerror(errno));
        } else {
            fprintf(stdout, "LatSimOes: opening input data file '%s'\n", filename);
        }

        return fpevents;
    }



unsigned LdfParser::from_file(FILE* fpevents, unsigned char* buffer, bool* swap)
                          //bool* swap = 0, IteratePFI* iterate = 0)
{
  static const unsigned IntroWords   = 2;
  static const unsigned InitialWords = 6;
  unsigned   size = 0;
  unsigned   id;
  unsigned   ids;

  if (fpevents) {
    size_t n = fread(buffer, sizeof(int), IntroWords, fpevents);
    if (n == IntroWords) {
      id  = *evtId(buffer);
      ids = EBF_swap32_lclXbig(id);
      if      (id  == EBFevent::ID)
      {
        if (swap)     *swap    = false;
        //if (iterate)  *iterate = iterateEBF;
        size                   = *evtsize(buffer) & 0xffff;
      }
      else if (ids == EBFevent::ID)
      {
        if (swap)     *swap    = true;
        //if (iterate)  *iterate = iterateEBF;
        size                   = EBF_swap32_lclXbig(*evtsize(buffer)) & 0xffff;
      }
      else if (id  == LATdatagram::ID)
      {
        if (swap)     *swap    = false;
        //if (iterate)  *iterate = iterateLDF;
        size                   = *evtsize(buffer);
      }
      else if (ids == LATdatagram::ID)
      {
        if (swap)     *swap    = true;
        //if (iterate)  *iterate = iterateLDF;
        size                   = EBF_swap32_lclXbig(*evtsize(buffer));
      }
      else
      {
        if (swap)     *swap    = !DFC_BIG_ENDIAN;
        //if (iterate)  *iterate = iterateLDF;
        size                   = (InitialWords - IntroWords) << 2;
      }
      if (size) {
        if ((size & 0x3) == 0) {
          unsigned remaining = (size>>2)-IntroWords;
          n = fread(evtremaining(buffer), sizeof(int), remaining, fpevents);
          if (n != remaining) {
            fprintf(stderr, "*** LDFdump: EOF found while reading\n");
            size = 0;
          }
        } else {
          fprintf(stderr,"*** LDFdump: datagram size %d not word aligned\n",
                  size);
          size = 0;
        }
      } else {
        fprintf(stderr, "*** LDFdump: illegal zero datagram size\n");
        size = 0;
      }
    } else {
      fprintf(stdout, "LDFdump: reached EOF\n");
    }
  }
  return size;
}




/* OLD version
    unsigned LdfParser::from_file(FILE* fpevents, unsigned char* buffer) {
        static const unsigned IntroWords = 2;
        unsigned size = 0;
        if (fpevents) {
            size_t n = fread(buffer, sizeof(int), IntroWords, fpevents);
            if (n == IntroWords) {
                size = EBF_swap32(*evtsize(buffer));
                if (size) {
                    if ((size & 0x3) == 0) {
                        unsigned remaining = (size>>2)-IntroWords;
                        n = fread(evtremaining(buffer), sizeof(int), remaining, fpevents);
                        if (n != remaining) {
                            fprintf(stderr, "*** LDFdump: EOF found while reading\n");
                            size = 0;
                        }
                    } else {
                         fprintf(stderr,"*** LDFdump: datagram size %d not word aligned\n", size);
                         size = 0;
                     }
                 } else {
                     fprintf(stderr, "*** LDFdump: illegal zero datagram size\n");
                     size = 0;
                 }
            } else {
                fprintf(stdout, "LDFdump: reached EOF\n");
            }
         }

         if (!DFC_BIG_ENDIAN)
         EBF_swap32_lclXbigN((unsigned*)buffer, size / sizeof (unsigned));

         return size;
    }
*/

    void LdfParser::file_finalize(FILE* fpevents) {
        if (fpevents) {
            fprintf(stdout, "LDFdump: closing input data file\n");
            fclose(fpevents);
        }
    }


unsigned* LdfParser::evtsize(unsigned char* buffer)
{
  static const unsigned SizePos = 1<<2;
  return (unsigned*)(buffer+SizePos);
}

unsigned* LdfParser::evtId(unsigned char* buffer) {
  static const unsigned IdPos = 0<<2;
  return (unsigned*)(buffer+IdPos);
}

unsigned* LdfParser::evtremaining(unsigned char* buffer)
{
  static const unsigned EvtRemaining = 2<<2;
  return (unsigned*)(buffer+EvtRemaining);
}

double LdfParser::timeForTds() {
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


void LdfParser::ignoreSegFault(bool value)  {
    // Purpose and Method:  Allows Client to ignore segmentation faults
    // Not using POSIX for now
    if (value)
        signal(SIGSEGV, SIG_IGN);
    else
        signal(SIGSEGV, SIG_DFL);
}

} //namespace
#endif
