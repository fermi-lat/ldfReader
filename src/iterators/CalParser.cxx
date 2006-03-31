#ifndef CalParser_CXX
#define CalParser_CXX 1

/** @file CalParser.cxx
@brief Implementation of the CalParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/CalParser.cxx,v 1.4 2005/04/05 21:23:39 heather Exp $
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
    CalParser::CalParser(EBFevent* event,
        CALcontribution* contribution,
        const char*      prefix) :
    CALcontributionIterator(event, contribution),
        m_prefix(prefix)
    {
    }


    void CalParser::parse()
    {
        // Purpose and Method:  Calls the CALcontributionIterator::iterate method which causes
        // CalParser::log to be called which does the real parsing

        unsigned length = contribution()->numLogAccepts();
        if (length == 0)
        {
            if (EbfDebug::getDebug())
                printf("%sCAL: no data - empty contribution\n", m_prefix);
            return;
        }
        if (EbfDebug::getDebug()) {

            printf("%sCAL:\n", m_prefix);
            printf("%s  Log Accepts           = 0x%08x\n", m_prefix, contribution()->logAccepts());
            printf("%s  Number of Log Accepts = %d\n", m_prefix,     contribution()->numLogAccepts());

            printf("%s  Data:\n", m_prefix);
            printf("%s                            +ADC  +ADC  -ADC  -ADC\n", m_prefix);
            printf("%s   tower GCCC  layer column value range value range\n", m_prefix);
        }

        iterate();

    }

    void CalParser::log(unsigned tower, unsigned layer, CALlog theLog)
    {
        // Purpose and Method:  The CALcontributionIterator callback which handles the real 
        // parser.

        char name[10];
        sprintf(name, "  %1d    %2s ", gccc[layer], layerTag[layer]);
        if (EbfDebug::getDebug()) {
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
        }

        // Get the Layer and Column 
        unsigned int offlineLayer = layer;
        unsigned int offlineColumn = theLog.column();
        // Determine the mode
        unsigned readout4 = EventSummary::readout4(event()->summary());
        ldfReader::CalDigi::CalTrigMode mode = (readout4 == 0) ? ldfReader::CalDigi::BESTRANGE : ldfReader::CalDigi::ALLRANGE;

        // Retrieve the CalDigi object for this layer and column - or create a new one
        ldfReader::CalDigi *digi = tData->getCalDigi(offlineLayer, offlineColumn);
        if (!digi) {
            digi = new ldfReader::CalDigi();
            digi->init(offlineLayer, offlineColumn, mode);
            tData->addCalDigi(digi);
        } else {
            // If the digi already exists for this tower - we should be in AllRange mode
            if (readout4 != 1) 
                fprintf(stderr, 
                "Two CalDigis with the same layer/column combination - yet we are in BESTRANGE mode\n");
        }

        // Add the readout data
        digi->addReadout(ldfReader::CalDigi::CalReadout(theLog.positive().range(), theLog.positive().value(), 
            theLog.negative().range(), theLog.negative().value()));

    }

    int CalParser::handleError(CALcontribution *contribution, unsigned code, 
                    unsigned p1, unsigned p2) const {
        
        fprintf(stderr, "MyCALiterator::handleError:  Somehow an error occured. \n ");
        fprintf(stderr, "  code=%d, p1=%d, p2=%d\n", code, p1, p2);
        return 0;
    }

}
#endif
