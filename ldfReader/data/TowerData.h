#ifndef ldfReader_TOWERDATA_H
#define ldfReader_TOWERDATA_H

#include "TkrLayer.h"
#include "CalDigi.h"

#include <vector>
#include <map>
#include <algorithm>

/** @class TowerData
@brief Local storage of tower data for one event

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/TowerData.h,v 1.5 2004/02/14 05:11:04 heather Exp $
*/

namespace ldfReader {

    class TowerData {
    public:

        TowerData() { m_tkrMap.clear(); };
        TowerData(unsigned short id) { m_tkrMap.clear(); m_calDigiCol.clear(); m_towerId = id; };
        ~TowerData() { clear(); };

        void clear();

        unsigned short getTowerId() const { return m_towerId; };

        const std::map<unsigned int, TkrLayer*>* getTkrLayerMap() const { return &m_tkrMap; };

        TkrLayer* getTkrLayer(unsigned int id);

        const std::map<unsigned int, CalDigi*>& getCalDigiCol() { return m_calDigiCol;}

        void addTkrLayer(TkrLayer *layer) { m_tkrMap[layer->getLayerNum()] = layer; };

        void addCalDigi(CalDigi *log) { m_calDigiCol[log->getLayerNum()*100+log->getColumnNum()] = log; };

        CalDigi* getCalDigi(unsigned int layer, unsigned int col);

    private:

        unsigned short m_towerId;
        std::map<unsigned int, TkrLayer*> m_tkrMap;
        //std::vector<CalDigi> m_calDigiCol;
        std::map<unsigned int, CalDigi*> m_calDigiCol;

    };
}

#endif
