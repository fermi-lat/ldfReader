#ifndef EBFEVENTPARSER_CXX
#define EBFEVENTPARSER_CXX 1

#include "EbfEventParser.h"
#include "ldfReader/data/LatData.h"

namespace ldfReader {

    int EbfEventParser::process(EBFevent* event)
    {
        if (event->status() != 0)
        {
            printf("\n");
            printf("Bad status %d in event header\n", event->status());
            printf("Aborting processing of event\n");
            printf("\n");
            return event->status();
        }

        ldfReader::LatData::instance()->setFormatIdentity(event->identity().value());
        LatComponentParser lci;
        lci.EBFcontributionIterator::iterate(event);

        // Chose to return the lower level status if both are set
        int status = 0;
        if (lci.EBFcontributionIterator::status())
            status = lci.EBFcontributionIterator::status();
        if (lci.TEMcontributionIterator::status())
            status = lci.TEMcontributionIterator::status();
        return status; // Return non-zero to abort iteration
    }


int EbfEventParser::handleError(EBFevent* evt, unsigned code, unsigned p1,
 unsigned p2) const
 {
   switch (code)
   {
     case EBFeventIterator::ERR_NonEBFevent:
     {
       fprintf(stderr, "EBFeventIterator::iterate: "
                       "Encountered a non-EBFevent datagram contribution "
                       "with typeId %08x\n", p1);
       return 1;
       break;
     }
     default: break;
   }
   return 0;
 }


}
#endif
