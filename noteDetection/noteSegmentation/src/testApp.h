#pragma once

#include "ofMain.h"

#include "ofxAudioUnit.h"
#include "ofxUI.h"
#include "ofxXmlSettings.h"

#include "scrollingGraph.h"
#include "smoother.h"

#include "pitchDetector.h"

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


class testApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void exit();
    
    float findMostCommonPitch(audioNote note);
    
    void setupGUI();
    void guiEvent(ofxUIEventArgs &e);

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    
    //soundStream
    ofSoundStream ss;
    void audioIn(float * input, int bufferSize, int nChannels);
    void audioOut(float * output, int bufferSize, int nChannels);
    
    // zach notes:
    audioNote currentNote;
    vector < audioNote > notes;
    
    // recording
    vector<float> samples;
    bool bAmRecording;
    float audioVol, sinVol;
    float sinAngle;
    
    
    //pitch detector
    int numPDs;
    int PDMethod;
    vector<char_t *> methods;
    vector<aubioPitchDetector> pitchDetectors;
    
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
    

    //AUs
    ofxAudioUnitFilePlayer player;
    ofxAudioUnit lpf;
    ofxAudioUnitTap tap;
    ofxAudioUnitSampler sampler;
    ofxAudioUnitMixer mixer;
    ofxAudioUnitOutput output;

    
    //UI
    ofxUICanvas * gui;
    
};
