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
#include "audioUnitManager.h"
#include "utils.h"

#pragma once





class segmentationManager {
    
public: 
    
    void setup( int numPitchDetectors, int _bufferSize );
    void update(float * samples, int sampleTime);
    void draw();
    
    void updateGraphs();
    void scrollMarkers();
    
    void playSegments(vector<float> &output);
    float audioVol, sinVol;
    int samplerOctavesUp, sinOctavesUp;
    float sinAngle;
    
    float findMostCommonPitch(audioNote note);
    
    int bufferSize;
    
    pitchDetectorManager * PDM;
    int nPds;
    
    audioUnitManager * AU;
    
    
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