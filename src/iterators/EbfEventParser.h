#ifndef EBFEVENTPARSER_H
#define EBFEVENTPARSER_H 1

#include "EBFeventIterator.h"
#include "src/iterators/LatComponentParser.h"
#include "ldfReader/data/LatData.h"

/** @class EbfEventParser
@brief Calls the appropriate routines in the Online/EBF library to start
processing the event.

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/EbfEventParser.h,v 1.1.1.1 2004/04/15 20:02:22 heather Exp $
*/
namespace ldfReader {
    class EbfEventParser : public EBFeventIterator
    {
    public:
        EbfEventParser() : EBFeventIterator() {}

        //EbfEventParser(unsigned char* buffer, size_t size, bool swap) :
        // EBFeventIterator(buffer, size, swap) {}
        virtual ~EbfEventParser() {}

        virtual int process(EBFevent* event);

       virtual int handleError(EBFevent *evt, unsigned code, unsigned p1=0, unsigned p2=0) const;

    };


}
#endif