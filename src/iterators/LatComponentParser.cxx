#ifndef LATCOMPONENTPARSER_CXX
#define LATCOMPONENTPARSER_CXX 1

/** @file LatComponentParser.cxx
@brief Implementation of the LatComponentParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/LatComponentParser.cxx,v 1.20 2004/02/14 05:14:08 heather Exp $
*/

#include <stdio.h> // included for LATcomponentIterator.h in Online/EBF
#include "LatComponentParser.h"
#include "LATp.h"
#include "GEMcontribution.h"
#include "CALcontribution.h"
#include "TKRcontribution.h"
#include "DiagnosticParser.h"
#include "TkrParser.h"
#include "CalParser.h"
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
        printf("\nOSW:\n");
        //((MyEBFcontribution*)contribution)->dump(event, prefix);
        //printf("%sOSW:\n", prefix);
        //MyOSWiterator iterator(event, contribution, prefix);
        //iterator.dump();
        return 0;
    }

    int LatComponentParser::GLTcomponent(EBFevent* event, GEMcontribution* gem)
    {
        if (EbfDebug::getDebug())  printf("\nGEM:\n");
        ldfReader::LatData::instance()->setSummary(event->summary());

        // N.B. the payload of the GEM contribution will change in the future
        unsigned seconds     = ((unsigned*)gem->data())[0];
        unsigned nanoSeconds = ((unsigned*)gem->data())[1];
        ldfReader::LatData::instance()->setTime(seconds, nanoSeconds);
        if (gem->payloadSize() > 8) {              // Timebase does not exist in old data
            unsigned upperPpcWord = ((unsigned*)gem->data())[2];
            unsigned lowerPpcWord = ((unsigned*)gem->data())[3];
            ldfReader::LatData::instance()->setPpcTimeBase(upperPpcWord, lowerPpcWord);
        }

        //((MyEBFcontribution*)contribution)->dump("  ");
        //((MyGLTcontribution*)contribution)->dump("  ");
        return 0;
    }

    int LatComponentParser::ACDcomponent(EBFevent*, AEMcontribution* )
    {
        if (EbfDebug::getDebug() ) printf("\nAEM:\n");
        //((MyEBFcontribution*)contribution)->dumpCommon("  ");
        //((MyACDcontribution*)contribution)->dump("  ");
        return 0;
    }

    int LatComponentParser::CALcomponent(EBFevent* event, CALcontribution* contribution)
    {
        // Purpose and Method:

        using namespace ldfReader;

        _calSrc = LATPcellHeader::source(contribution->header());
        if (EbfDebug::getDebug() ) printf("\nTEM %2d:\n", _calSrc);

        // Actually do the parsing
        CalParser cal(event, contribution, "   ");
        cal.parse();
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

        if (_calSrc != LATPcellHeader::source(contribution->header()))
        {
            // printf("\nTEM: %d\n", _calSrc);
            //((MyEBFcontribution*)contribution)->dumpCommon("  ");
        }

        // Retrieve the tower object we wish to update with this TKR data
        unsigned int towerId = _calSrc;
        LatData* curLatData = LatData::instance();
        TowerData* tower = curLatData->getTower(towerId);
        if (!tower) {
            tower = new TowerData(towerId);
            curLatData->addTower(tower);
        }

        // Actually do the parsing
        TkrParser tkr(event, contribution, tower, "   ");
        tkr.parse();
        // Set the location of the diagnostic data in this event
        TKRend(tkr.diagnostic());
        return 0;
    }

    int LatComponentParser::diagnostic (EBFevent* event, TEMcontribution* contribution) {
        if ( EventSummary::diagnostic(event->summary())) {
            //   Process the trigger primitives in the diagnostic data
            DiagnosticParser iter(event,contribution,TKRend(),ldfReader::LatData::instance()->diagnostic());
            iter.iterateCAL();
            iter.iterateTKR();

            diagnosticEnd(TKRend()+iter.size());  // Reset the end of the diagnostic data
        }
        return 0;
    }

    int LatComponentParser::error(EBFevent* event, TEMcontribution* contribution) {
        return 0;
    }

    int LatComponentParser::cleanup (EBFevent*        /*event*/,
        TEMcontribution* contribution)
    {
        // determine whether the remainder of the contribution is
        // a: less than one cell away
        // b: all zeroes
        char* prefix = "  ";

        //  printf("CALend: %d, TKRend: %d, diagEnd: %d, errEnd: %d\n",
        //	   CALend(),TKRend(),diagnosticEnd(),errorEnd());

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

}
#endif
