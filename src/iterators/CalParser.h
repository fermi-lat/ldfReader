#ifndef CalParser_H
#define CalParser_H 1

// $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/CalParser.h,v 1.4 2004/02/14 05:14:08 heather Exp $

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

    class CalParser : public CALcontributionIterator
    {
    public:
        CalParser(EBFevent* event,
            CALcontribution* contribution,
            const char* prefix);

        virtual ~CalParser() {}

        virtual void log(unsigned tower, unsigned layer, CALlog theLog);

        void parse();

    private:
        const char *m_prefix;
    };
}
#endif

