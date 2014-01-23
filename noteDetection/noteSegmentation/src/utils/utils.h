

#include "ofMain.h"

#pragma once

float freq2midi(float freq);
int findMostCommon (vector < int > & vals);
string getAudioDirectory();

void loadAudioToData(string fileName, vector < float > & audioSamples);
void saveDataToAudio(string fileName, vector < float > & audioSamples);

string zeroPadNumber(int num, int sizeToBePaddedTo);

//gonna move these somewhere else in the future...

struct marker {
    float start;
    float end;
};

struct audioNote {
    
    bool bPlaying;
    bool bWasPlaying;
    
//    vector < float > samples;
    vector < float > analysisFrames;

    int playhead;
    
    int nFramesRecording;
    int startTime;
    int endTime;
    
    //META DATA
    int mostCommonPitch;
    float yinStdDev;
    float yinFFTStdDev;
    float meloStdDev;
    float yinAgree;
    float yinFFTAgree;
    
};
