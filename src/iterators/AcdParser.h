#ifndef AcdParser_H
#define AcdParser_H 1

// $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/AcdParser.h,v 1.5 2006/07/28 23:26:24 heather Exp $

// Online EBF library includes
#include "AEMcontributionIterator.h"
#include "EBFevent.h"
// ldfReader includes
//#include "ldfReader/data/TowerData.h"
// 
#include <string>

/** @class AcdParser
* @brief The specific callbacks for the CAL to handle parsing of the data.
* 
* The log method is called during the iteration of the CAL data so that the contents can
* be stored in the ldfReader object for use by ldfReader's clients.
*/

namespace ldfReader {

    class AcdParser : public virtual AEMcontributionIterator
    {
    public:
        AcdParser(const char* prefix);

        virtual ~AcdParser() {}

        //void parse();

        virtual void header(unsigned cable, AEMheader hdr);
        virtual void pha(unsigned cable, unsigned channel, ACDpha p);

        virtual int  gaemTMOerror(unsigned cable);
        virtual int  gaemHDRParityError(unsigned cable, AEMheader hdr);
        virtual int  gaemPHAParityError(unsigned cable, unsigned channel, ACDpha p);


       virtual int commonComponentData(EBFcontribution*) = 0;

 virtual int handleError(AEMcontribution *contribution, unsigned code, unsigned p1=0, unsigned p2=0) const;

        void setCurHeader(const AEMheader &header) { m_curHeader = header; };
        const AEMheader& getCurHeader() const { return m_curHeader; };

        unsigned int constructTileNum(const char *name);

        int lookup(const char* name, std::string& newName, char &side);

   protected :
        virtual void _handleErrorCommon() const = 0;

    private:
        const char *m_prefix;
        AEMheader m_curHeader;
    };
}
#endif

