#pragma once

#include "ofMain.h"
#include "note.h"

struct pianoKey {
    float width;
    float height;
    bool blackKey;
    float pos;
    string keyBinding;
};

class testApp : public ofBaseApp {

public:
    void setup();
    void update();
    void draw();

    void setupKeyBindings();
    void setupKeyboard();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);

    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    

    ofSoundStream soundStream;
    void audioOut(float * output, int bufferSize, int nChannels);

    vector < note * > notes;
    map<int, int> keyboard;
    vector<pianoKey> pianoKeys;
    
    int octave;

    bool bPlayingTone;
    float sinAngle;
    float startTime;
    float sinAngleAdder;
    
    
};
