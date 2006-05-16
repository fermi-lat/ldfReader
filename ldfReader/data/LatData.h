#ifndef ldfReader_LATDATA_H
#define ldfReader_LATDATA_H

#include "TowerData.h"
#include "AcdDigi.h"
#include "GemData.h"
#include "EventSummaryData.h"
#include "OswData.h"
#include "AemData.h"
#include <map>

#include "enums/EventFlags.h"

#include "lsfData/LsfMetaEvent.h"
#include "lsfData/LsfCcsds.h"

/** @class LatData
@brief Singleton class to provide global access to LAT data for one event

$Header
*/

namespace ldfReader {

    class LatData {
    public:

        static LatData *instance();
        ~LatData() { clearTowers(); };

    protected:
        LatData();

    public:

        void print() const;

        const lsfData::MetaEvent& getMetaEvent() const {
               return m_metaEvent; }

        lsfData::MetaEvent* getMetaEventPtr() {
               return &m_metaEvent; }

        const lsfData::LsfCcsds& getCcsds() const { return m_ccsds; }

        lsfData::LsfCcsds* getCcsdsPtr() { return &m_ccsds; };

        void setSummary(unsigned summary) { m_summaryData.setSummary(summary); };

        void setTime(unsigned sec, unsigned nanosec) { m_summaryData.setTime(sec, nanosec); }
        void setPpcTimeBase(unsigned upperWord, unsigned lowerWord) { m_summaryData.setPpcTimeBase(upperWord, lowerWord); };
        const EventSummaryData& summaryData() { return m_summaryData;};

        void setRunId(unsigned int runId) { m_runId = runId; };
        unsigned int runId() const { return m_runId; };

        void setEventId(unsigned long long eventId) { m_eventId = eventId; };
        unsigned long long eventId() const { return m_eventId; };

        void setGem(const GemData& gem) { m_gem = gem; };
        GemData& getGem() { return m_gem; };
        const GemData& getGem() const { return m_gem; };

        void setOsw(const OswData& osw) { m_osw = osw; };
        OswData& getOsw() { return m_osw; };
        const OswData& getOsw() const { return m_osw; };

        void setAem(const AemData& aem) { m_aem = aem; };
        AemData& getAem() { return m_aem; };
        const AemData& getAem() const { return m_aem; };

        void setFormatIdentity(unsigned id) { m_formatIdentity = id; };
        unsigned getFormatIdentity() const { return m_formatIdentity; };

        const TowerData* getTower(unsigned int id) const;
        TowerData* getTower(unsigned int id);

        const std::map<unsigned int, TowerData*>& getTowerMap() { return m_towerMap; };

        void addTower(TowerData *tower) { m_towerMap[tower->getTowerId()] = tower; };

        const std::map<const char*, AcdDigi*>& getAcdCol() const { return m_acdCol; };
        void addAcd(AcdDigi *acd) { m_acdCol[acd->getTileName()] = acd; };
        const AcdDigi* getAcd(const char *name) const;
        AcdDigi* getAcd(const char *name);

        void clearTowers();

        /// Checks all of the event Sequence numbers across all contributions
        /// If they do not all match - we should set the appropriate flag in 
        /// badEvent
        bool eventSeqConsistent() const;

        void setBadEventSeqFlag() { m_flags |= enums::EVTSEQ; };
        unsigned int getEventFlags() const { return m_flags; };
        bool badEventSequence() const { return (m_flags & enums::EVTSEQ); };
        bool goodEvent() const { return (m_flags == enums::GOOD); };
        void setPacketErrorFlag() { m_flags |= enums::PACKETERROR; };
        void setTemErrorFlag() { m_flags |= enums::SUMMARYERROR; };
        void setTrgParityErrorFlag() { m_flags |= enums::TRGPARITYERROR; };
        void setBadLdfStatusFlag() { m_flags |= enums::BADLDFSTATUS; };
        void setGtrcPhaseFlag() { m_flags |= enums::GTRCPHASE; };
        void setGtfePhaseFlag() { m_flags |= enums::GTFEPHASE; };
        void setGtccFifoFlag() { m_flags |= enums::GTCCFIFO; };
        void setGtccHdrParityFlag() { m_flags |= enums::GTCCHDRPARITY; };
        void setGtccWcParityFlag() { m_flags |= enums::GTCCWCPARITY; };
        void setGtrcSummaryFlag() { m_flags |= enums::GTRCSUMMARY; };
        void setGtccDataParityFlag() { m_flags |= enums::GTCCDATAPARITY; };
        void setGtccTimeoutFlag() { m_flags |= enums::GTCCTIMEOUT; };
        void setGcccErrorFlag() { m_flags |= enums::GCCCERROR; };
        void setGtccErrorFlag() { m_flags |= enums::GTCCERROR; };
        void setPhaseErrorFlag() { m_flags |= enums::PHASEERROR; };
        void setTimeoutErrorFlag() { m_flags |= enums::TIMEOUTERROR; };

        unsigned checkTemError();
        unsigned checkPacketError();
        unsigned checkTrgParityError();
        bool checkAemError();

        unsigned packetError() const { return (m_flags & enums::PACKETERROR); }

        unsigned errorEventSummary() const { 
            return (m_flags & enums::SUMMARYERROR); }
        unsigned trgParityError() const { 
            return (m_flags & enums::TRGPARITYERROR); }
        unsigned badLdfStatus() const { 
            return (m_flags & enums::BADLDFSTATUS); }
        unsigned gtrcPhase() const { 
            return (m_flags & enums::GTRCPHASE); }
        unsigned gtfePhase() const { 
            return (m_flags & enums::GTFEPHASE); }
        unsigned gtccFifo() const { 
            return (m_flags & enums::GTCCFIFO); }
        unsigned gtccHdrParity() const { 
            return (m_flags & enums::GTCCHDRPARITY); }
        unsigned gtccWcParity() const { 
            return (m_flags & enums::GTCCWCPARITY); }
        unsigned gtrcSummary() const { 
            return (m_flags & enums::GTRCSUMMARY); }
        unsigned gtccDataParity() const { 
            return (m_flags & enums::GTCCDATAPARITY); }
        unsigned gtccTimeout() const { 
            return (m_flags & enums::GTCCTIMEOUT); }
        unsigned gcccError() const { 
            return (m_flags & enums::GCCCERROR); }
        unsigned gtccError() const { 
            return (m_flags & enums::GTCCERROR); }
        unsigned phaseError() const { 
            return (m_flags & enums::PHASEERROR); }
        unsigned timeoutError() const { 
            return (m_flags & enums::TIMEOUTERROR); }

        void setEventSizeInBytes(unsigned long size){ m_eventSize = size; };
        unsigned long eventSizeInBytes() const { return m_eventSize; };

        void setTimeInSecTds(double t) { m_timeInSecTds = t; };
        double getTimeInSecTds() const { return m_timeInSecTds; };

    private:

        static LatData *m_instance;
        EventSummaryData m_summaryData;
        unsigned int m_runId;
        unsigned long long m_eventId;
        std::map<unsigned int, TowerData*> m_towerMap;
        std::map<const char*, AcdDigi*> m_acdCol;
        GemData m_gem;
        // Corresponds to the EBF format identity printed in LDFdump
        // Right now we "know" this is an unsigned from LATtypeId.h
        // Be better to do this in a nicer fashion
        unsigned m_formatIdentity;
        OswData m_osw;
        AemData m_aem;

        unsigned int m_flags;

        unsigned long m_eventSize;

        lsfData::MetaEvent m_metaEvent;

        lsfData::LsfCcsds m_ccsds;

        double m_timeInSecTds;
    };
}
#endif
