#ifndef ldfReader_ErrParser_CXX
#define ldfReader_ErrParser_CXX 1

/** @file ErrParser.cxx
@brief Implementation of the ErrParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/CalParser.cxx,v 1.
7 2006/04/07 16:46:49 heather Exp $
*/

#include <stdio.h>
#include <assert.h>

// ldfReader includes
#include "ErrParser.h"
#include "ldfReader/data/LatData.h"
#include "ldfReader/LdfException.h"
#include "../EbfDebug.h"

namespace ldfReader {

int ErrParser::gcccError(unsigned /* tower */, unsigned gccc, GCCCerror err)
{
  ldfReader::LatData::instance()->setGcccErrorFlag();
  if (err.type() == 0) {
    fprintf(stderr, 
        "%s    GCCC %d had missing start bit(s) on sweep %d (raw: 0x%04x):",
           m_prefix, gccc, err.sweep(), err.param());
    fprintf(stderr, "  Event %llu Apid: %d\n",
        ldfReader::LatData::instance()->eventId(),
        ldfReader::LatData::instance()->getCcsds().getApid());
  } else {
    fprintf(stderr,
        "%s    GCCC %d had parity error(s) on sweep %d (raw: 0x%04x):",
           m_prefix, gccc, err.sweep(), err.param());
    fprintf(stderr, "  Event %llu Apid: %d\n",
        ldfReader::LatData::instance()->eventId(),
        ldfReader::LatData::instance()->getCcsds().getApid());
   }
  return 0;
}

int ErrParser::gtccError(unsigned /* tower */, unsigned gtcc, GTCCerror err)
{
  ldfReader::LatData::instance()->setGtccErrorFlag();
  fprintf(stderr, "%s  GTCC %d, GTRC %d (msg: 0x%04x):",
         m_prefix, gtcc, err.GTRC(), err.raw());
  fprintf(stderr, "  Event %llu Apid: %d\n",
         ldfReader::LatData::instance()->eventId(),
        ldfReader::LatData::instance()->getCcsds().getApid());
  return 0;
}

int ErrParser::phaseError(unsigned /* tower */, unsigned short err)
{
  ldfReader::LatData::instance()->setPhaseErrorFlag();
  fprintf(stderr, "\n");
  fprintf(stderr, "Event %llu Apid: %d\n",
         ldfReader::LatData::instance()->eventId(),
        ldfReader::LatData::instance()->getCcsds().getApid());
  fprintf(stderr, "%s  Cable Controller phasing error tags (raw: 0x%04x):\n",
         m_prefix, err);

  unsigned cc[]  = {3, 6, 2, 7, 0, 5, 1, 4};
  unsigned tag[] = {0, 0, 0, 0, 0, 0, 0, 0};
  unsigned i;
  for (i=0; i<8; i++)
  {
    tag[cc[i]] = (err >> (i << 1)) & 0x0003;
  }
  fprintf(stderr, "%s    GTCC:  0  1  2  3  4  5  6  7\n" , m_prefix);
  fprintf(stderr, "%s     tag:", m_prefix);
  for (i=0; i<8; i++)
  {
      fprintf(stderr, "  %d", tag[i]);
  }
  fprintf(stderr, "\n");
  return 0;
}

int ErrParser::timeoutError(unsigned /* tower */, unsigned short err)
{
  ldfReader::LatData::instance()->setTimeoutErrorFlag();
  fprintf(stderr,"\n");
  fprintf(stderr, "Event %llu Apid: %d\n",
         ldfReader::LatData::instance()->eventId(),
        ldfReader::LatData::instance()->getCcsds().getApid());
  fprintf(stderr, 
     "%s  Cable Controller timeouts (raw: 0x%03x):\n", m_prefix, err);
  unsigned short cTmos = err & 0x000f;
  fprintf(stderr, "%s    GCCC:", m_prefix);
  unsigned i;
  for (i=0; i<4; i++)
  {
    if (cTmos & (1 << i))  fprintf(stderr," %d", i);
  }
  unsigned short tTmos = (err >> 4) & 0x00ff;
  fprintf(stderr,", GTCC:");
  for (i=0; i<8; i++)
  {
    if (tTmos & (1 << i))  fprintf(stderr," %d", i);
  }
  fprintf(stderr, "\n");
  return 0;
}

int ErrParser::gtrcPhaseError (unsigned  /* tower */,
                                   unsigned  /* gtcc */,
                                   unsigned  /* gtrc */,
                                   GTRCerror err)
{
  ldfReader::LatData::instance()->setGtrcPhaseFlag();
  fprintf(stderr,"\n");
  fprintf(stderr, "Event %llu Apid: %d",
         ldfReader::LatData::instance()->eventId(),
        ldfReader::LatData::instance()->getCcsds().getApid());
  fprintf(stderr,
      "%s    GTRC phasing error (raw: 0x%04x)\n", m_prefix, err.raw());
  fprintf(stderr,"%s      Current tag number:           %d",
         m_prefix, err.currentTag());
  fprintf(stderr,"%s      Layer 0 tag number:           %d\n",
         m_prefix, err.layer0Tag());
  fprintf(stderr,"%s      Expected layer number:        %d\n",
         m_prefix, err.expectedLayer());
  fprintf(stderr,"%s      Layer number received by TEM: %d\n",
         m_prefix, err.receivedLayer());
  return 0;
}

int ErrParser::gtfePhaseError (unsigned       /* tower */,
                                   unsigned       /* gtcc */,
                                   unsigned       /* gtrc */,
                                   unsigned short err1,
                                   unsigned short err2,
                                   unsigned short err3,
                                   unsigned short err4,
                                   unsigned short err5)
{
  ldfReader::LatData::instance()->setGtfePhaseFlag();
  fprintf(stderr,"\n");
  fprintf(stderr, "Event %llu Apid: %d",
         ldfReader::LatData::instance()->eventId(),
        ldfReader::LatData::instance()->getCcsds().getApid());
  fprintf(stderr, "%s    GTFE phasing error, GTFE tags: ", m_prefix);
  fprintf(stderr, " 0x%03x 0x%03x 0x%03x 0x%03x 0x%03x", err1, err2, err3, 
          err4, err5);
  fprintf(stderr, "\n");
  return 0;
}

int ErrParser::gtccFIFOerror  (unsigned tower,
                                   unsigned gtcc,
                                   unsigned short err)
{
  ldfReader::LatData::instance()->setGtccFifoFlag();
  fprintf(stderr,"\n");
  fprintf(stderr, "Event %llu Apid: %d",
         ldfReader::LatData::instance()->eventId(),
        ldfReader::LatData::instance()->getCcsds().getApid());
  fprintf(stderr,"%s    FIFO full error, projected word count = 0x%04x\n",
         m_prefix, err);
  ldfReader::LatData::instance()->getTower(tower)->getTem().getErr()->setTkrFifoFull(gtcc);;
  return 0;
}

int ErrParser::gtccTMOerror        (unsigned /* tower */,
                                        unsigned /* gtcc */)
{
  ldfReader::LatData::instance()->setGtccTimeoutFlag();
  fprintf(stderr,"%s    Cable timeout error", m_prefix);
  fprintf(stderr, "  Event %llu Apid: %d\n",
         ldfReader::LatData::instance()->eventId(),
        ldfReader::LatData::instance()->getCcsds().getApid());
  return 0;
}

int ErrParser::gtccHDRParityError  (unsigned /* tower */,
                                        unsigned /* gtcc */)
{
  ldfReader::LatData::instance()->setGtccHdrParityFlag();
  fprintf(stderr,"%s    Header parity error", m_prefix);
  fprintf(stderr, "  Event %llu Apid: %d\n",
         ldfReader::LatData::instance()->eventId(),
        ldfReader::LatData::instance()->getCcsds().getApid());
  return 0;
}

int ErrParser::gtccWCParityError   (unsigned /* tower */,
                                        unsigned /* gtcc */)
{
  ldfReader::LatData::instance()->setGtccWcParityFlag();
  fprintf(stderr,"%s    Header parity error", m_prefix);
  fprintf(stderr,"%s    Word count parity error", m_prefix);
  fprintf(stderr, "  Event %llu Apid: %d\n",
         ldfReader::LatData::instance()->eventId(),
        ldfReader::LatData::instance()->getCcsds().getApid());
  return 0;
}

int ErrParser::gtrcSummaryError    (unsigned /* tower */,
                                        unsigned /* gtcc */)
{
  ldfReader::LatData::instance()->setGtrcSummaryFlag();
  fprintf(stderr,"%s    GTRC summary error", m_prefix);
  fprintf(stderr, "  Event %llu Apid: %d\n",
         ldfReader::LatData::instance()->eventId(),
        ldfReader::LatData::instance()->getCcsds().getApid());
  return 0;
}

int ErrParser::gtccDataParityError (unsigned /* tower */,
                                        unsigned /* gtcc */)
{
  ldfReader::LatData::instance()->setGtccDataParityFlag();
  fprintf(stderr,"%s    Data parity error\n", m_prefix);
  fprintf(stderr, "  Event %llu Apid: %d\n",
         ldfReader::LatData::instance()->eventId(),
        ldfReader::LatData::instance()->getCcsds().getApid());
  return 0;
}

} // end namespace ldfReader

#endif
