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

const unsigned ErrParser::m_gccc[]  = {0, 2, 3, 1};
const unsigned ErrParser::m_gtcc[]  = {3, 6, 2, 7, 0, 5, 1, 4};
const char* ErrParser::m_pn[]       = {"+", "-"};
const char* ErrParser::m_xy[]       = {"X", "Y"};
const unsigned ErrParser::m_layer[] = {0, 2, 2, 0};


int ErrParser::summary(ErrorSummary theError)
{
    // Skip this contribution if there was a packet error
    if ( (EBFcontribution*)contribution()->packetError()) {
        fprintf(stream(),
             "Packet Error found in TEM - during error processing - skipping");
        _handleErrorCommon();
        return 0;
    }

    ldfReader::ErrData err;
    err.initLength(((EBFcontribution*)contribution())->length());
    //err.initPacketError(((EBFcontribution*)contribution)->packetError());

    unsigned int towerId = LATPcellHeader::source(contribution()->header());
    LatData* curLatData = LatData::instance();
    TowerData* tower = curLatData->getTower(towerId);
    if (!tower) {
        tower = new TowerData(towerId);
        curLatData->addTower(tower);
    }

    ldfReader::TemData& tem = tower->getTem();
    if (!tem.exist()) {
        printf("WARNING TemData does not exist for tower %d creating now\n", towerId);
       ldfReader::EventSummaryCommon summary(((EBFcontribution*)contribution())->summary());
        //ldfReader::TemData temNew(summary);
        tem.initSummary(summary);
        tem.setExist();
        tem.initPacketError(((EBFcontribution*)contribution())->packetError());
        tem.initLength(((EBFcontribution*)contribution())->length());
        //tower->setTem(temNew);
    }

    tower->getTem().getErr()->setExist();

    tower->getTem().getErr()->initCal(theError.cal());
    tower->getTem().getErr()->initTkr(theError.tkr());
    tower->getTem().getErr()->initPhs(theError.phs());
    tower->getTem().getErr()->initTmo(theError.tmo());

    tower->getTem().getErr()->initLength(ERRcontributionIterator::size());

    return 0;
}


int ErrParser::gaemTMOerror(unsigned cable) {
  fprintf(stream(), "ErrParser:"
                    "No start bit seen => cable timeout for cable %d\n",
          cable);

  _handleErrorCommon();

  return 0;
}

int ErrParser::gaemHDRParityError(unsigned cable) {

  fprintf(stream(), "ErrParser:"
                    "Header parity error for cable %d\n",
          cable);
  _handleErrorCommon();

  return 0;
}


int ErrParser::gaemPHAParityError(unsigned cable, unsigned channel) {
  fprintf(stream(), "ErrParser:"
                    "PHA parity error for cable %d, channel %d\n",
          cable, channel);

  _handleErrorCommon();

  return 0;
}



int ErrParser::gcccError(unsigned tower, unsigned gccc, GCCCerror err)
{
  ldfReader::LatData::instance()->setGcccErrorFlag();

  const char* msg = (err.type() == 0) ? "Missing start bit(s)" : "Parity error(s)";
  fprintf(stream(),
          "%s    %s: Tower %d, GCCC %d, sweep %d (raw: 0x%04x):",
          m_prefix, msg, tower, gccc, err.sweep(), err.raw());
  unsigned i;
  for (i=0; i<8; i++)
  {
    if (err.param() & (1 << i))
    {
      fprintf(stream(), "%s      %s%s%d, wire %d\n", m_prefix,
              m_pn[i > 3],
              m_xy[gccc & 1],
              m_layer[gccc] + ((i >> 1) & 1),
              i & 1);
    }
  }
  _handleErrorCommon();

  return 0;
}

