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
#include "utils.h"

#pragma once


struct marker {
    float start;
    float end;
};

struct audioNote {
    
    bool bPlaying;
    bool bWasPlaying;
    
    vector < float > samples;
    vector < float > analysisFrames;
    int mostCommonPitch;
    int playhead;
};


class segmentationManager {
    
public: 
    
    void setup( int numPitchDetectors );
    void update(float * samples, int bufferSize);
    void draw();
    float findMostCommonPitch(audioNote note);
    
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
    
    //notes
    audioNote currentNote;
    vector < audioNote > notes;
    
    
    
};