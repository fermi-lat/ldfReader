#ifndef TKRPARSER_CXX
#define TKRPARSER_CXX 1

/** @file TkrParser.cxx
@brief Implementation of the TkrParser class

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/iterators/TkrParser.cxx,v 1.13 2004/02/13 05:32:12 heather Exp $
*/
#include <stdio.h> // included for TKRcontributionIterator.h
// Online EBF library includes
#include "TKRstrip.h"
// ldfReader includes
#include "TkrParser.h"
#include "ldfReader/EbfException.h"
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

    TkrParser::TkrParser(EBFevent* event,
        TKRcontribution* contribution,
        ldfReader::TowerData *tData,
        const char* prefix) :
    TKRcontributionIterator(event, contribution),
        m_event(event),
        m_contribution(contribution),
        m_prefix(prefix),
        m_tData(tData)
    {
    }

    void TkrParser::parse()
    {
        // Purpose and Method:  Initiates the iteration over the TKR data - causing the callbacks
        // to the strip and ToT methods

        // Check if there is any TKR data in this event
        unsigned length  = m_contribution->numAccepts(m_event);
        if (!length)
        {
            if (EbfDebug::getDebug()) 
                printf("%sTKR: no data - empty contribution\n", m_prefix);
            return;
        }

        char pfx[80];
        sprintf(pfx, "%s    ", m_prefix);

        if (EbfDebug::getDebug()) printf("%sTKR:\n", m_prefix);

        unsigned       array[8];              // 8 longwords of space
        unsigned*      accepts = m_contribution->acceptsMask(m_event, array);

        printf("%s           GTCC   odd even  GTCC\n", m_prefix);
        unsigned i;

        if (EbfDebug::getDebug()) {
            for (i = 0; i < 8; i += 2)
            {
                printf("%s    %s %5s  %d  0x%03x 0x%03x  %d  %s %-5s\n",
                    m_prefix, xy[i],        label[i],  gtcc[i], accepts[i],
                    accepts[i+1], gtcc[i+1], xy[i+1], label[i+1]);
            }

            printf("%s  Number of Accepts     = %d\n", m_prefix, m_contribution->numAccepts(m_event));
            printf("%s  Data:\n", m_prefix);
        }

        if (EbfDebug::getDebug()) {
            printf("%s             Layer\n", pfx);
            printf("%sTower Plane   x/y l/h  GTFE Strip   raw  =  raw\n", pfx);
        }
        // Iterate over the strip data
        iterateStrips();

        // Iterate over TOT data
        if (EbfDebug::getDebug()) printf("\n%sTower layer_end  TOT = TOT\n", pfx);
        iterateTOTs();
    }



    // layerEnd uses least sig. bit to keep track of low or high end readout.
    // Remainder is (uni)layer number, shifted up one
    void TkrParser::strip(unsigned tower, unsigned layerEnd, unsigned hit) {
        // Purpose and Method:  The strip data callback which stores the strip data in our
        // local static object for general access to ldfReader clients.

        using namespace ldfReader;

        unsigned myPlane = layerEnd >> 1;
        unsigned  myLowHigh = layerEnd & 1;
        const char* myXy = xyByPlane[myPlane % 4];
        if (EbfDebug::getDebug()) {
            printf("%s  %d   %d   %s  %d   %02d    %02d  0x%03x = %4d\n",
                m_prefix, tower, myPlane, myXy, myLowHigh,
                TKRstrip::gtfe(hit), TKRstrip::strip(hit), hit, hit);
        }

        unsigned myLayer = myPlane/2;

        ldfReader::TkrLayer *layer = m_tData->getTkrLayer(myLayer);
        if (!layer) {
            layer = new ldfReader::TkrLayer(myLayer);
            m_tData->addTkrLayer(layer);
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
            fprintf(stderr, 
                "failed to find the TKR layer, view, controller combination\n");
        }
    }

    void TkrParser::TOT(unsigned tower, unsigned layerEnd, unsigned tot) {
        // Purpose and Method:  ToT callback that handles unpacking ToT data for storage in 
        // the ldfReader static object that provides data access for ldfReader clients

        using namespace ldfReader;

        unsigned myLowHigh = layerEnd & 1;
        unsigned myPlane = layerEnd >> 1;
        const char* myXy = xyByPlane[myPlane % 4];

        if (EbfDebug::getDebug()) {
            printf("%s %2d %2d 0x%02x = %3d\n", m_prefix, tower, layerEnd, tot, tot);
        }

        unsigned myLayer = myPlane/2;

        ldfReader::TkrLayer *layer = m_tData->getTkrLayer(myLayer);
        if (!layer) {
            layer = new ldfReader::TkrLayer(myLayer);
            m_tData->addTkrLayer(layer);
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
        }

    }
}
#endif

