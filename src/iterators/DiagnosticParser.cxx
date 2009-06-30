#ifndef DiagnosticParser_CXX
#define DiagnosticParser_CXX 1

/** @file DiagnosticParser.cxx
@brief Implementation of the DiagnosticParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/DiagnosticParser.cxx,v 1.5 2008/10/03 03:39:17 heather Exp $
*/

#include "DiagnosticParser.h"

//#include "ldfReader/EbfException.h"
#include "ldfReader/data/LatData.h"
#include "../EbfDebug.h"

namespace ldfReader {
    int DiagnosticParser::CALdiag(unsigned tower, unsigned layer, CALdiagnostic cal)
    {
        // Diagnostic cont structure is:
        // - 8 32 bit contributions for GCCC
        //     Order: 0(x0,x1) 2(x2,x3) 3(y0,y1) 4(y2,y3)
        //  const char*    layerLabel[] = {"x0", "x1", "x2", "x3", "y0", "y1", "y2", "y3"};
        //  const unsigned gccc[]       = {0, 2, 3, 1};

        // Retrieve the tower or create a new TowerData object if necessary
        ldfReader::LatData* curLatData = ldfReader::LatData::instance();
        ldfReader::TowerData* twrData = curLatData->getTower(tower);
        if (!twrData) {
            twrData = new ldfReader::TowerData(tower);
            curLatData->addTower(twrData);
        }
        ldfReader::DiagnosticData* diagData = twrData->getTem().getDiagnostic();
        diagData->setExist();

        const ldfReader::CalDiagnosticData calDiagData(cal.datum(), tower, layer);
        diagData->addCalDiagnostic(calDiagData);
        diagData->initLength(DIAGcontributionIterator::size());

        unsigned long diagSize = diagData->lenInBytes();

        if (DIAGcontributionIterator::size() != diagSize)
          printf("WARNING - Diagnostic data size is not %d, it is %d\n",
                 diagSize, DIAGcontributionIterator::size());
        return 0;
    }

    int DiagnosticParser::TKRdiag(unsigned tower, unsigned gtcc,  TKRdiagnostic tkr)
    {
        // Retrieve the tower or create a new TowerData object if necessary
        ldfReader::LatData* curLatData = ldfReader::LatData::instance();
        ldfReader::TowerData* twrData = curLatData->getTower(tower);
        ldfReader::DiagnosticData* diagData = twrData->getTem().getDiagnostic();

        // - 8 16 bit contributions for GTCC
        const ldfReader::TkrDiagnosticData tkrDiagData(tkr.datum(), tower, gtcc);
        diagData->addTkrDiagnostic(tkrDiagData);
        return 0;
    }


    int DiagnosticParser::handleError(TEMcontribution* contribution,
                                 unsigned code, unsigned p1, unsigned p2) const
    {
        int rc = DIAGcontributionIterator::handleError(contribution,code,p1,p2);
        _handleErrorCommon();
        return rc;
    }



}
#endif
