#ifndef ldfReader_OswData_H
#define ldfReader_OswData_H

#include <vector>

#include "EventSummaryCommon.h"

namespace ldfReader {

    /** @class OswData
      * @brief Local storage of OSW data
      * $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/OswData.h,v 1.8 2005/01/28 23:02:44 heather Exp $
    */
    class OswData {
    public:

        OswData() { clear(); };
        OswData(const OswData& osw) { 
            clear();
            m_summary = osw.m_summary; 
            m_lenInBytes = osw.m_lenInBytes; 
            m_evtSequence = osw.m_evtSequence;
            m_exist = osw.m_exist; };
 
        OswData(const EventSummaryCommon& summary) { clear(); m_summary = summary; };
        ~OswData() { clear(); };

        void clear() { 
            m_summary.setSummary(0); 
            m_lenInBytes = 0; 
            m_packetError = 0;
            m_evtSequence = 0;
            m_exist = false;};

        void print() const {
            if(!exist()) {
                printf("No OSW\n");
                return;
            }
            printf("OSW: \n");
            m_summary.print();
            printf("EvtSequence %u\n", m_evtSequence);
            printf("Len: %lu\n\n", m_lenInBytes);
        }

        const EventSummaryCommon& summary() const { return m_summary; };
        void initSummary(unsigned summary) { m_summary.setSummary(summary);};

       void setExist() { m_exist = true; };
       bool exist() const { return m_exist; };

       void initLength(unsigned long length) { m_lenInBytes = length; };
       unsigned long lenInBytes() const { return m_lenInBytes; };

       void initPacketError(unsigned packetError) { m_packetError=packetError; };
       unsigned packetError() const {  return m_packetError; };

       void setEvtSequence(unsigned evtSequence) { m_evtSequence = evtSequence; };
       unsigned evtSequence() const { return m_evtSequence; };

    private:

        // Store the event sequence number for this contribution
        EventSummaryCommon m_summary;
        bool m_exist;

        unsigned m_packetError;
        unsigned long m_lenInBytes;

        // New as of v05-06-01 32bit event sequence number
        unsigned int m_evtSequence;
    };
} // end namespace
#endif
