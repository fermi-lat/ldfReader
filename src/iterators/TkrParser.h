#ifndef TKRPARSER_H
#define TKRPARSER_H 1

// $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/TkrParser.h,v 1.2 2005/04/05 21:23:39 heather Exp $

#include "TKRcontributionIterator.h"
#include "ldfReader/data/TowerData.h"
#include <string>

/** @class TkrParser
* @brief The specific callbacks for the TKR.
*
* Handles the parsing of the TKR data.  The parse routine is called which initiates the
* callbacks:  strips and ToT.  The data is then stored in our local static object which is
* used to share the data with clients of ldfReader.
*/
namespace ldfReader {
    class TkrParser : public virtual TKRcontributionIterator
    {
    public:
        TkrParser(const char* prefix);

        TkrParser(ldfReader::TowerData* tData,
                  const char*           prefix);

        virtual ~TkrParser() {}

        // Even newer, as of 03-00-00
        // NOTE: 2nd argument in strip(..) changed from "layer" to
        //      "layerEnd".  Compatible with local copy of EBF library,
        //      but not yet with standard Online version
        virtual void strip(unsigned tower, unsigned layerEnd, unsigned hit);
        virtual void TOT  (unsigned tower, unsigned layerEnd, unsigned tot);

        // Unused with EBF library v3 and later
        //static void setInstrument(const std::string& ) {}

        virtual int handleError(TKRcontribution*, unsigned code, unsigned p1=0, unsigned p2=0) const;

   protected :
        virtual void _handleErrorCommon() const = 0;


    private:
        const char*     m_prefix;
        ldfReader::TowerData *m_tData;
    };
}
#endif

