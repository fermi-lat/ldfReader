#ifndef ldfReader_LATCONTRIBUTIONPARSER_CXX
#define ldfReader_LATCONTRIBUTIONPARSER_CXX 1

/** @file LatContributionParser.cxx
@brief Implementation of the LatContributionParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/LatContributionParser.cxx,v 1.1.1.1 2004/04/15 20:02:23 heather Exp $
*/

//#include <stdio.h> // included for LATcomponentIterator.h in Online/EBF
#include "LatContributionParser.h"
#include "../EbfDebug.h"

namespace ldfReader {
    int LatContributionParser::EBF(EBFevent* event, EBFevent* end) {
        // Iterate over a list of EBF events
        m_eep.iterate(event, end);

        return m_eep.status();
    }

    int LatContributionParser::UDF(LATcontribution* event, LATcontribution* end)   { 
        printf ("WARNING:  Ignoring UDF contributions - any questions, see Eduardo\n");
        return 0; 

    }
}
#endif
