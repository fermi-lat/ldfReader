#ifndef ldfReader_TKRLAYER_H
#define ldfReader_TKRLAYER_H

#include <vector>

/** @class TkrLayer
@brief Local storage of TKR layer data

Stores strips from both x and y for a given layer.  The strips are stored in separate lists
depending upon what controller was used to read it out.

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/ldfReader/data/TkrLayer.h,v 1.5 2003/08/19 04:57:21 heather Exp $

*/
namespace ldfReader {

    class TkrLayer {
    public:

        TkrLayer();
        TkrLayer(unsigned short layerNum);
        ~TkrLayer();

        void clear();

        unsigned int getNumXstrips() const { return (m_xstripsC0.size() + m_xstripsC1.size()); }
        unsigned int getNumYstrips() const { return (m_ystripsC0.size() + m_ystripsC1.size()); }

        void addXStripC0(unsigned short strip) { m_xstripsC0.push_back(strip); };
        void addXStripC1(unsigned short strip) { m_xstripsC1.push_back(strip); };
        void addYStripC0(unsigned short strip) { m_ystripsC0.push_back(strip); };
        void addYStripC1(unsigned short strip) { m_ystripsC1.push_back(strip); };

        const std::vector<unsigned short>& getXStripC0Col() const { return m_xstripsC0; };
        const std::vector<unsigned short>& getXStripC1Col() const { return m_xstripsC1; };
        const std::vector<unsigned short>& getYStripC0Col() const { return m_ystripsC0; };
        const std::vector<unsigned short>& getYStripC1Col() const { return m_ystripsC1; };

        unsigned short getLayerNum() const{ return m_layerNum; };

        unsigned int getX_ToT(unsigned short end) const { return m_xToT[end]; };
        void setX_ToT(unsigned short end, unsigned int tot) { m_xToT[end] = tot; };
        unsigned int getY_ToT(unsigned short end) const { return m_yToT[end]; };
        void setY_ToT(unsigned short end, unsigned int tot) { m_yToT[end] = tot; };



    private:

        unsigned short m_layerNum;
        unsigned int m_xToT[2], m_yToT[2];

        std::vector<unsigned short> m_xstripsC0;
        std::vector<unsigned short> m_xstripsC1;
        std::vector<unsigned short> m_ystripsC0;
        std::vector<unsigned short> m_ystripsC1;

    };
}
#endif
