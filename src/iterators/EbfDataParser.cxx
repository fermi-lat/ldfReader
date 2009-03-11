#ifndef EbfDataParser_CXX
#define EbfDataParser_CXX 1

/** @class EbfDataParser.cxx
@brief Implementation of the EbfDataParser class
$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/EbfDataParser.cxx,v 1.3 2008/12/02 14:35:23 heather Exp $
*/

// ldfReader includes
#include "EbfDataParser.h"
#include "ldfReader/data/LatData.h"
#include "ldfReader/LdfException.h"
#include "../EbfDebug.h"

#include <map>

namespace {
}   // end default namespace

namespace ldfReader {

enum { Pri_Id_BtAncEvt = Pri_Base_OEF + 0x030 }; /*! Beam Test Ancillary Event */
enum { Pri_Id_BtAncHdr = Pri_Base_OEF + 0x040 }; /*! Beam Test Ancillary Header */
enum { Pri_Id_BtAncTlr = Pri_Base_OEF + 0x050 }; /*! Beam Test Ancillary Trailer */

enum { Sec_Id_BtAncEvt = 0x12c               }; /*! Beamtest Ancillary Event  */
enum { Sec_Id_BtAncHdr = 0x0c8               }; /*! Beamtest Ancillary Header */
enum { Sec_Id_BtAncTlr = 0x064               }; /*! Beamtest Ancillary Trailer*/


EbfDataParser::EbfDataParser(const char* prefix,
                             ACDmap* a, CALmap* c, TKRmap* t) :
  AcdParser(prefix),
  AdfParser(),
  CalParser(prefix),
  TkrParser(prefix),
  DiagnosticParser(prefix),
  ErrParser(prefix),
  OswParser(),
  EBFiteratorBase(a, c, t),
  m_prefix(prefix)
{
}

unsigned int EbfDataParser::iterate2(const unsigned char* buf, unsigned int len,
                                    bool swap) {

    return EBFiteratorBase::iterate(buf,len,swap);
}



int EbfDataParser::OSW_UDF(const EBFevent*        /* event */,
                             const OSWcontribution* contribution) const
{
  printf("%sUndefined OSW contribution encountered with identity %08x\n",
         m_prefix, contribution->identity().value());
  return 0;
}

int EbfDataParser::OSW_time(const EBFevent*            /*event*/,
                              const OSWtimeContribution* contribution) const
{
  const struct timespec* ts = contribution->timeStamp();
  const OSWtimeBase*     tb = contribution->timebase();
  printf("%s  OSWtime:\n", m_prefix);
  printf("%s    Event GMT timestamp = %ld.%09ld seconds after 1/1/1970\n",
         m_prefix, ts->tv_sec, ts->tv_nsec);
  printf("%s    PPC timebase        = 0x%08x%08x\n", m_prefix,
         tb->upper(), tb->lower());
  printf("%s    Extended event no.  = 0x%08x = %d\n", m_prefix,
         contribution->evtSequence(), contribution->evtSequence());

  return 0;
}



int EbfDataParser::UDF(const LATcontribution* event, const LATcontribution* end)
{
    //(LATcontribution*) eventString = const_cast<LATcontribution*>event;
  if (event->identity().getPrimary() == Pri_Id_BtAncEvt) {
    if (EbfDebug::getDebug())
        printf("Found BtAncEvt, 0x%08X\n", event->identity().value());
    const unsigned char* buf = (const_cast<LATcontribution*>(event))->string(false);
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
      const unsigned char* buf = (const_cast<LATcontribution*>(event))->string(false);
      int status = m_adf.parseHeader(buf);

      //unsigned int *h = &((unsigned int*)buf)[0];
      //unsigned int ver =(h[0] >> 20) & 0xfff;
      //printf("ver: 0x%08X %u, len: 0x%08X %u\n", ver, ver, h[1], h[1]);
      //unsigned int runNum = h[2];
      //printf("runNum 0x%08X %u\n", runNum, runNum);

  } else if (event->identity().getPrimary() == Pri_Id_BtAncTlr) {
      if (EbfDebug::getDebug())
          printf("Found BtAncTlr, 0x%08X\n", event->identity().value());
      const unsigned char* buf = (const_cast<LATcontribution*>(event))->string(false);
      int status = m_adf.parseTrailer(buf);
  } else {
      fprintf(stderr, "EbfDataParser::UDF: "
          "Found unrecognized LATdatagram contribution type 0x%08X\n",
          event->identity().value());
  }


  return 0;

}

int EbfDataParser::UDF(const EBFevent* event, const EBFcontribution* contribution)
{
    printf("Found UDF in EbfDataParser\n");
    //fprintf (stderr, "\nUndefined EBF component\n");
    //((MyEBFcontribution*)contribution)->dump("  ");
    return 0;
}

int EbfDataParser::GLT(const EBFevent* event, const EBFcontribution* ebf)
{
  const GLTcontribution *glt = (const GLTcontribution*)ebf;
  //if (EbfDebug::getDebug())  printf("\nGLT:\n");
  ldfReader::LatData::instance()->setSummary(event->summary());

  // N.B. the payload of the GEM contribution will change in the future
  //unsigned seconds     = ((unsigned*)gem->data())[0];
  //unsigned nanoSeconds = ((unsigned*)gem->data())[1];
  //ldfReader::LatData::instance()->setTime(seconds, nanoSeconds);

  // Only attempt to retrieve time if this is an older EBF file
  if (ldfReader::LatData::instance()->getFormatIdentity() < ID_WITH_OSW ) {
    const struct timespec* ts = glt->timeStamp();
    const OSWtimeBase*     tb = glt->timebase();

    ldfReader::LatData::instance()->setTime(ts->tv_sec, ts->tv_nsec);


    if (glt->payloadSize() > 8) {              // Timebase does not exist in old data
      unsigned upperPpcWord = tb->upper();//((unsigned*)gem->data())[2];
      unsigned lowerPpcWord = tb->lower();//((unsigned*)gem->data())[3];
      ldfReader::LatData::instance()->setPpcTimeBase(upperPpcWord, lowerPpcWord);
    }
  }

  return 0;
}

int EbfDataParser::GEM(const EBFevent *event, const EBFcontribution *ebf_contribution) {

    const GEMcontribution *contribution = (const GEMcontribution*) ebf_contribution;
    if (EbfDebug::getDebug())  printf("\nEbfDataParser::GEM:\n");
    ldfReader::GemData gem;

    // Set EventSummary here - in case there is no OSW
    ldfReader::LatData::instance()->setSummary(event->summary());

    ldfReader::EventSummaryCommon summary(((EBFcontribution*)contribution)->summary());
    gem.setExist();
    gem.initPacketError(contribution->packetError());

    gem.setSummary(summary);
    gem.initLength(((EBFcontribution*)contribution)->length());

    const GEMtileList *onlineTiles = contribution->tileList();
    ldfReader::GemDataTileList tileList(onlineTiles->XZM(), onlineTiles->XZP(),
               onlineTiles->YZM(), onlineTiles->YZP(), onlineTiles->XY(),
               onlineTiles->RBN(), onlineTiles->NA());
    ldfReader::GemDataOnePpsTime time(contribution->onePPStime().timebase(), contribution->onePPStime().seconds());
    gem.initTrigger(contribution->tkrVector(), contribution->roiVector(),
        contribution->calLEvector(), contribution->calHEvector(),
        contribution->cnoVector(), contribution->conditionSummary(),
        contribution->missed(), tileList);

        GemDataCondArrivalTime condArrTime;
        condArrTime.init(contribution->condArrTime().datum());
        gem.initSummary(contribution->liveTime(), contribution->prescaled(),
        contribution->discarded(), condArrTime,
        contribution->triggerTime(), time, contribution->deltaEventTime(), contribution->deltaWindowOpenTime());


    ldfReader::LatData::instance()->setGem(gem);
    return 0;
}

int EbfDataParser::commonComponentData(EBFcontribution *contribution) {
        int len = contribution->length();
        // printed in hex
        int error = contribution->packetError();
        int seqNum = contribution->sequence();
        // LCB header  printed in hex
        int header = contribution->header();
        return 0;
    }

int EbfDataParser::handleError(const EBFevent* e, unsigned  code,
                           unsigned  p1, unsigned  p2) const
{
   int rc = EBFiteratorBase::handleError(e, code, p1, p2);
   _handleErrorCommon();
   return rc;
}

int EbfDataParser::handleError(const EBFcontribution* c, unsigned code,
                           unsigned p1, unsigned p2) const
{

  ldfReader::LatData* latData = ldfReader::LatData::instance();
  switch (code) {
      case EBFcontributionIterator::ERR_TrgParityError:
      {
          latData->setTrgParityErrorFlag();
          break;
      }
      case EBFcontributionIterator::ERR_PacketError:
      {
          latData->setPacketErrorFlag();
 /* ERR_PacketError gets an additional parameter */
//      char *type;
//
//      switch (p1)
//      {
//        case EBFcontribution::Parity:      type = "parity";       break;
//        case EBFcontribution::Truncated:   type = "truncated";    break;
//        case EBFcontribution::WriteFault:  type = "write fault";  break;
//        case EBFcontribution::TimedOut:    type = "timed out";    break;
//        default:                           type = "unknown";      break;
//     }
//      fprintf(stream(),
//              "EBFcontributionIterator: For EBFcontribution at %p:\n"
//              "  Skipping contribution with source ID %d "
//              "and 'packet %s' (%d) error\n",
//              c, p2, type, p1);

          break;
      }
      default:
      {
        break;
      }
  }

  // Call base method
  int rc = EBFiteratorBase::handleError(c, code, p1, p2);
  _handleErrorCommon();
  return rc;
}

int EbfDataParser::handleError(const AEMcontribution* c, unsigned code,
                           unsigned p1, unsigned p2) const
{
  int rc = EBFiteratorBase::handleError(c, code, p1, p2);
  _handleErrorCommon();
  return rc;
}

int EbfDataParser::handleError(const TEMcontribution* c, unsigned code,
                           unsigned p1, unsigned p2) const
{
  int rc = EBFiteratorBase::handleError(c, code, p1, p2);
  _handleErrorCommon();
  return rc;
}

int EbfDataParser::handleError(const CALcontribution* c, unsigned code,
                           unsigned p1, unsigned p2) const
{
  int rc = EBFiteratorBase::handleError(c, code, p1, p2);
  _handleErrorCommon();
  return rc;
}

int EbfDataParser::handleError(const TKRcontribution* c, unsigned code,
                           unsigned p1, unsigned p2) const
{
  int rc = EBFiteratorBase::handleError(c, code, p1, p2);
  _handleErrorCommon();
  return rc;
}

void EbfDataParser::_handleErrorCommon() const
{
  ldfReader::LatData* latData = ldfReader::LatData::instance();
  unsigned long long  evtId   = ((latData->contextExists()) ?
                                 latData->eventId() :
                                 latData->getOsw().evtSequence());

  fprintf(stream(), "\n Event: %llu Apid: %d\n",
          evtId, latData->getCcsds().getApid());
}

} // end namespace

#endif
