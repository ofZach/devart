//
//  PDCompare.h
//  noteSegmentation
//
//  Created by Jason Levine on 2014-01-21.
//
//


#include "scrollingGraph.h"
#include "smoother.h"
#include "pitchDetectorManager.h"

class PDCompare {
public:
    void setup(pitchDetectorManager * _PDM);
    void update();
    void draw();
    
    int nFrames;
    
    pitchDetectorManager * PDM;
    //graphs
    vector<smoother> smoothers;
    vector<scrollingGraph> medianGraphs;
    vector<float> means;
    vector<float> stdDevs;
    float graphWidth, graphHeight, graphMax;
};
