#include "testApp.h"
#include <sndfile.hh>


bool sortNotes(note * a, note *b){
    return a->mostCommonNote < b->mostCommonNote ? true : false;
}


//--------------------------------------------------------------
void testApp::setup(){

    
    ofDirectory dir;
    dir.listDir( "../../../../audio/output/" );
    
    vector < string > files;
    
    for (int i = 0; i < dir.size(); i++){
    
        ofDirectory dir2;
        dir2.listDir(dir.getPath(i));
        
        for (int j = 0; j < dir2.size(); j++){
            files.push_back(dir2.getPath(j));
        }
        
    }
    
    cout << files.size() << endl;
    
    
    for (int i = 0; i < files.size(); i++){
        note * tempNote = new note();
        //tempNote->AA = &AA;
        tempNote->loadFile(files[i]);
        notes.push_back(tempNote);
        
    }
    
   ofSort(notes, sortNotes);
    
    for (int i = 0; i < notes.size(); i++){
        cout << notes[i]->mostCommonNote << endl;
    }
    
    soundStream.setup(this, 2, 0, 44100, 256, 4);
    
    setupKeyboard();
    setupKeyBindings();

}


//--------------------------------------------------------------
void testApp::audioOut(float * output, int bufferSize, int nChannels){
    
    
    for (int i = 0; i < notes.size(); i++){
        notes[i]->addToSoundBuffer(output, bufferSize, nChannels);
    }
    
    if (bPlayingTone == true){
        if (ofGetElapsedTimef()-startTime > 1.0) bPlayingTone = false;
        
            float pct = (ofGetElapsedTimef()-startTime)  / 1.0;
        if (pct > 1) pct = 1;
        float scalar = sin(pct * PI);
        
            for (int j = 0; j < bufferSize; j++){
                for (int i = 0; i < nChannels; i++){
                    //output[j *nChannels + i] += sin(sinAngle) * 0.3;
                }
                sinAngle+=sinAngleAdder;
            }
//    bPlayingTone = true;
//    startTime = 0;
//    float freq = MIDI2freq(note);
//    sinAngleAdder = freq / 44100;
    }
    
    

}

//--------------------------------------------------------------
void testApp::setupKeyBindings() {
//    keyboard['a'] = 0;
//    keyboard['w'] = 1;
//    keyboard['s'] = 2;
//    keyboard['e'] = 3;
//    keyboard['d'] = 4;
//    keyboard['f'] = 5;
//    keyboard['t'] = 6;
//    keyboard['g'] = 7;
//    keyboard['y'] = 8;
//    keyboard['h'] = 9;
//    keyboard['u'] = 10;
//    keyboard['j'] = 11;
//    keyboard['k'] = 12;
    
    
    for (int i = 0; i < pianoKeys.size(); i++) {
        keyboard[pianoKeys[i].keyBinding] = i;
    }
    
    octave = 4;
}

//--------------------------------------------------------------
void testApp::setupKeyboard() {
    int whiteKeyHeight = 300;
    int whiteKeyWidth = 100;
    int blackKeyHeight = 200;
    int blackKeyWidth = 75;
    
    pianoKey c;
    c.blackKey = false;
    c.pos = 0.0;
    c.keyBinding = 'a';
    c.keyPressed = false;
    pianoKeys.push_back(c);
    
    pianoKey csharp;
    csharp.pos = 0.5;
    csharp.keyBinding = 'w';
    csharp.blackKey = true;
    csharp.keyPressed = false;
    pianoKeys.push_back(csharp);
    
    pianoKey d;
    d.pos = 1.0;
    d.keyBinding = 's';
    d.blackKey = false;
    d.keyPressed = false;
    pianoKeys.push_back(d);
    
    pianoKey dsharp;
    dsharp.pos = 1.5;
    dsharp.keyBinding = 'e';
    dsharp.blackKey = true;
    dsharp.keyPressed = false;
    pianoKeys.push_back(dsharp);
    
    pianoKey e;
    e.pos = 2.0;
    e.keyBinding = 'd';
    e.blackKey = false;
    e.keyPressed = false;
    pianoKeys.push_back(e);
    
    pianoKey f;
    f.pos = 3.0;
    f.keyBinding = 'f';
    f.blackKey = false;
    f.keyPressed = false;
    pianoKeys.push_back(f);
    
    pianoKey fsharp;
    fsharp.pos = 3.5;
    fsharp.keyBinding = 't';
    fsharp.blackKey = true;
    fsharp.keyPressed = false;
    pianoKeys.push_back(fsharp);
    
    pianoKey g;
    g.pos = 4.0;
    g.keyBinding = 'g';
    g.blackKey = false;
    g.keyPressed = false;
    pianoKeys.push_back(g);
    
    pianoKey gsharp;
    gsharp.pos = 4.5;
    gsharp.keyBinding = 'y';
    gsharp.blackKey = true;
    gsharp.keyPressed = false;
    pianoKeys.push_back(gsharp);
    
    pianoKey a;
    a.pos = 5.0;
    a.keyBinding = 'h';
    a.blackKey = false;
    a.keyPressed = false;
    pianoKeys.push_back(a);
    
    pianoKey asharp;
    asharp.pos = 5.5;
    asharp.keyBinding = 'u';
    asharp.blackKey = true;
    asharp.keyPressed = false;
    pianoKeys.push_back(asharp);
    
    pianoKey b;
    b.pos = 6.0;
    b.keyBinding = 'j';
    b.blackKey = false;
    b.keyPressed = false;
    pianoKeys.push_back(b);
    
    pianoKey cOctvUp;
    cOctvUp.pos = 7.0;
    cOctvUp.keyBinding = 'k';
    cOctvUp.blackKey = false;
    cOctvUp.keyPressed = false;
    pianoKeys.push_back(cOctvUp);
    
}

