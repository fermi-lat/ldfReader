#ifndef LdfReader_ErrParser_H
#define LdfReader_ErrParser_H 1

#include "ERRcontributionIterator.h"
#include "ldfReader/data/LatData.h"

/** @class ErrParser 
@brief Provides callbacks for Error data .
$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/ErrParser.h,v 1.3 2006/04/01 09:33:38 heather Exp $
*/
namespace ldfReader {
    class ErrParser : public ERRcontributionIterator
    {
    public:
        ErrParser(EBFevent* event, TEMcontribution *contrib, unsigned dataStart) :
            ERRcontributionIterator(event, contrib)  { offset(dataStart); }

        virtual ~ErrParser() {}


        virtual int handleError(TEMcontribution* contribution, unsigned code, 
                                unsigned p1=0, unsigned p2=0) const {

           switch (code) { 
               case ERRcontributionIterator::ERR_PastEnd: 
                    fprintf(stderr,
                      "ERRcontributionIterator.handleError:  Iterated past the "
                      "end of the contribution by %d words at stage %d",
                       p1, p2);
                   fprintf(stderr, " Event: %llu Apid: %d\n", 
                      ldfReader::LatData::instance()->eventId(),
                      ldfReader::LatData::instance()->getCcsds().getApid());
                   return 0; 
               default: 
                   fprintf(stderr, 
                   "MyERRiterator::handleError:  Somehow an error occured. \n ");
                    fprintf(stderr, "  code=%d, p1=%d, p2=%d\n", code, p1, p2);
                    fprintf(stderr, " Event: %llu Apid: %d\n", 
                        ldfReader::LatData::instance()->eventId(), 
                        ldfReader::LatData::instance()->getCcsds().getApid());
               }
           return 0;
        }


    private:
    };

}
#endif

