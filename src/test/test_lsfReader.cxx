#ifndef TEST_lsfReader_CXX
#define TEST_lsfReader_CXX 1


/** @file test_dfiReader.cxx
@brief Test routine for the new EBF reader

$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/test/test_lsfReader.cxx,v 1.4 2006/03/05 09:21:26 heather Exp $
*/

#include <iostream>

#include "ldfReader/DfiParser.h"
#include "facilities/Util.h"
#include "../../src/EbfDebug.h" 
#include "ldfReader/data/LatData.h"

#include <string>

int main(int argn, char** argc) {
  try {
    using namespace ldfReader;
    int debug = 1;
    int contextOnly = 0;
    std::string fileName("$(EVENTFILEROOT)/src/test/events.lpa");


    if (argn>1) debug = facilities::Util::atoi(argc[1]);
    if (argn>2) contextOnly = facilities::Util::atoi(argc[2]);
    if( argn>3 ) fileName = argc[3];

    if (facilities::Util::expandEnvVar(&fileName) < 0) {
        std::cout << "Failed to expand env variable in file name "
                  << fileName << std::endl;
        return -1;
    }

    EbfDebug::setDebug(((debug==1)?true:false));

    std::cout << "Reading file: " << fileName << std::endl;

    DfiParser *ebfP = new DfiParser(fileName);

    std::cout << "DfiParser is set up" << std::endl;
    ebfP->printHeader();

    int status = 0;
    do {
        status = ebfP->loadData();
        std::cout << "After loading" << std::endl;
        if (status < 0) {
            std::cout << "Failed to Load data - EOF" << std::endl;
            break;
        }
        // Retrieve the LAT data for this event and print out its contents
        LatData* myLatData = LatData::instance();


        if (contextOnly) {
            myLatData->print();
        } else {
        printf("\nSTART NEW EVENT\n");
        myLatData->print();
        printf("Run: %d, Event %d \n", myLatData->runId(), myLatData->summaryData().eventSequence());
        printf("Event Size in Bytes %u \n", myLatData->eventSizeInBytes());

        printf("EBF Format Identity 0x%08x\n", myLatData->getFormatIdentity());
        // Event Summary
        EventSummaryData summary = myLatData->summaryData();
        summary.print();
        printf("   Event GMT timestamp   = %ld.%09ld seconds after 1/1/1970\n",
          summary.timeSec(), summary.timeNanoSec());

        OswData osw = myLatData->getOsw();
        osw.print();

        // Check GEM data
        GemData gem = myLatData->getGem();
        gem.print();

        AemData aem = myLatData->getAem(); 
        aem.print();


        // Check the ACD data
        printf("ACD Data\n");
        const std::map<const char*, AcdDigi*> acdCol = myLatData->getAcdCol();
        std::map<const char*, AcdDigi*>::const_iterator acdIter;
        for (acdIter = acdCol.begin(); acdIter != acdCol.end(); acdIter++) {
            (acdIter->second)->print();
        }

        // Check the data in the Towers
        unsigned int iTower;
        for (iTower = 0; iTower < 16; iTower++) {
            TowerData *tem = myLatData->getTower(iTower);
            if (!tem) {
                //printf("Tem does not exist\n");
            } else {
                tem->print();
                printf("TEM:  %d\n", tem->getTowerId());
                unsigned int i;
                // Print CAL data
                //const std::map<CalDigi> calData = tem->getCalDigiCol();
                //std::vector<CalDigi>::const_iterator it;
                printf("CAL data\n");

                //      for (it = calData.begin(); it != calData.end(); it++) {
                int iLayer, iCol;
                const int maxLayerNum = 20;
                const int maxColNum = 15;
                for (iLayer = 0; iLayer < maxLayerNum; iLayer++) {
                    for (iCol = 0; iCol < maxColNum; iCol++) {
                        CalDigi* it = tem->getCalDigi(iLayer, iCol);
                        if (!it) continue;
                        printf("Layer = %d, Col = %d, Mode = %d\n", it->getLayerNum(), 
                            it->getColumnNum(), it->getMode());
                        CalDigi::CalTrigMode mode = it->getMode();
                        if (mode == CalDigi::BESTRANGE) {
                            const CalDigi::CalReadout *readout = it->getXtalReadout(0);
                            printf("Pos Range %d, Pos ADC %d\n", readout->getRange(CalDigi::POS), readout->getAdc(CalDigi::POS));
                            printf("Neg Range %d Neg ADC %d\n", readout->getRange(CalDigi::NEG), readout->getAdc(CalDigi::NEG));
                        } else { // ALLRANGE
                            unsigned int j;
                            for (j=0; j<4; j++) {
                                const CalDigi::CalReadout *readout = it->getXtalReadout(j);
                                printf("Pos Range %d, Pos ADC %d\n", readout->getRange(CalDigi::POS), readout->getAdc(CalDigi::POS));
                                printf("Neg Range %d Neg ADC %d\n", readout->getRange(CalDigi::NEG), readout->getAdc(CalDigi::NEG));

                            }
                        }
                    }
                }

                // Print TKR data
                printf("TKR Data\n");
                for (i = 0; i < 18; i++) {
                    TkrLayer *layer = tem->getTkrLayer(i);
                    if (layer) {
                        printf("    Layer:  %d\n", layer->getLayerNum());
                        const std::vector<unsigned short> xStripColC0 = layer->getXStripC0Col();
                        std::vector<unsigned short>::const_iterator xStripC0;
                        printf("     X Strips C0: ");
                        for (xStripC0 = xStripColC0.begin(); xStripC0 != xStripColC0.end(); xStripC0++) {
                            printf(" %d ", *xStripC0);
                        }
                        printf("\n");
                        const std::vector<unsigned short> xStripColC1 = layer->getXStripC1Col();
                        std::vector<unsigned short>::const_iterator xStripC1;
                        printf("     X Strips C1: ");
                        for (xStripC1 = xStripColC1.begin(); xStripC1 != xStripColC1.end(); xStripC1++) {
                            printf(" %d ", *xStripC1);
                        }
                        printf("\n");
                        const std::vector<unsigned short> yStripColC0 = layer->getYStripC0Col();
                        std::vector<unsigned short>::const_iterator yStripC0;
                        printf("     Y Strips C0: ");
                        for (yStripC0 = yStripColC0.begin(); yStripC0 != yStripColC0.end(); yStripC0++) {
                            printf(" %d ", *yStripC0);
                        }
                        printf("\n");
                        const std::vector<unsigned short> yStripColC1 = layer->getYStripC1Col();
                        std::vector<unsigned short>::const_iterator yStripC1;
                        printf("     Y Strips C1: ");
                        for (yStripC1 = yStripColC1.begin(); yStripC1 != yStripColC1.end(); yStripC1++) {
                            printf(" %d ", *yStripC1);
                        }

                        printf("\n");

                    }
                }
                printf("\n\n");

            }
        } 
        }

        // Move the event pointer to the next event in the EBF file
        status = ebfP->nextEvent();

    } while (status >= 0);


    delete ebfP;
    ebfP = 0;

    std::cout << "Test completed successfully" << std::endl;
    return 0;
  } catch(...) {
    printf("Caught exception - exiting\n"); 
    return(-1);
  }
}

#endif
