#ifndef ldfReader_LATDATA_H
#define ldfReader_LATDATA_H

#include "TowerData.h"
#include "AcdDigi.h"
#include "GemData.h"
#include "EventSummaryData.h"
#include "DiagnosticData.h"
#include "OswData.h"
#include "AemData.h"
#include "ErrData.h"
#include <map>

/** @class LatData
@brief Singleton class to provide global access to LAT data for one event

$Header
*/

namespace ldfReader {

    class LatData {
    public:

        typedef enum {
            GOOD = 0,
            EVTSEQ = 1,
            PACKETERROR = 4,
            SUMMARYERROR = 8
        } EventFlags;

        static LatData *instance();
        ~LatData() { clearTowers(); };

    protected:
        LatData();

    public:
        void setSummary(unsigned summary) { m_summaryData.setSummary(summary); };

        void setTime(unsigned sec, unsigned nanosec) { m_summaryData.setTime(sec, nanosec); }
        void setPpcTimeBase(unsigned upperWord, unsigned lowerWord) { m_summaryData.setPpcTimeBase(upperWord, lowerWord); };
        const EventSummaryData& summaryData() { return m_summaryData;};

        void setRunId(unsigned int runId) { m_runId = runId; };
        unsigned int runId() const { return m_runId; };

        void setGem(const GemData& gem) { m_gem = gem; };
        GemData& getGem() { return m_gem; };
        const GemData& getGem() const { return m_gem; };

        void setOsw(const OswData& osw) { m_osw = osw; };
        OswData& getOsw() { return m_osw; };
        const OswData& getOsw() const { return m_osw; };

        void setAem(const AemData& aem) { m_aem = aem; };
        AemData& getAem() { return m_aem; };
        const AemData& getAem() const { return m_aem; };

        void setErr(const ErrData& err) { m_err = err; };
        ErrData& getErr() { return m_err; };
        const ErrData& getErr() const { return m_err; };

        void setFormatIdentity(unsigned id) { m_formatIdentity = id; };
        unsigned getFormatIdentity() const { return m_formatIdentity; };

        const DiagnosticData* diagnostic() const { return &m_diagnostic; };
        DiagnosticData* diagnostic() { return &m_diagnostic; };

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

        void setBadEventSeqFlag() { m_flags |= EVTSEQ; };
        unsigned int getEventFlags() const { return m_flags; };
        bool badEventSequence() const { return (m_flags & EVTSEQ); };
        bool goodEvent() const { return (m_flags == GOOD); };
        void setPacketErrorFlag() { m_flags |= PACKETERROR; };
        void setErrorSummaryFlag() { m_flags |= SUMMARYERROR; };

        unsigned checkErrorInEventSummary();
        unsigned checkPacketError();

        unsigned packetError() const { return (m_flags & PACKETERROR); };
        unsigned errorEventSummary() const { return (m_flags & SUMMARYERROR); }

    private:

        static LatData *m_instance;
        EventSummaryData m_summaryData;
        DiagnosticData m_diagnostic;
        unsigned int m_runId;
        std::map<unsigned int, TowerData*> m_towerMap;
        std::map<const char*, AcdDigi*> m_acdCol;
        GemData m_gem;
        // Corresponds to the EBF format identity printed in LDFdump
        // Right now we "know" this is an unsigned from LATtypeId.h
        // Be better to do this in a nicer fashion
        unsigned m_formatIdentity;
        OswData m_osw;
        AemData m_aem;
        ErrData m_err;

        unsigned int m_flags;
    };
}
#endif
