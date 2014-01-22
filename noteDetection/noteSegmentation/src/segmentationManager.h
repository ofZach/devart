//
//  segmentationManager.h
//  noteSegmentation
//
//  Created by zach on 1/17/14.
//
//

#include "ofMain.h"
//#include "scrollingGraph.h"
//#include "smoother.h"
#include "pitchDetectorManager.h"
#include "audioUnitManager.h"
#include "utils.h"

#pragma once





class segmentationManager {
    
public: 
    
    void setup( int numPitchDetectors, int _bufferSize );
    void update(float * samples, int sampleTime);
    void draw();
    
    void scrollMarkers();
    
    void playSegments(vector<float> &output);
    float audioVol, sinVol;
    int samplerOctavesUp, sinOctavesUp;
    float sinAngle;
    
    float findMostCommonPitch(audioNote note);
    
    int bufferSize;
    
    pitchDetectorManager * PDM;
    
    audioUnitManager * AU;
    
    
    int minPitch;
    bool bBelowMinPitch;
    
    bool bVelFine;
    
    //markers
    vector<marker> markers;
    bool drawMarkers;

    float coarseThreshold, fineThreshold;
    float minDuration;
    float noteRun;
    
    bool bAmRecording;
    
    //notes
    audioNote currentNote;
    vector < audioNote > notes;
    
    
    

    
};