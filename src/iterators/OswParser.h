#ifndef LdfReader_OswParser_H
#define LdfReader_OswParser_H 1

#include "OSWcontributionIterator.h"
#include "ldfReader/data/LatData.h"

/** @class OswParser 
@brief Provides callbacks for OSW .
$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/OswParser.h,v 1.2 2004/07/30 04:37:46 heather Exp $
*/
namespace ldfReader {
    class OswParser : public OSWcontributionIterator
    {
    public:
        OswParser(EBFevent* event, OSWcontribution *contrib) :
            OSWcontributionIterator(event, contrib)  {}

        virtual ~OswParser() {}

/*
        virtual int OSWtime(const struct timespec* timeStamp, 
                            const OSWtimeBase* timeBase) const {

            ldfReader::LatData::instance()->setTime(timeStamp->tv_sec, timeStamp->tv_nsec);
            if (contribution()->payloadSize() > 8) { // Timebase does not exist in old data
                ldfReader::LatData::instance()->setPpcTimeBase(timeBase->upper(), timeBase->lower());
            }
            return 0;
        };
*/

        virtual int OSW_time(EBFevent*            event,
                             OSWtimeContribution* contribution) const {

            const struct timespec* ts = contribution->timeStamp();
            const OSWtimeBase*     tb = contribution->timebase();
            ldfReader::LatData::instance()->setTime(ts->tv_sec, ts->tv_nsec);
            if (tb) 
                ldfReader::LatData::instance()->setPpcTimeBase(tb->upper(), tb->lower());

            ldfReader::LatData::instance()->getOsw().setEvtSequence(contribution->evtSequence());

            return 0;

        }

        virtual int OSW_UDF(EBFevent* event, OSWcontribution* contribution)const {
            fprintf(stderr, "Undefined OSW contribution encountered with identity %08x\n", contribution->identity().value());  
            return 0;
        } ;


        virtual int handleError(OSWcontribution *contribution, unsigned code,
                                unsigned p1=0, unsigned p2=0) const {
            fprintf(stderr, "MyOSWiterator::handleError:  Somehow an error occured. \n ");
            fprintf(stderr, "  code=%d, p1=%d, p2=%d\n", code, p1, p2);
            return 0;

        }

    private:
    };

}
#endif