//--------------------------------------------------------------
void testApp::update(){

   
}

//--------------------------------------------------------------
void testApp::draw(){
    
    //white keys

    
    float whiteKeyHeight = 300;
    float whiteKeyWidth = 100;
    float blackKeyHeight = 200;
    float blackKeyWidth = 75;
    
    for (int i = 0; i < pianoKeys.size(); i++) {
        if (!pianoKeys[i].blackKey) {
            ofSetColor((pianoKeys[i].keyPressed ? 0 : 255));
            ofFill();
            ofRect(pianoKeys[i].pos * whiteKeyWidth, 0, whiteKeyWidth, whiteKeyHeight);
            
            ofSetColor((pianoKeys[i].keyPressed ? 255 : 0));
            ofNoFill();
            ofRect(pianoKeys[i].pos * whiteKeyWidth, 0, whiteKeyWidth, whiteKeyHeight);
            
            float kbX = pianoKeys[i].pos * whiteKeyWidth + whiteKeyWidth / 2;
            float kbY = whiteKeyHeight - whiteKeyHeight / 6;
            ofDrawBitmapString(ofToString(pianoKeys[i].keyBinding), kbX, kbY);
        }
    }
        
    for (int i = 0; i < pianoKeys.size(); i++) {
        if (pianoKeys[i].blackKey) {
            ofSetColor((pianoKeys[i].keyPressed ? 0 : 255));
            ofFill();
            ofRect(pianoKeys[i].pos * whiteKeyWidth + blackKeyWidth/6, 0, blackKeyWidth, blackKeyHeight);
            
            ofSetColor((pianoKeys[i].keyPressed ? 255 : 0));            ofNoFill();
            ofRect(pianoKeys[i].pos * whiteKeyWidth + blackKeyWidth/6, 0, blackKeyWidth, blackKeyHeight);
            
            float kbX = pianoKeys[i].pos * whiteKeyWidth + blackKeyWidth / 2;
            float kbY = blackKeyHeight - blackKeyHeight / 6;
            ofDrawBitmapString(ofToString(pianoKeys[i].keyBinding), kbX, kbY);
        }
    }
    
    
    /*
    //white keys
    int whiteKeyHeight = 300;
    int whiteKeyWidth = 100;
    
    for (int i = 0; i < 7; i++) {
        ofSetColor(255);
        ofFill();
        ofRect(i * whiteKeyWidth, 0, whiteKeyWidth, whiteKeyHeight);
        
        ofSetColor(0);
        ofNoFill();
        ofRect(i * whiteKeyWidth, 0, whiteKeyWidth, whiteKeyHeight);
    }
    
    
    //black keys
    int blackKeyHeight = 200;
    int blackKeyWidth = 75;
    
    vector<float> blackKeyPos;
    blackKeyPos.push_back(0.5);
    blackKeyPos.push_back(1.5);
    blackKeyPos.push_back(3.5);
    blackKeyPos.push_back(4.5);
    blackKeyPos.push_back(5.5);
    
    
    for (int i = 0; i < blackKeyPos.size(); i++) {
        ofSetColor(255);
        ofFill();
        ofRect(blackKeyPos[i] * whiteKeyWidth + 12.5, 0, blackKeyWidth, blackKeyHeight);
        
        ofSetColor(0);
        ofNoFill();
        ofRect(blackKeyPos[i] * whiteKeyWidth + 12.5, 0, blackKeyWidth, blackKeyHeight);
    }
*/
    
}

float MIDI2freq(int MIDI_note) {
	return 440.0 * powf(2.0,(((float)MIDI_note-(float)69)/(float)12));
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    if (key < 97) return;
    
    if (key == 'z') {
        octave--;
        if (octave < 0) octave = 0;
    }
    
    if (key == 'x') {
        octave++;
        if (octave > 6) octave = 6;
    }
    

    if (keyboard[key] != 0 || key == 'a') {
        int note = keyboard[key] + (octave+1) * 12;
        cout << "trying to play note " << note << endl;
        
        vector < int > anyGoods;
        for (int i = 0; i < notes.size(); i++){
            int which = i;
            if (notes[which]->mostCommonNote == note && notes[which]->bPlaying == false){
                anyGoods.push_back(i);
                //notes[which]->play();
                //break;
            }
        }
        
        //if (ofRandom(0,1) > 0.3){
        if (anyGoods.size() > 0){
            int which = anyGoods[ ofRandom(0,anyGoods.size()) ];
            notes[which]->play();
        }
        //} else {
            bPlayingTone = true;
            startTime = ofGetElapsedTimef();
            sinAngle = 0;
            float freq = MIDI2freq(note);
            sinAngleAdder = (freq * TWO_PI) / 44100.0;
        
        pianoKeys[keyboard[key]].keyPressed = true;
            
       // }
    }
    
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    pianoKeys[keyboard[key]].keyPressed = false;
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
