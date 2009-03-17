#ifndef CalParser_CXX
#define CalParser_CXX 1

/** @file CalParser.cxx
@brief Implementation of the CalParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/CalParser.cxx,v 1.8 2008/10/03 03:39:17 heather Exp $
*/

// EBF Online Library includes
#include "eventSummary.h"
#include "CALlog.h"
#include "CALlogEnd.h"

#include <stdio.h>
#include <assert.h>

// ldfReader includes
#include "CalParser.h"
#include "ldfReader/data/LatData.h"
#include "ldfReader/LdfException.h"
#include "../EbfDebug.h"

namespace {
    static const char* layerTag[] = {"x0", "y0", "x1", "y1", "x2", "y2", "x3", "y3"};
    static const unsigned  gccc[] = {  0,    3,    0,    3,    2,    1,    2,    1};
}   // end default namespace

namespace ldfReader {
    CalParser::CalParser( const char*      prefix) :
        CALcontributionIterator(), m_prefix(prefix)
    {
    }


    void CalParser::log(unsigned tower, unsigned layer, CALlog theLog)
    {
        // Purpose and Method:  The CALcontributionIterator callback which handles the real 
        // parser.

        char name[10];
        sprintf(name, "  %1d    %2s ", gccc[layer], layerTag[layer]);
        if (EbfDebug::getDebug()==EbfDebug::ALL) {
            printf("%s     %2d   %10s   %2d   0x%03x   %1d   0x%03x   %1d\n",
                m_prefix, tower, name, theLog.column(),
                theLog.positive().value(), theLog.positive().range(),
                theLog.negative().value(), theLog.negative().range());
        }  

        // Retrieve the tower or create a new TowerData object if necessary
        ldfReader::LatData* curLatData = ldfReader::LatData::instance();
        ldfReader::TowerData* tData = curLatData->getTower(tower);
        if (!tData) {
            tData = new ldfReader::TowerData(tower);
            curLatData->addTower(tData);
            ldfReader::EventSummaryCommon summary(((EBFcontribution*)contribution())->summary());
            ldfReader::TemData tem(summary);
            //printf("Summary in TEM in CAL\n");
            //tem.summary().print();
            tem.setExist();
            tem.initPacketError(((EBFcontribution*)contribution())->packetError());
            tem.initLength(((EBFcontribution*)contribution())->length());
            tData->setTem(tem);

        }

        // Get the Layer and Column 
        unsigned int offlineLayer = layer;
        unsigned int offlineColumn = theLog.column();
        // Determine the mode
        unsigned readout4 = EventSummary::readout4(event()->summary());
        ldfReader::CalDigi::CalTrigMode mode = (readout4 == 0) ? 
                 ldfReader::CalDigi::BESTRANGE : ldfReader::CalDigi::ALLRANGE;

        // Retrieve the CalDigi object for this layer and column - or create a new one
        ldfReader::CalDigi *digi = tData->getCalDigi(offlineLayer, offlineColumn);
        if (!digi) {
            digi = new ldfReader::CalDigi();
            digi->init(offlineLayer, offlineColumn, mode);
            tData->addCalDigi(digi);
        } else {
            // If the digi already exists for this tower - we should be in AllRange mode
            if (readout4 != 1) {
                fprintf(stderr, 
                "Two CalDigis with the same layer/column combination ");
                fprintf(stderr,
                " - yet we are in BESTRANGE mode.\n");
                _handleErrorCommon();
            }
        }

        // Add the readout data
        digi->addReadout(ldfReader::CalDigi::CalReadout( 
            theLog.positive().range(), theLog.positive().value(), 
            theLog.negative().range(), theLog.negative().value()));

    }

    int CalParser::handleError(CALcontribution *contribution, unsigned code, 
                    unsigned p1, unsigned p2) const {
        
        _handleErrorCommon();
        return CALcontributionIterator::handleError(contribution, code,p1,p2);

    }

}
#endif
