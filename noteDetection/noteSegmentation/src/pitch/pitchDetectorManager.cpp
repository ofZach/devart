//
//  pitchDetectorsManager.cpp
//  noteSegmentation
//
//  Created by zach on 1/17/14.
//
//



#include "pitchDetectorManager.h"
#include "utils.h"




void pitchDetectorManager::setup(int winSize, int hopSize){

    numAPDs = 2;
    
    for (int i = 0; i < numAPDs; i++) {
        pitchDetectors.push_back(new aubioPitchDetector);
    }
    
    methods.push_back("yin");
    methods.push_back("yinfft");
//    methods.push_back("specacf");
//    methods.push_back("schmitt");
//    methods.push_back("mcomb");
//    methods.push_back("fcomb");
    
    for (int i = 0; i < numAPDs; i++) {
        aubioPitchDetector * APD = static_cast<aubioPitchDetector*>(pitchDetectors[i]);
        APD->setup("midi", methods[i], winSize, hopSize);
    }
    

    
    melo = new filePitchDetector();
    melo->name = "melodia";
    ((filePitchDetector*)melo)->loadAssociatedFile(getAudioDirectory() + "rebelradio.vals.txt");
    pitchDetectors.push_back(melo);
    
    /*
    pyin = new filePitchDetector();
    pyin->name = "pYin";
    ((filePitchDetector*)pyin)->loadAssociatedFile(getAudioDirectory() + "rebelradio.pyin.vals.txt");
    pitchDetectors.push_back(pyin);
*/
    
    nPitchDetectors = pitchDetectors.size();
    
    PDMethod = 0;
}


void pitchDetectorManager::processPitchDetectors(float * buffer, int bufferSize, int sampleTime){
    
    for (int i = 0; i < pitchDetectors.size(); i++) {
        pitchDetectors[i]->calculatePitch(buffer, bufferSize, sampleTime);
    }
    
}
float pitchDetectorManager::getPitch(){
    return pitchDetectors[PDMethod]->getPitch();
}

float pitchDetectorManager::getPitch(int whichOne){
     return pitchDetectors[whichOne]->getPitch();
    
}
