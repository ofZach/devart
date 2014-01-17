//
//  pitchDetectorsManager.h
//  noteSegmentation
//
//  Created by zach on 1/17/14.
//
//

#include "ofMain.h"

#pragma once

#include "pitchDetector.h"

// I hold the pitch detectors.  I process buffers and split out pitch

class pitchDetectorManager {

public:
    
    int numAPDs;
    int PDMethod;
    int nPitchDetectors;
    
    vector<char_t *> methods;
    vector<basePitchDetector*> pitchDetectors;

    void setup(int winSize, int hopSize);
    void processPitchDetectors(float * buffer, int bufferSize, int sampleTime);   //samples, bufferSize, au.getSampleTime()
    float getPitch();
    
    void setPitchMethod(int whichMethod){
        PDMethod = whichMethod;
    }
    
    int getMethod(){
        return PDMethod;
    }
    // maybe I'll rename this?
    int size(){
        return nPitchDetectors;
    }
    
};

