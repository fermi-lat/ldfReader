#ifndef LdfReader_ErrParser_H
#define LdfReader_ErrParser_H 1

#include "ERRcontributionIterator.h"
#include "ldfReader/data/LatData.h"

/** @class ErrParser
@brief Provides callbacks for Error data .
$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/ErrParser.h,v 1.6 2008/10/03 03:39:17 heather Exp $
*/
namespace ldfReader {
    class ErrParser : public virtual ERRcontributionIterator
    {
    public:
        ErrParser(const char* prefix) : m_prefix(prefix)  { }

        virtual ~ErrParser() {}

        // definable error handlers
        virtual int summary            (ErrorSummary);
        // ACD errors
        virtual int gaemTMOerror       (unsigned cable);
        virtual int gaemHDRParityError (unsigned cable);
        virtual int gaemPHAParityError (unsigned cable, unsigned channel);
        // TEM errors
        virtual int gcccError          (unsigned twr, unsigned cc, GCCCerror);
        virtual int gtccError          (unsigned twr, unsigned cc, GTCCerror);
        virtual int phaseError         (unsigned twr, unsigned short ew);
        virtual int timeoutError       (unsigned twr, unsigned short ew);
        // TKR errors
        virtual int gtrcPhaseError     (unsigned twr, unsigned cc, unsigned rc,
                                        GTRCerror);
        virtual int gtfePhaseError     (unsigned twr, unsigned cc, unsigned rc,
                                        unsigned short ew1, unsigned short ew2,
                                        unsigned short ew3, unsigned short ew4,
                                        unsigned short ew5);
        virtual int gtccFIFOerror      (unsigned twr, unsigned cc, unsigned rc,
                                        unsigned short ew);
        virtual int gtccTMOerror       (unsigned twr, unsigned cc, unsigned rc);
        virtual int gtccHDRParityError (unsigned twr, unsigned cc, unsigned rc);
        virtual int gtccWCParityError  (unsigned twr, unsigned cc, unsigned rc);
        virtual int gtrcSummaryError   (unsigned twr, unsigned cc, unsigned rc);
        virtual int gtccDataParityError(unsigned twr, unsigned cc, unsigned rc);

        virtual int handleError(const TEMcontribution* contribution, 
                                unsigned code,
                                unsigned p1=0, unsigned p2=0) const;

   protected :
        virtual void _handleErrorCommon() const = 0;


    private:
        static const unsigned m_gccc[];
        static const unsigned m_gtcc[];
        static const char* m_pn[];
        static const char* m_xy[];
        static const unsigned m_layer[];

        const char* m_prefix;
    };

}
#endif

