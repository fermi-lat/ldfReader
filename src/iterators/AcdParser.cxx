#ifndef AcdParser_CXX
#define AcdParser_CXX 1

/** @file AcdParser.cxx
@brief Implementation of the AcdParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/AcdParser.cxx,v 1.1 2004/04/21 22:18:54 heather Exp $
*/

// EBF Online Library includes
#include "eventSummary.h"
#include <stdio.h>
#include <assert.h>

// ldfReader includes
#include "AcdParser.h"
#include "ldfReader/data/LatData.h"
#include "ldfReader/EbfException.h"
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

void AcdParser::pha(unsigned cable, unsigned channel, ACDpha p)
{
  LATtypeId id   = event()->identity();
  // Tile number in [0,107]
  const ACDtileSide *pmt = map()->lookup(id, cable, channel);
  // A or B
  char      side = pmt->a() ? 'A' : 'B'; //map()->side(id, cable, channel) ? 'B' : 'A';
  AcdDigi::PmtSide digiSide = pmt->a() ? AcdDigi::PmtSide::A : AcdDigi::PmtSide::B;

  //printf("%s       %2d   %4s     %c    %d   0x%03x = %4d    %d     %d\n",
  //       m_prefix, channel, map()->tileName(id, tile), side,
  //       p.ADCrange(), p.ADCvalue(), p.ADCvalue(), p.parityError(), p.more());
  char *pEnd;
  unsigned int tileNum = strtol(pmt->name(), &pEnd, 0);

  // Retrieve the tower or create a new TowerData object if necessary
  ldfReader::LatData* curLatData = ldfReader::LatData::instance();
  AcdDigi* acd = curLatData->getAcd(pmt->name());
  if (!acd) {
      AcdDigi *digi = new AcdDigi(pmt->name(), tileNum);
      curLatData->addAcd(digi);
      // Now retrieve the pointer to the newly created AcdDigi
      acd = curLatData->getAcd(pmt->name());
  } 
  acd->addPmt(ldfReader::AcdDigi::AcdPmt(p.ADCvalue(), p.ADCrange(), digiSide));

}


    void AcdParser::parse()
    {
        iterate();
    }


}
#endif
