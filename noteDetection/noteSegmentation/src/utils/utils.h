

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

struct note {
    
    int startTime;
    int endTime;
    
    int mostCommonPitch;
    
    bool bPlaying;
    int playbackTime;

};

struct metadata {
    float yinStdDev;
    float yinFFTStdDev;
    float meloStdDev;
    float yinAgree;
    float yinFFTAgree;
};
