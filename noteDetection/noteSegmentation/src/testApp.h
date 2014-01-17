#pragma once

#include "ofMain.h"

//#include "ofxAudioUnit.h"
#include "ofxUI.h"
#include "ofxXmlSettings.h"
#include "pitchDetectorManager.h"
#include "audioUnitManager.h"
#include "segmentationManager.h"





class testApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void loadAudio( string fileName );
    
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

    
    //------soundStreamManager
    
    //soundStream
    int samplerate, windowSize, hopSize;
    ofSoundStream ss;
    void audioIn(float * input, int bufferSize, int nChannels);
    void audioOut(float * output, int bufferSize, int nChannels);
    

    
    float audioVol, sinVol;
    int samplerOctavesUp, sinOctavesUp;
    float sinAngle;

    pitchDetectorManager PDM;

    segmentationManager SM;
   
    audioUnitManager au;

    ofxUICanvas * gui;
    
};
