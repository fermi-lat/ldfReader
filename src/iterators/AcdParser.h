#ifndef AcdParser_H
#define AcdParser_H 1

// $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/AcdParser.h,v 1.1.1.1 2004/04/15 20:02:22 heather Exp $

// Online EBF library includes
#include "AEMcontributionIterator.h"
#include "EBFevent.h"
// ldfReader includes
#include "ldfReader/data/TowerData.h"
// 
#include <string>

/** @class AcdParser
* @brief The specific callbacks for the CAL to handle parsing of the data.
* 
* The log method is called during the iteration of the CAL data so that the contents can
* be stored in the ldfReader object for use by ldfReader's clients.
*/

namespace ldfReader {

    class AcdParser : public AEMcontributionIterator
    {
    public:
        AcdParser(EBFevent* event,
            AEMcontribution* contribution,
            const char* prefix);

        virtual ~AcdParser() {}

        void parse();

        virtual void header(unsigned cable, AEMheader hdr);
        virtual void pha(unsigned cable, unsigned channel, ACDpha p);

    private:
        const char *m_prefix;
    };
}
#endif

