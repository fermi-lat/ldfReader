#ifndef DiagnosticPARSER_H
#define DiagnosticPARSER_H 1

#include <stdio.h>
#include "DIAGcontributionIterator.h"
#include "ldfReader/data/DiagnosticData.h"

/** @class DiagnosticParser
@brief Calls the appropriate routines in the Online/EBF library to 
processing the diagnostic data

$Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/ldfReader/src/iterators/DiagnosticParser.h,v 1.3 2008/10/03 03:39:17 heather Exp $
*/

namespace ldfReader {
    class DiagnosticParser : public virtual DIAGcontributionIterator
    {
    public:
        DiagnosticParser(const char* prefix) : m_prefix(prefix) { };

        virtual ~DiagnosticParser() {}


        virtual int CALdiag(unsigned tower, unsigned layer, CALdiagnostic diag);
        virtual int TKRdiag(unsigned tower, unsigned gtcc,  TKRdiagnostic diag);

        virtual int handleError(TEMcontribution*, unsigned code, unsigned p1=0, unsigned p2=0) const;

   protected :
        virtual void _handleErrorCommon() const = 0;

    private:
        const char *m_prefix;
    };

}
#endif
