#ifndef ldfReader_LATCONTRIBUTIONPARSER_CXX
#define ldfReader_LATCONTRIBUTIONPARSER_CXX 1

/** @file LatContributionParser.cxx
@brief Implementation of the LatContributionParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/LatContributionParser.cxx,v 1.2 2005/01/14 19:55:41 heather Exp $
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

    int LatContributionParser::UDF(LATcontribution* event, LATcontribution* end)
   {
        printf ("WARNING:  Ignoring UDF contributions - any questions, see Eduardo\n");
        return 0;

    }

int LatContributionParser::handleError(LATcontribution* contribution,
                               unsigned code, unsigned p1, unsigned p2) const
{
  switch (code)
  {
    case LATcontributionIterator::ERR_UDFcontribution:
    {
      fprintf(stderr, "LATcontributionIterator::UDF: "
        "Found unrecognized LATdatagram contribution type 0x%08X\n",
        p1);
      return -1;
      break;
    }
    default: break;
  }
  return 0;
}


}
#endif
