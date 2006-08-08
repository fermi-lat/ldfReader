#ifndef EBFDATAGRAMPARSER_H
#define EBFDATAGRAMPARSER_H 1

#include <stdio.h>               // jrb - for EBF file LATdatagramIterator.h
#include "LATdatagramIterator.h"
#include "src/iterators/LatContributionParser.h"
#include "ldfReader/data/LatData.h"

#include "../../src/iterators/EbfEventParser.h"


/** @class EbfDatagramParser
@brief Calls the appropriate routines in the Online/EBF library to start
processing the event.

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/EbfDatagramParser.h,v 1.3 2005/04/05 21:23:39 heather Exp $
*/
namespace ldfReader {
    class EbfDatagramParser : public LATdatagramIterator
    {
    public:
        EbfDatagramParser(LATdatagram* start, LATdatagram* end) : LATdatagramIterator(start, end) {}
        virtual ~EbfDatagramParser() {}

        virtual int process(LATdatagram*);

        virtual int handleError(LATdatagram *datagram, unsigned code, unsigned p1=0, unsigned p2=0) const {

        switch (code)
        {
            case LATdatagramIterator::ERR_IDmismatch:
            {
                fprintf(stderr, "LATdatagramIterator::iterate: "
                       "Identity mismatch: got %08x, expected %08x\n",
                       p1, p2);
                return -1;
                break;
            }
            default: break;
        }
        return 0;
    };

    private:
        LatContributionParser m_lci;
    };



}
#endif
