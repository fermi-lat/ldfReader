#ifndef LdfReader_ErrParser_H
#define LdfReader_ErrParser_H 1

#include "ERRcontributionIterator.h"
#include "ldfReader/data/LatData.h"

/** @class ErrParser 
@brief Provides callbacks for Error data .
$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/ErrParser.h,v 1.1 2004/08/04 21:44:20 heather Exp $
*/
namespace ldfReader {
    class ErrParser : public ERRcontributionIterator
    {
    public:
        ErrParser(EBFevent* event, TEMcontribution *contrib, unsigned dataStart) :
            ERRcontributionIterator(event, contrib)  { offset(dataStart); }

        virtual ~ErrParser() {}


        virtual int handleError(TEMcontribution* contribution, unsigned code, unsigned p1=0, unsigned p2=0) const {

            fprintf(stderr, "MyERRiterator::handleError:  Somehow an error occured. \n ");
            fprintf(stderr, "  code=%d, p1=%d, p2=%d\n", code, p1, p2);
            return 0;
        }


    private:
    };

}
#endif

