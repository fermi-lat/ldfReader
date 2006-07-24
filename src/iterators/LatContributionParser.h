#ifndef LdfReader_LatContributionParser_H
#define LdfReader_LatContributionParser_H 1

#include "LATcontributionIterator.h"
#include "EbfEventParser.h"
#include "EBFevent.h"
#include "AdfParser.h"

/** @class LatContributionIterator
@brief Provides callbacks for each component.
$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/LatContributionParser.h,v 1.3 2005/04/05 21:23:39 heather Exp $
*/
namespace ldfReader {
    class LatContributionParser : public LATcontributionIterator
    {
    public:
        LatContributionParser() : LATcontributionIterator()  {}
        virtual ~LatContributionParser() {}

        virtual int EBF(EBFevent* start, EBFevent* end);
        // Over-ride so we can skip UDF and not generate an error
        virtual int UDF(LATcontribution* start, LATcontribution* end);
        virtual int handleError(LATcontribution *contrib, unsigned code,
                    unsigned p1=0, unsigned p2=0) const;

    private:
        EbfEventParser m_eep;
        AdfParser m_adf;
    };

}
#endif

