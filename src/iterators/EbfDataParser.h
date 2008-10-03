#ifndef EBFDATAPARSER_H
#define EBFDATAPARSER_H 1

#include "EBFiteratorBase.h"
#include "GLTcontribution.h"
#include "AcdParser.h"
#include "AdfParser.h"
#include "CalParser.h"
#include "TkrParser.h"
#include "DiagnosticParser.h"
#include "ErrParser.h"
#include "OswParser.h"

/** @class EbfDataParser
@brief Provides callbacks for each component.
$Header: $
*/
namespace ldfReader {
    class EbfDataParser : public AcdParser,
                          public AdfParser,
                          public CalParser,
                          public TkrParser,
                          public DiagnosticParser,
                          public ErrParser,
                          public OswParser,
                          public virtual EBFiteratorBase
    {
    public:

        typedef enum {
            ID_ORG = 0x101f0010,
            ID_WITH_OSW = 0x102f0010
        } EBF_FORMAT_ID;


        EbfDataParser(const char* prefix,
                      ACDmap* a = 0, CALmap* c = 0, TKRmap* t = 0);
        virtual ~EbfDataParser() {}

        //! Virtual base class methods that are implemented by this class
        //! These are not meant to be reimplemented by subclasses
        virtual int UDF(EBFevent*, EBFcontribution*);
        virtual int UDF(LATcontribution* event, LATcontribution* end);
        virtual int OSW_time(const EBFevent*, const OSWtimeContribution*) const;
        virtual int OSW_UDF(const EBFevent*, const OSWcontribution*) const;
        virtual int GLT(EBFevent*, GLTcontribution*);
        virtual int GEM(EBFevent*, GEMcontribution*);

        virtual int commonComponentData(EBFcontribution*);

        //! Virtual methods reimplementing error handling.
        virtual int handleError(const EBFevent*,        unsigned code,
                                unsigned p1=0, unsigned p2=0) const;
        virtual int handleError(const EBFcontribution*, unsigned code,
                                unsigned p1=0, unsigned p2=0) const;
        virtual int handleError(const AEMcontribution*, unsigned code,
                                unsigned p1=0, unsigned p2=0) const;
        virtual int handleError(const TEMcontribution*, unsigned code,
                                unsigned p1=0, unsigned p2=0) const;
        virtual int handleError(const CALcontribution*, unsigned code,
                                unsigned p1=0, unsigned p2=0) const;
        virtual int handleError(const TKRcontribution*, unsigned code,
                                unsigned p1=0, unsigned p2=0) const;
    protected:
        virtual void _handleErrorCommon() const;

    private:
        AdfParser m_adf;
        const char *m_prefix;
    };
}
#endif
