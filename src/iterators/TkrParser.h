#ifndef TKRPARSER_H
#define TKRPARSER_H 1

// $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/TkrParser.h,v 1.1.1.1 2004/04/15 20:02:23 heather Exp $

#include "TKRcontribution.h"
#include "TKRcontributionIterator.h"
#include "ldfReader/data/TowerData.h"
#include <string>

/** @class TkrParser
* @brief The specific callbacks for the TKR.
* 
* Handles the parsing of the TKR data.  The parse routine is called which initiates the 
* callbacks:  strips and ToT.  The data is then stored in our local static object which is
* used to share the data with clents of ldfReader.
*/
namespace ldfReader {
    class TkrParser : public TKRcontributionIterator
    {
    public:
        TkrParser(EBFevent*        event,
            TKRcontribution* contribution,
            ldfReader::TowerData *tData,
            const char*      prefix);

        ~TkrParser() {}

        void parse();

        // Even newer, as of 03-00-00
        // NOTE: 2nd argument in strip(..) changed from "layer" to
        //      "layerEnd".  Compatible with local copy of EBF library,
        //      but not yet with standard Online version
        void strip(unsigned tower, unsigned layerEnd, unsigned hit);
        void TOT  (unsigned tower, unsigned layerEnd, unsigned tot);

        // Unused with EBF library v3 and later
        //static void setInstrument(const std::string& ) {}

        virtual int handleError(TKRcontribution*, unsigned code, unsigned p1=0, unsigned p2=0) const;

    private:
        EBFevent *m_event;
        TKRcontribution *m_contribution;
        const char*     m_prefix;
        ldfReader::TowerData *m_tData;
    };
}
#endif

