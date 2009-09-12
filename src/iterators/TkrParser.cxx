#ifndef TKRPARSER_CXX
#define TKRPARSER_CXX 1

/** @file TkrParser.cxx
@brief Implementation of the TkrParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/TkrParser.cxx,v 1.11 2009/08/31 18:17:58 jrb Exp $
*/
#include <stdio.h> // included for TKRcontributionIterator.h
// Online EBF library includes
#include "TKRstrip.h"
// ldfReader includes
//#include "TkrParser.h"
#include "ldfReader/iterators/TkrParser.h"
#include "ldfReader/LdfException.h"
#include "ldfReader/data/LatData.h"
#include "../EbfDebug.h"

namespace {
    /*
    Following utilities are based on Eduardo's tables. Here is
    EM version:
    Offline  CC  RC   Measured   Front end chips read by CC,RC combination
    Layer           Coordinate    (multiply by 64 to get the strip ID's)

    7     6    1       y3     0 to 7
    7     7    1       y3     9 to 23
    6     4    1       x3     12 to 23
    6     5    1       x3     0 to 11
    5     0    1       x2     0 to 14
    5     1    1       x2     16 to 23
    4     2    1       y2     23
    4     3    1       y2     0 to 3
    3     6    0       y1     0 to 20
    3     7    0       y1     22 to 23
    2     4    0       x1     12 to 23
    2     5    0       x1     0 to 11
    1     0    0       x0     0 to 11
    1     1    0       x0     12 to 23
    0     2    0       y0     high
    0     3    0       y0     low

    For equivalent LAT information, see
    http://www.slac.stanford.edu/~jrb/glast/electGeo.shtml
    */

    // setup some constants

    // This is used when unpacking the data to determine if this is an
    // X or Y oriented layer
    static const char* xyByPlane[] = {"y", "x", "x", "y"};
    // These constants are used for debug print statements
    static const unsigned gtcc[]  = {6, 3, 7, 2, 5, 0, 4, 1};
    // Because of rotation in assembled EM, not sure that the
    // label and xy arrays are correct.  (jrb  8 May 2003)
    static const char* label[] = {"Left", "Right", "Right", "Left",
        "Right", "Left", "Left", "Right"};
    static const char* xy[]    = {"x", "x", "x", "x", "y", "y", "y", "y"};
} // end default namespace

namespace ldfReader {

    TkrParser::TkrParser(const char* prefix) :
        TKRcontributionIterator(), m_prefix(prefix)
    {
    }


    // layerEnd uses least sig. bit to keep track of low or high end readout.
    // Remainder is (uni)layer number, shifted up one
    void TkrParser::strip(unsigned tower, unsigned layerEnd, unsigned hit) {
        // Purpose and Method:  The strip data callback which stores the strip data in our
        // local static object for general access to ldfReader clients.

        using namespace ldfReader;

        // Retrieve the tower object we wish to update with this TKR data
        /* unsigned int towerId =*/ LATPcellHeader::source(contribution()->header());
        LatData* curLatData = LatData::instance();
        TowerData* tData = curLatData->getTower(tower);
        if (!tData) {
            tData = new TowerData(tower);
            curLatData->addTower(tData);
        }
            // HMK 09152008 Not sure we need this
        ldfReader::TemData& tem = tData->getTem();
        if (!tem.exist()) {
             ldfReader::EventSummaryCommon summary(((EBFcontribution*)contribution())->summary());
             tem.initSummary(summary);
        //     ldfReader::TemData temNew(summary);
             //tem.summary().print();
             tem.setExist();
             tem.initPacketError(((EBFcontribution*)contribution())->packetError());
             tem.initLength(((EBFcontribution*)contribution())->length());
          //      tData->setTem(temNew);
        }
           //HMK 

        unsigned myPlane = layerEnd >> 1;
        unsigned  myLowHigh = layerEnd & 1;
        const char* myXy = xyByPlane[myPlane % 4];
        if (EbfDebug::getDebug()==EbfDebug::ALL) {
            printf("%s  %s  %d   %d   %s  %d   %02d    %02d  0x%03x = %4d\n",
                m_prefix, "TkrParser::strip ", tower, myPlane, myXy, myLowHigh,
                TKRstrip::gtfe(hit), TKRstrip::strip(hit), hit, hit);
        }

        unsigned myLayer = myPlane/2;

        ldfReader::TkrLayer *layer = tData->getTkrLayer(myLayer);
        if (!layer) {
            layer = new ldfReader::TkrLayer(myLayer);
            tData->addTkrLayer(layer);
        }

        unsigned short stripId = hit;
        if ((strcmp(myXy, "x")==0) && (myLowHigh == 0) ) {
            layer->addXStripC0(stripId);
        } else if ((strcmp(myXy, "x")==0) && (myLowHigh == 1)) {
            layer->addXStripC1(stripId);
        } else if ((strcmp(myXy, "y")==0) && (myLowHigh == 0)) {
            layer->addYStripC0(stripId);
        } else if ((strcmp(myXy, "y")==0) && (myLowHigh == 1)) {
            layer->addYStripC1(stripId);
        } else {
            fprintf(stderr, "%s  failed to find the TKR layer, view, controller combination\n", m_prefix);
            _handleErrorCommon();
        }
    }

    void TkrParser::TOT(unsigned tower, unsigned layerEnd, unsigned tot) {
        // Purpose and Method:  ToT callback that handles unpacking ToT data for storage in
        // the ldfReader static object that provides data access for ldfReader clients

        using namespace ldfReader;

        if (EbfDebug::getDebug()==EbfDebug::ALL) {
            printf("%s %s %2d %2d 0x%02x = %3d\n", m_prefix, "TOT", tower, layerEnd, tot, tot);
        }

        // Retrieve the tower or create a new TowerData object if necessary
        ldfReader::LatData* curLatData = ldfReader::LatData::instance();
        ldfReader::TowerData* tData = curLatData->getTower(tower);
        // No need to check whether TowerData exists since there can not be
        // and TOTs without strip hits and therefore must exist

        unsigned myLowHigh = layerEnd & 1;
        unsigned myPlane = layerEnd >> 1;
        const char* myXy = xyByPlane[myPlane % 4];
        unsigned myLayer = myPlane/2;

        ldfReader::TkrLayer *layer = tData->getTkrLayer(myLayer);
        if (!layer) {
            layer = new ldfReader::TkrLayer(myLayer);
            tData->addTkrLayer(layer);
        }

        if ((strcmp(myXy, "x")==0) && (myLowHigh == 0)) {
            layer->setX_ToT(0, tot);
        } else if ((strcmp(myXy, "x")==0) && (myLowHigh == 1)) {
            layer->setX_ToT(1, tot);
        } else if ((strcmp(myXy, "y")==0) && (myLowHigh == 0)) {
            layer->setY_ToT(0, tot);
        } else if ((strcmp(myXy, "y")==0) && (myLowHigh == 1)) {
            layer->setY_ToT(1, tot);
        } else {
            fprintf(stderr,
                "failed to find the TKR layer, view, controller combination\n");
            _handleErrorCommon();
        }

    }



    int TkrParser::handleError(TKRcontribution *contribution, unsigned code,
                    unsigned p1, unsigned p2) const {

        int rc = TKRcontributionIterator::handleError(contribution, code,p1,p2);
        _handleErrorCommon();
        return rc;
   }

}
#endif