int ErrParser::gtccError(unsigned tower, unsigned gtcc, GTCCerror err)
{
  ldfReader::LatData::instance()->setGtccErrorFlag();

  fprintf(stream(), "%s  Tower %d, GTCC %d, GTRC %d (raw: 0x%04x):",
          m_prefix, tower, gtcc, err.GTRC(), err.raw());
  _handleErrorCommon();

  return 0;
}

int ErrParser::phaseError(unsigned tower, unsigned short err)
{
  ldfReader::LatData::instance()->setPhaseErrorFlag();

  fprintf(stream(), "%s  Cable Controller phasing error: Tower %d, tags (raw: 0x%04x):",
         m_prefix, tower, err);

  unsigned tag[] = {0, 0, 0, 0, 0, 0, 0, 0};
  unsigned i;
  for (i=0; i<8; i++)
  {
    tag[m_gtcc[i]] = (err >> (i << 1)) & 0x0003;
  }
  fprintf(stream(), "%s    GTCC:  0  1  2  3  4  5  6  7\n" , m_prefix);
  fprintf(stream(), "%s     tag:", m_prefix);
  for (i=0; i<8; i++)
  {
    fprintf(stream(), "  %d", tag[i]);
  }
  fprintf(stream(), "\n");
  _handleErrorCommon();
  return 0;
}

int ErrParser::timeoutError(unsigned tower, unsigned short err)
{
  ldfReader::LatData::instance()->setTimeoutErrorFlag();

  fprintf(stream(), "%s  Cable Controller timeouts: Tower %d (raw: 0x%03x):\n",
          m_prefix, tower, err);
  unsigned short cTmos = err & 0x000f;
  fprintf(stream(), "%s    GCCC:", m_prefix);
  unsigned i;
  for (i=0; i<4; i++)
  {
    if (cTmos & (1 << i))  fprintf(stream(), " %d", m_gccc[i]);
  }
  unsigned short tTmos = (err >> 4) & 0x00ff;
  fprintf(stream(), ", GTCC:");
  for (i=0; i<8; i++)
  {
    if (tTmos & (1 << i))  fprintf(stream()," %d", m_gtcc[i]);
  }
  fprintf(stream(), "\n");
  _handleErrorCommon();
  return 0;
}

int ErrParser::gtrcPhaseError (unsigned  tower,
                               unsigned  gtcc,
                               unsigned  gtrc,
                               GTRCerror err)
{
  ldfReader::LatData::instance()->setGtrcPhaseFlag();

  fprintf(stream(), "%s    GTRC phasing error: "
          "Tower %d, GTCC %d, GTRC %d (raw: 0x%04x)\n", m_prefix,
          tower, gtcc, gtrc, err.raw());
  fprintf(stream(),"%s      Current tag number:           %d",
         m_prefix, err.currentTag());
  fprintf(stream(),"%s      Layer 0 tag number:           %d\n",
         m_prefix, err.layer0Tag());
  fprintf(stream(),"%s      Expected layer number:        %d\n",
         m_prefix, err.expectedLayer());
  fprintf(stream(),"%s      Layer number received by TEM: %d\n",
         m_prefix, err.receivedLayer());
  _handleErrorCommon();
  return 0;
}

int ErrParser::gtfePhaseError (unsigned       tower,
                               unsigned       gtcc,
                               unsigned       gtrc,
                               unsigned short tag0,
                               unsigned short tag1,
                               unsigned short tag2,
                               unsigned short tag3,
                               unsigned short tag4)
{
  ldfReader::LatData::instance()->setGtfePhaseFlag();

  fprintf(stream(), "%s    GTFE phasing error: "
          "Tower %d, GTCC %d, GTRC %d, GTFE tag words (raw: ", m_prefix,
          tower, gtcc, gtrc);
  fprintf(stream(), "0x%03x 0x%03x 0x%03x 0x%03x 0x%03x):\n",
          tag0, tag1, tag2, tag3, tag4);
  fprintf(stream(), "%s      GTFE  0  1  2  3  4  5  6  7  8  9 10 11\n",
          m_prefix);
  fprintf(stream(), "%s      0-11 ", m_prefix);
  unsigned tag[] = {tag0, tag1, tag2, tag3, tag4};
  unsigned tags[25];
  unsigned i;
  unsigned j;
  for (i = 0; i < 5; i++)
  {
    for (j = 0; j < 5; j++)
    {
      tags[5*i + j] = tag[i] & 0x3;
      tag[i] >>= 2;
    }
  }
  for (i = 0; i < 12; i++)
    fprintf(stream(), "%2d ", tags[i]);
  fprintf(stream(), "\n%s     12-23 ", m_prefix);
  for (i = 12; i < 24; i++)
    fprintf(stream(), "%2d ", tags[i]);
  _handleErrorCommon();
  return 0;
}

