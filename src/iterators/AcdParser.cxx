#ifndef AcdParser_CXX
#define AcdParser_CXX 1

/** @file AcdParser.cxx
@brief Implementation of the AcdParser class

$Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/ldfReader/src/iterators/AcdParser.cxx,v 1.27 2011/07/26 14:57:52 heather Exp $
*/

// EBF Online Library includes
#include "eventSummary.h"
#include <stdio.h>
#include <assert.h>

// ldfReader includes
//#include "AcdParser.h"
#include "ldfReader/iterators/AcdParser.h"
#include "ldfReader/data/LatData.h"
#include "ldfReader/LdfException.h"
#include "../EbfDebug.h"

#include <map>

namespace {
}   // end default namespace

namespace ldfReader {
    AcdParser::AcdParser( const char*      prefix) : m_prefix(prefix)
{
}

void AcdParser::header(unsigned cable, AEMheader hdr)
{
    // Store this header for use when parsing each PMT 
    setCurHeader(hdr);

    if (EbfDebug::getDebug()==EbfDebug::ALL) {

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

    ldfReader::AemData summary(contribution()->summary());
    summary.setExist();
    summary.initPacketError(contribution()->packetError());
    curLatData->setAem(summary);

    curLatData->getAem().initLength(((EBFcontribution*)contribution())->length());

//    _acdSrc = LATPcellHeader::source(contribution->header());
//    if (EbfDebug::getDebug() ) printf("\nAEM %2d:\n", _acdSrc);
    // HMK 09082008 Not sure how to call this method from EbfDataParser 
    if (EbfDebug::getDebug()==EbfDebug::ALL ) commonComponentData((EBFcontribution*)contribution());


    // loop over all bits in the accept and hit maps, if either is set store
    // a new AcdPmt and if necessary create a new AcdDigi
    unsigned int iChannel;
    for (iChannel=0; iChannel<18; iChannel++) {
        bool veto = (hitMap >> (offsetMap - iChannel)) & 1;
        bool accept = (acceptMap >> (offsetMap - iChannel)) & 1;
        if (!veto && !accept) continue;

        //LATtypeId id   = AEMcontribution::event()->identity();
        LATtypeId id   = event()->identity();
        // Tile number in [0,107]
        //const ACDtileSide *pmt = AEMcontribution::map()->lookup(id, cable, iChannel);
        const ACDtileSide *pmt = map()->lookup(id, cable, iChannel);
        char      side = pmt->a() ? 'A' : 'B';

        std::string tileName;
        if (curLatData->acdRemap()) {
            int stat = lookup(pmt->name(), tileName, side);
            if (stat < 0) {
                printf("ACD Remap failed for %s:%c\n", pmt->name(), side);
                printf("retaining original assignment\n");
                tileName = pmt->name();
                side = pmt->a() ? 'A' : 'B';
            }
        } else { // typical case with no remapping
            tileName = pmt->name();
        }

        // A or B
        AcdDigi::PmtSide digiSide = (side == 'A') ? AcdDigi::A : AcdDigi::B;
        unsigned int tileNum = constructTileNum(tileName.c_str());

        // Retrieve the tower or create a new TowerData object if necessary
        AcdDigi* acd = curLatData->getAcd(tileName.c_str());
        if (!acd) {
            AcdDigi *digi = new AcdDigi(tileName.c_str(), pmt->tile(), tileNum);
            curLatData->addAcd(digi);
            // Now retrieve the pointer to the newly created AcdDigi
            acd = curLatData->getAcd(tileName.c_str());
        }

        AcdDigi::ParityError headerParity =
            (hdr.parityError() == 0) ? AcdDigi::NOERROR : AcdDigi::ERROR;


        // Create the AcdPmt 
        acd->addPmt(ldfReader::AcdDigi::AcdPmt(digiSide, iChannel, veto, accept,
                                               headerParity));
        AcdDigi::AcdPmt* newPmt = acd->getPmtSide(digiSide);
        if(newPmt) {
           newPmt->initCable(cable);
        }
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
            const char* strptr = strpbrk(name, "0123456789");       // jrb
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
             _handleErrorCommon();
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

  if (!accept) {
    printf("Somehow managed to get into pha() without corresponding acceptMap "
           "bit being set\n(should be impossible)\n");
  }

 ldfReader::LatData* curLatData = ldfReader::LatData::instance();

  //LATtypeId id   = AEMcontribution::event()->identity();
  LATtypeId id   = event()->identity();
  // Tile number in [0,107]
  //const ACDtileSide *pmt = AEMcontribution::map()->lookup(id, cable, channel);
  const ACDtileSide *pmt = map()->lookup(id, cable, channel);

  // A or B
  char      side = pmt->a() ? 'A' : 'B'; 
  if (EbfDebug::getDebug()==EbfDebug::ALL) {
      printf("%s  %2d   %4s     %c    %d   %4d = 0x%03x    %d     %d\n",
         "PMT Info: ", channel, pmt->name(), side,
         p.ADCrange(), p.ADCvalue(), p.ADCvalue(), p.parityError(), p.more());
  }
  char sideSave = side;
  std::string tileName;
  if (curLatData->acdRemap()) {
      int stat = lookup(pmt->name(), tileName, side);
      if (stat < 0) {
            tileName = pmt->name();
            side = pmt->a() ? 'A' : 'B';
       } else if (EbfDebug::getDebug()==EbfDebug::ALL) {
           printf("Remapping %s:%c to %s:%c\n", pmt->name(), sideSave, tileName.c_str(), side);
        }
   } else { // typical case with no remapping
       tileName = pmt->name();
   }

  AcdDigi::PmtSide digiSide = (side == 'A') ? AcdDigi::A : AcdDigi::B;

  // HMK Unused? char *pEnd;

  // Retrieve the AcdDigi or create a new AcdDigi object if necessary
  AcdDigi* acd = curLatData->getAcd(tileName.c_str());
  if (!acd) {
      printf("WARNING: AEM parsing - could not find AcdDigi %s\n", pmt->name());
      unsigned int tileNum = constructTileNum(tileName.c_str());

      AcdDigi *digi = new AcdDigi(tileName.c_str(), pmt->tile(), tileNum);
      curLatData->addAcd(digi);

      // Now retrieve the pointer to the newly created AcdDigi
      acd = curLatData->getAcd(tileName.c_str());

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
          printf("WARNING:  PMT %c for %s does not exist\n", side, tileName.c_str());
          AcdDigi::ParityError headerParity = 
            (curHeader.parityError() == 0) ? AcdDigi::NOERROR : AcdDigi::ERROR;
          acd->addPmt(ldfReader::AcdDigi::AcdPmt(p.ADCvalue(), p.ADCrange(), 
                  digiSide, channel, p.more(), veto, accept, err, 
                  headerParity));
      } else {
          curPmt->initPhaValues(p.ADCvalue(),p.ADCrange(),p.more(),err);

      }
  }


  if (EbfDebug::getDebug()==EbfDebug::ALL) {
      const ldfReader::AcdDigi::AcdPmt* curPmt = acd->getPmtSide(digiSide);
      printf("name: %s, id: %u\n", acd->getTileName(), acd->getTileId());
      if (curPmt) curPmt->print(true);
  }

}


int AcdParser::gaemTMOerror(unsigned cable)
{
  fprintf(stream(), "AcdParser:"
                    "No start bit seen => cable timeout for cable %d\n",
          cable);
  return 0;
}

int AcdParser::gaemHDRParityError(unsigned cable, AEMheader /*hdr*/)
{
  fprintf(stream(), "AcdParser:"
                    "Header parity error for cable %d\n",
          cable);
  return 0;
}

int AcdParser::gaemPHAParityError(unsigned cable, unsigned channel, ACDpha /*p*/)
{
  fprintf(stream(), "AcdParser:"
                    "PHA parity error for cable %d, channel %d\n",
          cable, channel);

  return 0;
}



int AcdParser::handleError(AEMcontribution *contribution, unsigned code,
                            unsigned p1, unsigned p2) const {
    _handleErrorCommon();
    return AEMcontributionIterator::handleError(contribution, code, p1, p2);
}


int AcdParser::lookup(const char* name, std::string& newName, char &side) {

    std::map<const std::string*, const std::string*> acdMap =
        ldfReader::LatData::instance()->getAcdRemapCol();

    std::string searchStr=name;
    searchStr += ":";
    searchStr += side;

    std::map<const std::string*, const std::string*>::const_iterator acdMapIt;
    for (acdMapIt = acdMap.begin(); acdMapIt != acdMap.end(); acdMapIt++) {

        if (searchStr.compare(*(acdMapIt->first)) == 0) {
            std::string name2 = *(acdMapIt->second);
            std::string::size_type loc = name2.find(":",0);
            if (loc == std::string::npos) return -1;
            if (loc+1 >= name2.length()) return -1;
            newName = name2.substr(0,loc);
            side = name2.at(loc+1);
            return 0;
        }
    }

    return -1;


}


}
#endif
