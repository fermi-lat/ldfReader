#ifndef LdfReader_ErrParser_H
#define LdfReader_ErrParser_H 1

#include "ERRcontributionIterator.h"
#include "ldfReader/data/LatData.h"

/** @class ErrParser 
@brief Provides callbacks for Error data .
$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/ErrParser.h,v 1.2 2004/07/30 04:37:46 heather Exp $
*/
namespace ldfReader {
    class ErrParser : public ERRcontributionIterator
    {
    public:
        ErrParser(EBFevent* event, TEMcontribution *contrib, unsigned dataStart) :
            ERRcontributionIterator(event, contrib)  { offset(dataStart); }

        virtual ~ErrParser() {}


    private:
    };

}
#endif

