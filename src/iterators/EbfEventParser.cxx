#ifndef EBFEVENTPARSER_CXX
#define EBFEVENTPARSER_CXX 1

#include "EbfEventParser.h"

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

}
#endif
