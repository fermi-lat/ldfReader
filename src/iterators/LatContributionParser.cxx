#ifndef ldfReader_LATCONTRIBUTIONPARSER_CXX
#define ldfReader_LATCONTRIBUTIONPARSER_CXX 1

/** @file LatContributionParser.cxx
@brief Implementation of the LatContributionParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/LatContributionParser.cxx,v 1.4 2006/04/07 16:46:49 heather Exp $
*/

//#include <stdio.h> // included for LATcomponentIterator.h in Online/EBF
#include "LatContributionParser.h"
#include "../EbfDebug.h"

namespace ldfReader {

enum { Pri_Id_BtAncEvt = Pri_Base_OEF + 0x030 }; /*! Beam Test Ancillary Event */
enum { Pri_Id_BtAncHdr = Pri_Base_OEF + 0x040 }; /*! Beam Test Ancillary Header */
enum { Pri_Id_BtAncTlr = Pri_Base_OEF + 0x050 }; /*! Beam Test Ancillary Trailer */

enum { Sec_Id_BtAncEvt = 0x12c               }; /*! Beamtest Ancillary Event  */
enum { Sec_Id_BtAncHdr = 0x0c8               }; /*! Beamtest Ancillary Header */
enum { Sec_Id_BtAncTlr = 0x064               }; /*! Beamtest Ancillary Trailer*/



    int LatContributionParser::EBF(EBFevent* event, EBFevent* end) {
        // Iterate over a list of EBF events
        m_eep.iterate(event, end);

        return m_eep.status();
    }

    int LatContributionParser::UDF(LATcontribution* event, LATcontribution* end)
   {
 if (event->identity().getPrimary() == Pri_Id_BtAncEvt) {
    if (EbfDebug::getDebug()) 
        printf("Found BtAncEvt, 0x%08X\n", event->identity().value());
    unsigned char* buf = event->string(false);
    int status = m_adf.parseEvent(buf);

    //unsigned int *h = &((unsigned int*)buf)[0];
    //unsigned int ver =(h[0] >> 20) & 0xfff;
    //unsigned int evtLen = h[1];
    //printf("ver: 0x%08X %u, len: 0x%08X %u\n", ver, ver, h[1], h[1]);
    //unsigned int evtNum = h[2] & 0xfffffff;
    //unsigned int spillNum = (h[3] >> 12) & 0xffff;
    //unsigned int spillSize = h[3] & 0xfff;
    //printf("evtNum: %u, spillNum %u, spillSize: %u\n", evtNum, spillNum, spillSize);

    //unsigned int remainingBytes = len - 16;

  } else if (event->identity().getPrimary() == Pri_Id_BtAncHdr) {
      if (EbfDebug::getDebug()) 
          printf("Found BtAncHdr, 0x%08X\n", event->identity().value());
      unsigned char* buf = event->string(false);
      int status = m_adf.parseHeader(buf);

      //unsigned int *h = &((unsigned int*)buf)[0];
      //unsigned int ver =(h[0] >> 20) & 0xfff;
      //printf("ver: 0x%08X %u, len: 0x%08X %u\n", ver, ver, h[1], h[1]);
      //unsigned int runNum = h[2];
      //printf("runNum 0x%08X %u\n", runNum, runNum);

  } else if (event->identity().getPrimary() == Pri_Id_BtAncTlr) {
      if (EbfDebug::getDebug()) 
          printf("Found BtAncTlr, 0x%08X\n", event->identity().value());
      unsigned char* buf = event->string(false);
      int status = m_adf.parseTrailer(buf);
  } else {
      fprintf(stderr, "LatContributionParser::UDF: "
          "Found unrecognized LATdatagram contribution type 0x%08X\n",
          event->identity().value());
  }


        return 0;

  }

int LatContributionParser::handleError(LATcontribution* contribution,
                               unsigned code, unsigned p1, unsigned p2) const
{
  switch (code)
  {
    case LATcontributionIterator::ERR_UDFcontribution:
    {
      fprintf(stderr, "LATcontributionIterator::UDF: "
        "Found unrecognized LATdatagram contribution type 0x%08X\n", p1);
      fprintf(stderr, " Event: %llu Apid: %d\n", 
         ldfReader::LatData::instance()->eventId(), 
         ldfReader::LatData::instance()->getCcsds().getApid());
      return -1;
      break;
    }
    default: break;
  }
  return 0;
}


}
#endif
