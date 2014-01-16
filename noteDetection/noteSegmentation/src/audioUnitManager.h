//
//  audioUnitManager.h
//  noteSegmentation
//
//  Created by Jason Levine on 2014-01-16.
//
//

#include "ofxAudioUnit.h"

class audioUnitManager {
public:
    void setup(string filename, int _bufferSize);
    void playFile();
    void getTapSamples(float * samples);
    void startNote(int midiNote);
    void stopNote(int midiNote);
    
    int bufferSize;
    
    ofxAudioUnitFilePlayer player;
    ofxAudioUnit lpf;
    ofxAudioUnitTap tap;
    ofxAudioUnitSampler sampler;
    ofxAudioUnitMixer mixer;
    ofxAudioUnitOutput output;
    
};
