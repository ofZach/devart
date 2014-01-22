//
//  PDCompare.h
//  noteSegmentation
//
//  Created by Jason Levine on 2014-01-21.
//
//

#pragma once

#include "scrollingGraph.h"
#include "smoother.h"
#include "pitchDetectorManager.h"
#include "utils.h"


class PDCompare {
public:
    void setup(pitchDetectorManager * _PDM, int _bufferSize);
    void update(float * samples, int sampleTime);
    void draw();
    
    float findMostCommonPitch(audioNote note);
    
    int nFrames;
    int bufferSize;
    
    pitchDetectorManager * PDM;
    //graphs
    vector<smoother> smoothers;
    vector<scrollingGraph> medianGraphs;
    vector<scrollingGraph> noteFound;
    vector<float> agreedNotes;
    vector<float> means;
    vector<float> stdDevs;
    float graphWidth, graphHeight, graphMax;
    
    float stdDevThresh;
    
    bool sum;
    
    //from segMan
    float minDuration;
    float noteRun;
    
    bool bAmRecording;
    vector < float > pitchesForRecording;
    
    //notes
    audioNote currentNote;
    vector < audioNote > notes;
    
    int minPitch;
    vector<marker> markers;
};
