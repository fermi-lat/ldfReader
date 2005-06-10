#ifndef ldfReader_ErrData_H
#define ldfReader_ErrData_H

#include <vector>

//#include "EventSummaryCommon.h"

namespace ldfReader {

    /** @class ErrData
      * @brief Local storage of error data
      * $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/ErrData.h,v 1.8 2005/03/15 19:57:33 heather Exp $
    */
    class ErrData {
    public:

        ErrData() { clear(); };
        ErrData(const ErrData& error) { 
            m_lenInBytes = error.m_lenInBytes; 
            m_exist = error.m_exist; };
        ~ErrData() { clear(); };

        void clear() { 
            m_lenInBytes = 0; 
            m_cal = 0;
            m_tkr = 0;
            m_phs = 0;
            m_tmo = 0;
            m_exist = false; };

       void print() const {
           if (!exist()) {
               printf("No Error Data\n");
               return;
           }
           printf("Error Data:\n\n");
           printf("cal: %u tkr: %u \n", m_cal, m_tkr);
           printf("phs: %u tmo: %u \n", m_phs, m_tmo);
        }


        void setExist() { m_exist = true; };
        bool exist() const { return m_exist; };

        void initLength(unsigned long length) { m_lenInBytes = length; };
        unsigned long lenInBytes() const { return m_lenInBytes; };

        void initPacketError(unsigned packetError) { m_packetError=packetError; };
        unsigned packetError() const { return m_packetError; };
   
        // How to unpack these bits is discussed in section 4.4 of the TEM doc

        // bit  3  2  1  0
        // GCCC 1  3  2  0 
        unsigned short cal() const { return m_cal; };
        void initCal(unsigned short cal) { m_cal = cal; };

        // bit  7  6  5  4  3  2  1  0
       //  GTCC 4  1  5  0  7  2  6  3 
        unsigned short tkr() const { return m_tkr; };
        void initTkr(unsigned short tkr) { m_tkr = tkr; };
        
        bool phs() const { return (m_phs && 1); };
        void initPhs(unsigned short phs) { m_phs = phs; };

        bool tmo() const { return (m_tmo && 1); };
        void initTmo(unsigned short tmo) { m_tmo = tmo; };

    private:

        // denotes if this contribution exists for this event
        bool m_exist;

        unsigned m_packetError;
        unsigned long m_lenInBytes;

        // the four categories in the error summary word
        unsigned short m_cal;  // CAL
        unsigned short m_tkr;  // TKR
        unsigned short m_phs;  // Phase Error
        unsigned short m_tmo;  // Timeout Error
    };
} // end namespace
#endif
