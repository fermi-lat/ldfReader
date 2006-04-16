#ifndef LATCOMPONENTPARSER_CXX
#define LATCOMPONENTPARSER_CXX 1

/** @file LatComponentParser.cxx
@brief Implementation of the LatComponentParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/LatComponentParser.cxx,v 1.25 2006/04/07 16:46:49 heather Exp $
*/

#include <stdio.h> // included for LATcomponentIterator.h in Online/EBF
#include "LatComponentParser.h"
#include "LATp.h"
#include "GEMcontribution.h"
#include "GLTcontribution.h"
#include "CALcontribution.h"
#include "TKRcontribution.h"
#include "DiagnosticParser.h"
#include "AcdParser.h"
#include "TkrParser.h"
#include "CalParser.h"
#include "OswParser.h"
#include "ErrParser.h"
#include "ldfReader/data/LatData.h"
#include "../EbfDebug.h"

namespace ldfReader {

    int LatComponentParser::UDFcomponent(EBFevent* event, EBFcontribution* contribution)
    {
        // skip this guy if there was a packet error
        if (contribution->packetError()) {
            printf("Packet Error in UDF - skipping\n");
            return 0;
        }

        //fprintf (stderr, "\nUndefined EBF component\n");
        //((MyEBFcontribution*)contribution)->dump("  ");
        return 0;
    }

    int LatComponentParser::OSWcomponent(EBFevent*        event,
        OSWcontribution* contribution)
    {
        // If there was a packet error - skip this contribution
        if (contribution->packetError()) {
            printf("Packet Error in OSW - skipping\n");
            return 0; 
        }

        const char* prefix = "  ";

        ldfReader::LatData::instance()->setSummary(event->summary());

        ldfReader::OswData osw(ldfReader::EventSummaryCommon(((EBFcontribution*)contribution)->summary()));
        ldfReader::LatData::instance()->setOsw(osw);
        ldfReader::LatData::instance()->getOsw().initPacketError(contribution->packetError());

        ldfReader::LatData::instance()->getOsw().initLength(((EBFcontribution*)contribution)->length());
        //osw.initLength(((EBFcontribution*)contribution)->length());

        // OSW contribution only exists in later versions starting in Feb 2004
        if (ldfReader::LatData::instance()->getFormatIdentity() >= ID_WITH_OSW) {
        //    osw.setExist(); 
            ldfReader::LatData::instance()->getOsw().setExist();
            OswParser oswParse(event, contribution);
            oswParse.iterate();
        }
        return 0;
    }

