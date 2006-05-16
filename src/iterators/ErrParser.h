#ifndef LdfReader_ErrParser_H
#define LdfReader_ErrParser_H 1

#include "ERRcontributionIterator.h"
#include "ldfReader/data/LatData.h"

/** @class ErrParser 
@brief Provides callbacks for Error data .
$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/ErrParser.h,v 1.4 2006/04/07 16:46:49 heather Exp $
*/
namespace ldfReader {
    class ErrParser : public ERRcontributionIterator
    {
    public:
        ErrParser(EBFevent* event, TEMcontribution *contrib, unsigned dataStart,
                  const char* prefix) :
            ERRcontributionIterator(event, contrib),
            m_prefix(prefix)  { offset(dataStart); }

        virtual ~ErrParser() {}

        // definable error handlers
        virtual int gcccError (unsigned tower, unsigned gccc, GCCCerror err);
        virtual int gtccError (unsigned tower, unsigned gtcc, GTCCerror err);
        virtual int phaseError (unsigned tower, unsigned short err);
        virtual int timeoutError (unsigned tower, unsigned short err);
        // TKR errors
        virtual int gtrcPhaseError (unsigned tower, unsigned gtcc, 
                                   unsigned gtrc, GTRCerror err);
        virtual int gtfePhaseError (unsigned tower, unsigned gtcc, 
                               unsigned gtrc,
                               unsigned short err1, unsigned short err2,
                               unsigned short err3, unsigned short err4, 
                               unsigned short err5);
        virtual int gtccFIFOerror (unsigned tower, unsigned gtcc, 
                              unsigned short err);
        virtual int gtccTMOerror        (unsigned tower, unsigned gtcc);
        virtual int gtccHDRParityError  (unsigned tower, unsigned gtcc);
        virtual int gtccWCParityError   (unsigned tower, unsigned gtcc);
        virtual int gtrcSummaryError    (unsigned tower, unsigned gtcc);
        virtual int gtccDataParityError (unsigned tower, unsigned gtcc);


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
        const char* m_prefix;
    };

}
#endif

