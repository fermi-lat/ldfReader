#ifndef DiagnosticParser_CXX
#define DiagnosticParser_CXX 1

/** @file DiagnosticParser.cxx
@brief Implementation of the DiagnosticParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/DiagnosticParser.cxx,v 1.3 2004/02/05 17:29:51 heather Exp $
*/

#include "DiagnosticParser.h"

//#include "ldfReader/EbfException.h"
#include "../EbfDebug.h"

namespace ldfReader {
    DiagnosticParser::DiagnosticParser(EBFevent*  event,
        TEMcontribution* contribution,
        unsigned         dataStart,
        ldfReader::DiagnosticData* diagData) :
    DIAGcontributionIterator(event, contribution) {
        offset(dataStart);
        m_diagData = diagData;
    }

    int DiagnosticParser::CALdiag(unsigned tower, unsigned layer, CALdiagnostic cal)
    {
        // Diagnostic cont structure is:
        // - 8 32 bit contributions for GCCC
        //     Order: 0(x0,x1) 2(x2,x3) 3(y0,y1) 4(y2,y3)
        //  const char*    layerLabel[] = {"x0", "x1", "x2", "x3", "y0", "y1", "y2", "y3"};
        //  const unsigned gccc[]       = {0, 2, 3, 1};
        const ldfReader::CalDiagnosticData calDiagData(cal.datum());
        m_diagData->addCalDiagnostic(calDiagData);
        return 0;
    }

    int DiagnosticParser::TKRdiag(unsigned tower, unsigned gtcc,  TKRdiagnostic tkr)
    {
        // - 8 16 bit contributions for GTCC
        const ldfReader::TkrDiagnosticData tkrDiagData(tkr.datum() & 0xffff);
        m_diagData->addTkrDiagnostic(tkrDiagData);
        return 0;
    }
}
#endif
