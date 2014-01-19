

#include "ofMain.h"

#pragma once

float freq2midi(float freq);
int findMostCommon (vector < int > & vals);
string getAudioDirectory();

void loadAudioToData(string fileName, vector < float > & audioSamples);
void saveDataToAudio(string fileName, vector < float > & audioSamples);
