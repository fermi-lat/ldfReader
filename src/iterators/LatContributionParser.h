#ifndef LdfReader_LatContributionParser_H
#define LdfReader_LatContributionParser_H 1

#include "LATcontributionIterator.h"
#include "EbfEventParser.h"
#include "EBFevent.h"

/** @class LatContributionIterator
@brief Provides callbacks for each component.
$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/LatContributionIterator.h,v 1.3 2003/11/11 20:22:04 heather Exp $
*/
namespace ldfReader {
    class LatContributionParser : public LATcontributionIterator
    {
    public:
        LatContributionParser() : LATcontributionIterator()  {}
        virtual ~LatContributionParser() {}

        virtual int EBF(EBFevent* start, EBFevent* end);
    private:
        EbfEventParser m_eep;
    };

}
#endif