    int LatComponentParser::GEMcomponent(EBFevent *event, GEMcontribution *contribution) {
        
        // If there was a packet error - skip this contribution
        if (contribution->packetError())  {
            printf("Packet Error in GEM - skipping ");
            printf("Event: %llu Apid: %d\n", 
                ldfReader::LatData::instance()->eventId(),
                ldfReader::LatData::instance()->getCcsds().getApid());
            return 0; 
        }

        //if (EbfDebug::getDebug())  printf("\nGEM:\n");
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

    int LatComponentParser::GLTcomponent(EBFevent* event, GLTcontribution* glt)
    {
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

    int LatComponentParser::ACDcomponent(EBFevent* event, AEMcontribution* contribution)
    {
        using namespace ldfReader;

        // Skip contribution if there was a packet error
        if (contribution->packetError()) {
           printf("Packet Error in ACD - skipping\n");
           printf("Event: %llu Apid: %d\n", 
                ldfReader::LatData::instance()->eventId(),
                ldfReader::LatData::instance()->getCcsds().getApid());
           return 0;
        }

        ldfReader::AemData summary(contribution->summary());
        summary.setExist();
        summary.initPacketError(contribution->packetError());
        ldfReader::LatData::instance()->setAem(summary);
 
        ldfReader::LatData::instance()->getAem().initLength(((EBFcontribution*)contribution)->length());

//        _acdSrc = LATPcellHeader::source(contribution->header());
//        if (EbfDebug::getDebug() ) printf("\nAEM %2d:\n", _acdSrc);
        if (EbfDebug::getDebug() ) commonComponentData((EBFcontribution*)contribution);

        // Actually do the parsing
        AcdParser acd(event, contribution, "   ");
        acd.parse();
         return 0;
    }

    int LatComponentParser::CALcomponent(EBFevent* event, CALcontribution* contribution)
    {
        // Purpose and Method:

        using namespace ldfReader;

        // Skip this contribution if there was a packet error
        if ( (EBFcontribution*)contribution->packetError()) {
            printf("Packet Error in TEM - found in CAL processing - skipping\n");
            printf("Event: %llu Apid: %d\n", 
                ldfReader::LatData::instance()->eventId(),
                ldfReader::LatData::instance()->getCcsds().getApid());
            return 0;
        }

        // Use this to determine if we've already picked up the TEM summary
        _calSrc = LATPcellHeader::source(contribution->header());
        // Retrieve the tower object we wish to update
        // Just doing this for the summary data
        unsigned int towerId = _calSrc;
        LatData* curLatData = LatData::instance();
        TowerData* tower = curLatData->getTower(towerId);
        if (!tower) {
            tower = new TowerData(towerId);
            curLatData->addTower(tower);
            ldfReader::EventSummaryCommon summary(((EBFcontribution*)contribution)->summary());
            ldfReader::TemData tem(summary);
            //printf("Summary in TEM in CAL\n");
            //tem.summary().print();
            tem.setExist();
            tem.initPacketError(((EBFcontribution*)contribution)->packetError());
            tem.initLength(((EBFcontribution*)contribution)->length());
            tower->setTem(tem);
        }

       // if (EbfDebug::getDebug() ) printf("\nTEM %2d:\n", dumpLATPcellHeader(_calSrc));

        // Actually do the parsing
        CalParser cal(event, contribution, "   ");
        cal.parse();
        CALend(cal.CALend());
        tower->getTem().calEnd(CALend());
        return 0;
    }

    int LatComponentParser::TKRcomponent(EBFevent* event, 
        TKRcontribution* contribution)
    {
        // Purpose and Method:  Callback for processing the TKR data for one tower
        // Creates a TkrParser object which handles the actual parsing of the data
        // At the end of this routine - we set up the pointer for the location of the
        // diagnostic data via the TKRend call.

        using namespace ldfReader;

        // Skip this contribution if there was a packet error
        if ( (EBFcontribution*)contribution->packetError()) {
            printf("Packet Error in TEM - found in TKR processing - skipping\n");
            printf("Event: %llu Apid: %d\n", 
                ldfReader::LatData::instance()->eventId(),
                ldfReader::LatData::instance()->getCcsds().getApid());
            return 0;
         }

        // Retrieve the tower object we wish to update with this TKR data
        unsigned int towerId = LATPcellHeader::source(contribution->header());
        LatData* curLatData = LatData::instance();
        TowerData* tower = curLatData->getTower(towerId);
        if (!tower) {
            tower = new TowerData(towerId);
            curLatData->addTower(tower);
        }
        if (_calSrc != LATPcellHeader::source(contribution->header())) {
            // We haven't picked up this TEM's event summary yet
            ldfReader::EventSummaryCommon summary(((EBFcontribution*)contribution)->summary());
            ldfReader::TemData tem(summary);
            printf("Summary in TEM in TKR\n");
            tem.summary().print();
            tem.setExist();
            tem.initPacketError(((EBFcontribution*)contribution)->packetError());
            tem.initLength(((EBFcontribution*)contribution)->length());
            tower->setTem(tem);
        }
        
        // Actually do the parsing
        TkrParser tkr(event, contribution, tower, "   ");
        tkr.parse();
        // Set the location of the diagnostic data in this event
        TKRend(tkr.diagnostic());
        tower->getTem().tkrEnd(TKRend());
        return 0;
    }

    int LatComponentParser::diagnostic (EBFevent* event, TEMcontribution* contribution) {
       
        // Size of the Diagnostic Contribution
        const int diagSize = 48;

        // Skip this contribution if there was a packet error
        if ( (EBFcontribution*)contribution->packetError()) {
            printf("%s %s",
                 "Packet Error found in TEM ",
                 " - during Diagnostic processing - skipping ");
            printf("Event: %llu Apid: %d\n", 
                ldfReader::LatData::instance()->eventId(),
                ldfReader::LatData::instance()->getCcsds().getApid());
            return 0;
        }


        //ldfReader::LatData::instance()->diagnostic()->setSummary(contribution->summary());
        //ldfReader::LatData::instance()->diagnostic()->initPacketError(contribution->packetError());
        if ( EventSummary::diagnostic(contribution->summary())) {
            //   Process the trigger primitives in the diagnostic data
       
            unsigned int towerId = LATPcellHeader::source(contribution->header());
            LatData* curLatData = LatData::instance();
            TowerData* tower = curLatData->getTower(towerId);
            if (!tower) {
                tower = new TowerData(towerId);
                curLatData->addTower(tower);
            }
            tower->getTem().getDiagnostic()->setExist();
            DiagnosticParser iter(event,contribution,TKRend(),tower->getTem().getDiagnostic());
            iter.iterateCAL();
            iter.iterateTKR();

            diagnosticEnd(TKRend()+iter.size());  // Reset the end of the diagnostic data
            tower->getTem().getDiagnostic()->initLength(iter.size());

            if (iter.size() != diagSize) 
                printf("WARNING - Diagnostic data size is not %d, it is %d\n",
                       diagSize, iter.size());
        }
        return 0;
    }

int LatComponentParser::error(EBFevent* event, TEMcontribution* contribution) {

    // Skip this contribution if there was a packet error
    if ( (EBFcontribution*)contribution->packetError()) {
       printf("%s",
          "Packet Error found in TEM - during error processing - skipping");
       printf("Event: %llu Apid: %d\n", 
              ldfReader::LatData::instance()->eventId(),
              ldfReader::LatData::instance()->getCcsds().getApid());
       return 0;
    }

    ldfReader::ErrData err;
    err.initLength(((EBFcontribution*)contribution)->length());    
    //err.initPacketError(((EBFcontribution*)contribution)->packetError());
    if ( EventSummary::error(((EBFcontribution*)contribution)->summary())) {

        unsigned int towerId = LATPcellHeader::source(contribution->header());
        LatData* curLatData = LatData::instance();
        TowerData* tower = curLatData->getTower(towerId);
        if (!tower) {
            tower = new TowerData(towerId);
            curLatData->addTower(tower);
        }


        tower->getTem().getErr()->setExist();
        unsigned offset;
        if (0 != diagnosticEnd())
            offset=diagnosticEnd();
        else
            offset=TKRend();
        ErrParser errParse(event,contribution,offset);
        ErrorSummary theError = errParse.theError();
        tower->getTem().getErr()->initCal(theError.cal());
        tower->getTem().getErr()->initTkr(theError.tkr());
        tower->getTem().getErr()->initPhs(theError.phs());
        tower->getTem().getErr()->initTmo(theError.tmo());
        errParse.iterate();
        errorEnd(offset+errParse.size());
        tower->getTem().getErr()->initLength(errParse.size());    
    }
    return 0;
}

int LatComponentParser::cleanup (EBFevent*        /*event*/,
        TEMcontribution* contribution)
{
    // Skip this contribution if there was a packet error
    if ( (EBFcontribution*)contribution->packetError()) return 0;


        // determine whether the remainder of the contribution is
        // a: less than one cell away
        // b: all zeroes
        char* prefix = "  ";

        if (EbfDebug::getDebug())  
            printf("CALend: %d, TKRend: %d, diagEnd: %d, errEnd: %d\n",
        	   CALend(),TKRend(),diagnosticEnd(),errorEnd());

        unsigned* contribEnd = (unsigned*)((char*)contribution + contribution->length() );

        if (contribution->length() - errorEnd() > 31)
            printf("%sProblem in parsing:  0x%04x bytes after end of TEM\n",
            prefix, contribution->length() - errorEnd());

        unsigned* mbz = (unsigned*)((char*)contribution + errorEnd());
        //printf("%sError End: 0x%08x 0x%08x\n",prefix, mbz, contribution);

        while (mbz < (unsigned*)contribEnd)
        {
            if (*mbz) {
                printf("%sNon-zero bytes beyond end of TEM: 0x%08x\n",
                       prefix,*mbz);
                printf("Event: %llu Apid: %d\n", 
                    ldfReader::LatData::instance()->eventId(),
                    ldfReader::LatData::instance()->getCcsds().getApid());
            }
            mbz++;
        }
        return 0;
}

int LatComponentParser::commonComponentData(EBFcontribution *contribution) {
        int len = contribution->length();
        // printed in hex
        int error = contribution->packetError();
        int seqNum = contribution->sequence();
        // LCB header  printed in hex
        int header = contribution->header();
        return 0;
    }

void LatComponentParser::dumpLATPcellheader(const unsigned header, const char* pfx)
{
  printf("%s     source = 0x%02x\n", pfx, LATPcellHeader::source     (header));
  printf("%sdestination = 0x%02x\n", pfx, LATPcellHeader::destination(header));
  printf("%s    respond = %d\n"    , pfx, LATPcellHeader::respond    (header));
  printf("%s   protocol = %d\n"    , pfx, LATPcellHeader::protocol   (header));
  printf("%s     parity = %d\n"    , pfx, LATPcellHeader::parity     (header));
}


int LatComponentParser::handleError(EBFcontribution* contrib,
                                         unsigned code, unsigned p1, unsigned p2) const
 {
   fprintf(stderr, "%s %s\n",
      "MyLATcomponentIterator::handleError:  Somehow an error occured.");
   fprintf(stderr, "  code=%d, p1=%d, p2=%d\n", code, p1, p2);
   fprintf(stderr, "Event: %llu Apid: %d\n", 
                    ldfReader::LatData::instance()->eventId(),
                    ldfReader::LatData::instance()->getCcsds().getApid());
   return 0;
 }

int LatComponentParser::handleError(EBFevent* event,
                                unsigned code, unsigned p1, unsigned p2) const
 {
   switch (code)
   {
     case EBFcontributionIterator::ERR_NumContributions:
     {
       fprintf(stderr, "EBFcontributionIterator::iterate: "
                      "Number of contributions found > %d\n",
                      p1);
       fprintf(stderr, "Event: %llu Apid: %d\n", 
                    ldfReader::LatData::instance()->eventId(),
                    ldfReader::LatData::instance()->getCcsds().getApid());
       break;
     }
     case EBFcontributionIterator::ERR_PastEnd:
     {
       fprintf(stderr, "EBFcontributionIterator::iterate: "
                      "Iterated past end of event by 0x%0x = %d bytes\n",
                      p1, p1);
       fprintf(stderr, "Event: %llu Apid: %d\n", 
                    ldfReader::LatData::instance()->eventId(),
                    ldfReader::LatData::instance()->getCcsds().getApid());
       break;
     }
     case EBFcontributionIterator::ERR_ZeroLength:
     {
       fprintf(stderr, "\nEBFcontributionIterator::iterate:\n"
              "  Found a contribution with zero length\n");
       fprintf(stderr, "Event: %llu Apid: %d\n", 
                    ldfReader::LatData::instance()->eventId(),
                    ldfReader::LatData::instance()->getCcsds().getApid());
       break;
     }
     case EBFcontributionIterator::ERR_PacketError:
     {
       /* ERR_PacketError gets an additional parameter */
       char *type;

       switch (p1)
       {
         case EBFcontribution::Parity:      type = "parity";       break;
         case EBFcontribution::Truncated:   type = "truncated";    break;
         case EBFcontribution::WriteFault:  type = "write fault";  break;
         case EBFcontribution::TimedOut:    type = "timed out";    break;
         default:                           type = "unknown";      break;
       }
       fprintf(stderr, "\nEBFcontributionIterator::iterate:\n   Skipping contribution with source ID %d having packet %s error\n", p2, type);
       fprintf(stderr, "Event: %llu Apid: %d\n", 
                    ldfReader::LatData::instance()->eventId(),
                    ldfReader::LatData::instance()->getCcsds().getApid());
       break;
     }
     case EBFcontributionIterator::ERR_NoMap:
     {
       fprintf(stderr, "No contribution map exists for EBF version %0x\n",
              p1);
       fprintf(stderr, "Event: %llu Apid: %d\n", 
                    ldfReader::LatData::instance()->eventId(),
                    ldfReader::LatData::instance()->getCcsds().getApid());
       break;
     }
     default: break;
   }
   return code;
 }

}

#endif
