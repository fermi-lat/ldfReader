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

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/DiagnosticParser.h,v 1.1.1.1 2004/04/15 20:02:22 heather Exp $
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

        virtual int handleError(TEMcontribution*, unsigned code, unsigned p1=0, unsigned p2=0) const;

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
