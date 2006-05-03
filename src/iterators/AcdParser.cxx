#ifndef AcdParser_CXX
#define AcdParser_CXX 1

/** @file AcdParser.cxx
@brief Implementation of the AcdParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/AcdParser.cxx,v 1.13 2006/04/07 16:46:49 heather Exp $
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
        printf("%s    Start-bit           = %01x\n",     
                m_prefix, hdr.startBit());
        printf("%s    Hit-map             = 0x%05x\n",   
                m_prefix, hdr.hitMap());
        printf("%s    Accept-map          = 0x%05x\n",   
                m_prefix, hdr.acceptMap());
        printf("%s    PHA vector          = %01x\n",     
                m_prefix, hdr.phaVector());
        printf("%s    Header parity error = %01x\n",     
                m_prefix, hdr.parityError());
        printf("%s    End of Cables       = %01x\n",     
                m_prefix, hdr.endOfCables());
        printf("%s    Cable Number        = %02x\n",     
                m_prefix, hdr.cableNumber());
        if (hdr.phaVector())
        {
        }
        else
            printf("%s  Cable %d has no data\n", m_prefix, cable);
    }

// HMK new ACD

    if (cable != hdr.cableNumber()) {
       printf("cable passed to header routine doesn't match ");
       printf("cable in AEMheader\n");
    }
   // The bits in the hit map and accept map are numbered from left to right
   // zero - 17
    const unsigned offsetMap = 17;
    unsigned hitMap = hdr.hitMap();
    unsigned acceptMap = hdr.acceptMap();

    ldfReader::LatData* curLatData = ldfReader::LatData::instance();

    // loop over all bits in the accept and hit maps, if either is set store
    // a new AcdPmt and if necessary create a new AcdDigi
    unsigned int iChannel;
    for (iChannel=0; iChannel<18; iChannel++) {
        bool veto = (hitMap >> (offsetMap - iChannel)) & 1;
        bool accept = (acceptMap >> (offsetMap - iChannel)) & 1;
        if (!veto && !accept) continue;

        LATtypeId id   = event()->identity();
        // Tile number in [0,107]
        const ACDtileSide *pmt = map()->lookup(id, cable, iChannel);
        // A or B
        char      side = pmt->a() ? 'A' : 'B';
        AcdDigi::PmtSide digiSide = pmt->a() ? AcdDigi::A : AcdDigi::B;

        unsigned int tileNum = constructTileNum(pmt->name());

        // Retrieve the tower or create a new TowerData object if necessary
        AcdDigi* acd = curLatData->getAcd(pmt->name());
        if (!acd) {
            AcdDigi *digi = new AcdDigi(pmt->name(), pmt->tile(), tileNum);
            curLatData->addAcd(digi);
            // Now retrieve the pointer to the newly created AcdDigi
            acd = curLatData->getAcd(pmt->name());
        }

        AcdDigi::ParityError headerParity =
            (hdr.parityError() == 0) ? AcdDigi::NOERROR : AcdDigi::ERROR;


        // Create the AcdPmt 
        acd->addPmt(ldfReader::AcdDigi::AcdPmt(digiSide, iChannel, veto, accept,
                                               headerParity));
    }

// HMK new ACD end

 
}

unsigned int AcdParser::constructTileNum(const char *name) {
    // Construct a tileNum for NAs otherwise, just convert to an integer
    char *pEnd;
    unsigned int tileNum;
    if (strncmp(name, "NA", 2) == 0) {
        if ( (strlen(name) == 3) || (strlen(name) == 4) ) {
            char num[10];
            char* strptr = strpbrk(name, "0123456789");
            tileNum = 0;
            if (strptr) {
                strcpy(num, strptr );
                tileNum = 1000 + strtol(num, &pEnd, 10);
            }
         } else {
             printf("AEM Parsing:  tile name is non-standard %s ", name);
             printf(" Event: %ull, Apid: %u\n",
                 ldfReader::LatData::instance()->eventId(),
                 ldfReader::LatData::instance()->getCcsds().getApid());
             tileNum = 0;
         }

    } else
        tileNum = strtol(name, &pEnd, 10);

    return tileNum;

}

void AcdParser::pha(unsigned cable, unsigned channel, ACDpha p)
{
  // making an assumption that header is set before the pha calls
  AEMheader curHeader = getCurHeader();
  if (cable != curHeader.cableNumber()) {
      printf("cable for PHA doesn't match cable for header\n");
  }
  // The bits in the hit map and accept map are numbered from left to right
  // zero - 17
  const unsigned offsetMap = 17;
  unsigned hitMap = curHeader.hitMap();
  bool veto = (hitMap >> (offsetMap - channel)) & 1;
  unsigned acceptMap = curHeader.acceptMap();
  bool accept = (acceptMap >> (offsetMap - channel)) & 1;

  LATtypeId id   = event()->identity();
  // Tile number in [0,107]
  const ACDtileSide *pmt = map()->lookup(id, cable, channel);

  // A or B
  char      side = pmt->a() ? 'A' : 'B'; 

  AcdDigi::PmtSide digiSide = pmt->a() ? AcdDigi::A : AcdDigi::B;

  char *pEnd;

  // Retrieve the AcdDigi or create a new AcdDigi object if necessary
  ldfReader::LatData* curLatData = ldfReader::LatData::instance();
  AcdDigi* acd = curLatData->getAcd(pmt->name());
  if (!acd) {
      printf("WARNING: AEM parsing - could not find AcdDigi %s\n", pmt->name());
      unsigned int tileNum = constructTileNum(pmt->name());

      AcdDigi *digi = new AcdDigi(pmt->name(), pmt->tile(), tileNum);
      curLatData->addAcd(digi);

      // Now retrieve the pointer to the newly created AcdDigi
      acd = curLatData->getAcd(pmt->name());

      AcdDigi::ParityError err = 
            (p.parityError() == 0) ? AcdDigi::NOERROR : AcdDigi::ERROR;
      AcdDigi::ParityError headerParity = 
            (curHeader.parityError() == 0) ? AcdDigi::NOERROR : AcdDigi::ERROR;

      acd->addPmt(ldfReader::AcdDigi::AcdPmt(p.ADCvalue(), p.ADCrange(), 
                  digiSide, channel, p.more(), veto, accept, err, 
                  headerParity));
  } else {
      AcdDigi::ParityError err = 
            (p.parityError() == 0) ? AcdDigi::NOERROR : AcdDigi::ERROR;
      ldfReader::AcdDigi::AcdPmt* curPmt = acd->getPmtSide(digiSide);
      if (!curPmt) {
          printf("WARNING:  PMT %c for %s does not exist\n", side, pmt->name());
          AcdDigi::ParityError headerParity = 
            (curHeader.parityError() == 0) ? AcdDigi::NOERROR : AcdDigi::ERROR;
          acd->addPmt(ldfReader::AcdDigi::AcdPmt(p.ADCvalue(), p.ADCrange(), 
                  digiSide, channel, p.more(), veto, accept, err, 
                  headerParity));
      } else {
          curPmt->initPhaValues(p.ADCvalue(),p.ADCrange(),p.more(),err);

      }
  }


  if (EbfDebug::getDebug()) {
      const ldfReader::AcdDigi::AcdPmt* curPmt = acd->getPmtSide(digiSide);
      if (curPmt) curPmt->print(true);
  }

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
            fprintf(stderr, "%s %s %s %ull %s %d\n", 
                      "AEMcontributionIterator::iterate: ",
                      "more PHA values found than bits set in the Accept Map",
                      " Event: ", ldfReader::LatData::instance()->eventId(), 
                      " Apid: ", 
                      ldfReader::LatData::instance()->getCcsds().getApid());
             break;
        }

        default: break;
    }
    return 0;
}

}
#endif
