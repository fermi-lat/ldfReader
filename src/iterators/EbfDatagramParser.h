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

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/EbfDatagramParser.h,v 1.5 2003/05/20 16:10:27 jrb Exp $
*/
namespace ldfReader {
    class EbfDatagramParser : public LATdatagramIterator
    {
    public:
        EbfDatagramParser(LATdatagram* start, LATdatagram* end) : LATdatagramIterator(start, end) {}
        ~EbfDatagramParser() {}

        int process(LATdatagram*);
    private:
        LatContributionParser m_lci;
    };


    int EbfDatagramParser::process(LATdatagram* datagram)
    {
        m_lci.iterate(datagram);

        return m_lci.status();
    }

}
#endif
