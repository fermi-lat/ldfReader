#ifndef CalParser_H
#define CalParser_H 1

// $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/CalParser.h,v 1.2 2005/04/05 21:23:39 heather Exp $

// Online EBF library includes
#include "CALcontributionIterator.h"
#include "EBFevent.h"
// ldfReader includes
#include "ldfReader/data/TowerData.h"
// 
#include <string>

/** @class CalParser
* @brief The specific callbacks for the CAL to handle parsing of the data.
* 
* The log method is called during the iteration of the CAL data so that the contents can
* be stored in the ldfReader object for use by ldfReader's clients.
*/

namespace ldfReader {

    class CalParser : public virtual CALcontributionIterator
    {
    public:
        CalParser( const char* prefix);

        virtual ~CalParser() {}

        virtual void log(unsigned tower, unsigned layer, CALlog theLog);

    virtual int handleError(CALcontribution*, unsigned code, unsigned p1=0, unsigned p2=0) const;

   protected :
        virtual void _handleErrorCommon() const = 0;


    private:
        const char *m_prefix;
    };
}
#endif