int ErrParser::gtccFIFOerror  (unsigned tower,
                               unsigned gtcc,
                               unsigned gtrc,
                               unsigned short err)
{
  ldfReader::LatData::instance()->setGtccFifoFlag();
  ldfReader::LatData::instance()->getTower(tower)->getTem().getErr()->setTkrFifoFull(gtcc);

  fprintf(stream(),"%s    FIFO full error: ",
          "Tower %d, GTCC %d, GTRC %d, projected word count = 0x%04x\n",
          m_prefix, gtcc, gtrc, err);
  _handleErrorCommon();
  return 0;
}

int ErrParser::gtccTMOerror        (unsigned tower,
                                    unsigned gtcc,
                                    unsigned gtrc)
{
  ldfReader::LatData::instance()->setGtccTimeoutFlag();

  fprintf(stream(),"%s    Cable timeout error: "
          "Tower %d, GTCC %d, GTRC %d\n", m_prefix, tower, gtcc, gtrc);
  _handleErrorCommon();
  return 0;
}

int ErrParser::gtccHDRParityError  (unsigned tower,
                                    unsigned gtcc,
                                    unsigned gtrc)
{
  ldfReader::LatData::instance()->setGtccHdrParityFlag();

  fprintf(stream(),"%s    Header parity error: "
          "Tower %d, GTCC %d, GTRC %d\n", m_prefix, tower, gtcc, gtrc);
  _handleErrorCommon();
  return 0;
}

int ErrParser::gtccWCParityError   (unsigned tower,
                                    unsigned gtcc,
                                    unsigned gtrc)
{
  ldfReader::LatData::instance()->setGtccWcParityFlag();

  fprintf(stream(),"%s    Word count parity error: "
          "Tower %d, GTCC %d, GTRC %d\n", m_prefix, tower, gtcc, gtrc);
  _handleErrorCommon();
  return 0;
}

int ErrParser::gtrcSummaryError    (unsigned tower,
                                    unsigned gtcc,
                                    unsigned gtrc)
{
  ldfReader::LatData::instance()->setGtrcSummaryFlag();

  fprintf(stream(),"%s    GTRC summary error: "
          "Tower %d, GTCC %d, GTRC %d\n", m_prefix, tower, gtcc, gtrc);
  _handleErrorCommon();
  return 0;
}

int ErrParser::gtccDataParityError (unsigned tower,
                                    unsigned gtcc,
                                    unsigned gtrc)
{
  ldfReader::LatData::instance()->setGtccDataParityFlag();

  fprintf(stream(),"%s    Data parity error: "
          "Tower %d, GTCC %d, GTRC %d\n", m_prefix, tower, gtcc, gtrc);
  _handleErrorCommon();
  return 0;
}

int ErrParser::handleError(const TEMcontribution* contribution,
                           unsigned code, unsigned p1, unsigned p2) const
{
  switch (code) {
      case ERR_TEMbug:
      {
          ldfReader::LatData::instance()->setTemBugFlag();
          break;
      }
      default:
          break;
  } // end switch (code)

  return 0;  //rc
}



} // end namespace ldfReader

#endif
