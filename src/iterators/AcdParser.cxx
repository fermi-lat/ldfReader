#ifndef AcdParser_CXX
#define AcdParser_CXX 1

/** @file AcdParser.cxx
@brief Implementation of the AcdParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/AcdParser.cxx,v 1.8 2004/10/27 21:25:08 heather Exp $
*/

// EBF Online Library includes
#include "eventSummary.h"
#include <stdio.h>
#include <assert.h>

// ldfReader includes
#include "AcdParser.h"
#include "ldfReader/data/LatData.h"
#include "ldfReader/LdfException.h"
#include "../EbfDebug.h"

namespace {
}   // end default namespace

namespace ldfReader {
    AcdParser::AcdParser(EBFevent*        event,
                             AEMcontribution* contribution,
                             const char*      prefix):
  AEMcontributionIterator(event, contribution),
  m_prefix(prefix)
{
}

void AcdParser::header(unsigned cable, AEMheader hdr)
{
    // Store this header for use when parsing each PMT 
    setCurHeader(hdr);

    if (EbfDebug::getDebug()) {

        printf("%s  Cable %d = FREE board %s header:\n", m_prefix, cable,
            map()->freeName(event()->identity(), cable));
        printf("%s    Start-bit           = %01x\n",     m_prefix, hdr.startBit());
        printf("%s    Hit-map             = 0x%05x\n",   m_prefix, hdr.hitMap());
        printf("%s    Accept-map          = 0x%05x\n",   m_prefix, hdr.acceptMap());
        printf("%s    PHA vector          = %01x\n",     m_prefix, hdr.phaVector());
        printf("%s    Header parity error = %01x\n",     m_prefix, hdr.parityError());
        printf("%s    End of Cables       = %01x\n",     m_prefix, hdr.endOfCables());
        printf("%s    Cable Number        = %02x\n",     m_prefix, hdr.cableNumber());
        if (hdr.phaVector())
        {
            printf("%s                                  Value    parity\n",      m_prefix);
            printf("%s    Channel  Tile  Side Range (hex)  (dec) error  More\n", m_prefix);
        }
        else
            printf("%s  Cable %d has no data\n", m_prefix, cable);
    }
}

void AcdParser::pha(unsigned cable, unsigned channel, ACDpha p)
{
  // making an assumption that header is set before the pha calls
  AEMheader curHeader = getCurHeader();
  if (cable != curHeader.cableNumber()) {
      printf("cable for PHA doesn't match cable for header\n");
  }
  unsigned hitMap = curHeader.hitMap();
  bool veto = (hitMap >> channel) & 1;
  unsigned acceptMap = curHeader.acceptMap();
  bool accept = (acceptMap >> channel) & 1;

  LATtypeId id   = event()->identity();
  // Tile number in [0,107]
  const ACDtileSide *pmt = map()->lookup(id, cable, channel);
  // A or B
  char      side = pmt->a() ? 'A' : 'B'; //map()->side(id, cable, channel) ? 'B' : 'A';
  AcdDigi::PmtSide digiSide = pmt->a() ? AcdDigi::A : AcdDigi::B;

  //printf("%s       %2d   %4s     %c    %d   0x%03x = %4d    %d     %d\n",
  //       m_prefix, channel, map()->tileName(id, tile), side,
  //       p.ADCrange(), p.ADCvalue(), p.ADCvalue(), p.parityError(), p.more());
  char *pEnd;
  unsigned int tileNum = strtol(pmt->name(), &pEnd, 10);

  // Retrieve the tower or create a new TowerData object if necessary
  ldfReader::LatData* curLatData = ldfReader::LatData::instance();
  AcdDigi* acd = curLatData->getAcd(pmt->name());
  if (!acd) {
      AcdDigi *digi = new AcdDigi(pmt->name(), pmt->tile(), tileNum);
      curLatData->addAcd(digi);
      // Now retrieve the pointer to the newly created AcdDigi
      acd = curLatData->getAcd(pmt->name());
  } 
  AcdDigi::ParityError err = (p.parityError() == 0) ? AcdDigi::NOERROR : AcdDigi::ERROR;
  acd->addPmt(ldfReader::AcdDigi::AcdPmt(p.ADCvalue(), p.ADCrange(), digiSide, channel, p.more(), veto, accept, err));

}


    void AcdParser::parse()
    {
        iterate();
    }



 int AcdParser::handleError(AEMcontribution *contribution, unsigned code,
                            unsigned p1, unsigned p2) const {
     switch (code)
     {
         case AEMcontributionIterator::ERR_TooManyPhas:
         {
             fprintf(stderr, "AEMcontributionIterator::iterate: %s",
                       "more PHA values found than bits set in the Accept Map\n");
             break;
         }

         default: break;
     }
     return 0;
 }

}
#endif
