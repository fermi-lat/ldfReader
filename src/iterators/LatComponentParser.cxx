#ifndef LATCOMPONENTPARSER_CXX
#define LATCOMPONENTPARSER_CXX 1

/** @file LatComponentParser.cxx
@brief Implementation of the LatComponentParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/LatComponentParser.cxx,v 1.16 2005/01/31 20:18:31 heather Exp $
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

    int LatComponentParser::UDFcomponent(EBFevent*, EBFcontribution* )
    {
        fprintf (stderr, "\nUndefined EBF component\n");
        //((MyEBFcontribution*)contribution)->dump("  ");
        return 0;
    }

    int LatComponentParser::OSWcomponent(EBFevent*        event,
        OSWcontribution* contribution)
    {
        const char* prefix = "  ";
        ldfReader::LatData::instance()->setSummary(event->summary());
        ldfReader::OswData osw(ldfReader::EventSummaryCommon(((EBFcontribution*)contribution)->summary()));
        osw.initPacketError(contribution->packetError());
        //ldfReader::LatData::instance()->getOsw().initLength(((EBFcontribution*)contribution)->length());
        osw.initLength(((EBFcontribution*)contribution)->length());

        // OSW contribution only exists in later versions starting in Feb 2004
        if (ldfReader::LatData::instance()->getFormatIdentity() >= ID_WITH_OSW) {
            osw.setExist(); 
            OswParser oswParse(event, contribution);
            oswParse.iterate();
        }
        ldfReader::LatData::instance()->setOsw(osw);
        return 0;
    }

    int LatComponentParser::GEMcomponent(EBFevent *event, GEMcontribution *contribution) {
        
        //if (EbfDebug::getDebug())  printf("\nGEM:\n");
        ldfReader::GemData gem;

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
            tileList);
        gem.initSummary(contribution->liveTime(), contribution->prescaled(),
            contribution->discarded(), contribution->sent(), 
            contribution->triggerTime(), time, contribution->deltaEventTime());

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
       
        //ldfReader::LatData::instance()->diagnostic()->setSummary(contribution->summary());
        //ldfReader::LatData::instance()->diagnostic()->initPacketError(contribution->packetError());
        if ( EventSummary::diagnostic(contribution->summary())) {
            //   Process the trigger primitives in the diagnostic data
            ldfReader::LatData::instance()->diagnostic()->setExist();
            DiagnosticParser iter(event,contribution,TKRend(),ldfReader::LatData::instance()->diagnostic());
            iter.iterateCAL();
            iter.iterateTKR();

            diagnosticEnd(TKRend()+iter.size());  // Reset the end of the diagnostic data
            ldfReader::LatData::instance()->diagnostic()->initLength(iter.size());
        }
        return 0;
    }

int LatComponentParser::error(EBFevent* event, TEMcontribution* contribution) {
    ldfReader::ErrData err;
    err.initLength(((EBFcontribution*)contribution)->length());    
    //err.initPacketError(((EBFcontribution*)contribution)->packetError());
    if ( EventSummary::error(((EBFcontribution*)contribution)->summary())) {
        err.setExist();
        unsigned offset;
        if (0 != diagnosticEnd())
            offset=diagnosticEnd();
        else
            offset=TKRend();
        ErrParser errParse(event,contribution,offset);
        errParse.iterate();
        errorEnd(offset+errParse.size());
        err.initLength(errParse.size());    
    }
    ldfReader::LatData::instance()->setErr(err);
    return 0;
}

int LatComponentParser::cleanup (EBFevent*        /*event*/,
        TEMcontribution* contribution)
{
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
            if (*mbz)
                printf("%sNon-zero bytes beyond end of TEM: 0x%08x\n",
                prefix,*mbz);
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

}
#endif
