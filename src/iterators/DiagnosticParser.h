#ifndef DiagnosticPARSER_H
#define DiagnosticPARSER_H 1

#include <stdio.h>
#include "DIAGcontributionIterator.h"
#include "ldfReader/data/DiagnosticData.h"
//#include "LatComponentParser.h"
//#include "ldfReader/data/LatData.h"

/** @class DiagnosticParser
@brief Calls the appropriate routines in the Online/EBF library to 
processing the diagnostic data

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/DiagnosticParser.h,v 1.2 2004/02/05 17:29:51 heather Exp $
*/

namespace ldfReader {
    class DiagnosticParser : public DIAGcontributionIterator
    {
    public:
        DiagnosticParser(EBFevent*  event,
            TEMcontribution* contribution,
            unsigned         dataStart,
            ldfReader::DiagnosticData *diagData);

        virtual ~DiagnosticParser() {}

        //unsigned process(EBFevent* event);

        virtual int CALdiag(unsigned tower, unsigned layer, CALdiagnostic diag);
        virtual int TKRdiag(unsigned tower, unsigned gtcc,  TKRdiagnostic diag);

    private:
        ldfReader::DiagnosticData *m_diagData;
    };
    /*
    unsigned DiagnosticParser::process(EBFevent* event)
    {

    LatComponentParser lci;

    lci.EBFcontributionIterator::iterate(event);

    return 0;// Return non-zero to abort iteration
    }
    */
}
#endif
