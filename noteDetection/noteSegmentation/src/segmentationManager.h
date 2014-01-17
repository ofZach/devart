//
//  segmentationManager.h
//  noteSegmentation
//
//  Created by zach on 1/17/14.
//
//

#include "ofMain.h"
#include "scrollingGraph.h"
#include "smoother.h"
#include "pitchDetectorManager.h"

#pragma once


struct marker {
    float start;
    float end;
};


class segmentationManager {
    
public: 
    
    void setup( int numPitchDetectors );
    void update();
    void draw();
    
    pitchDetectorManager * PDM;
    int nPds;
    
    
    int minPitch;
    bool bBelowMinPitch;
    
    //graphs
    vector<smoother> smoothers;
    vector<scrollingGraph> pitchGraphs, medianGraphs, velGraphs;
    //vector<bool> drawPitch, drawMedian;
    vector<ofColor> graphColors;
    float graphWidth;
    float graphMax;
    float graphHeight;
    bool bVelFine;
    
    //markers
    vector<marker> markers;
    bool drawMarkers;
    scrollingGraph runs;
    float coarseThreshold, fineThreshold;
    float minDuration, maxDuration;
    float noteRun;
    
    bool bAmRecording;
    vector < float > pitchesForRecording;
    
    
    
};