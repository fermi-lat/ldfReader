#ifndef LdfReader_OswParser_H
#define LdfReader_OswParser_H 1

#include "OSWcontributionIterator.h"
#include "ldfReader/data/LatData.h"

/** @class OswParser 
@brief Provides callbacks for OSW .
$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/LatContributionParser.h,v 1.1.1.1 2004/04/15 20:02:23 heather Exp $
*/
namespace ldfReader {
    class OswParser : public OSWcontributionIterator
    {
    public:
        OswParser(EBFevent* event, OSWcontribution *contrib) :
            OSWcontributionIterator(event, contrib)  {}

        virtual ~OswParser() {}

        virtual int OSWtime(const struct timespec* timeStamp, 
                            const OSWtimeBase* timeBase) const {

            ldfReader::LatData::instance()->setTime(timeStamp->tv_sec, timeStamp->tv_nsec);
            if (contribution()->payloadSize() > 8) { // Timebase does not exist in old data
                ldfReader::LatData::instance()->setPpcTimeBase(timeBase->upper(), timeBase->lower());
            }
            return 0;
        };

        virtual int OSW_UDF(EBFevent* event, OSWcontribution* contribution)const {
            fprintf(stderr, "Undefined OSW contribution encountered with identity %08x\n", contribution->identity().value());  
        } ;

    private:
    };

}
#endif

