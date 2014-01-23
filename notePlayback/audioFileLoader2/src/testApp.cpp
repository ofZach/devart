#include "testApp.h"
#include <sndfile.hh>


bool sortNotes(note * a, note *b){
    return a->mostCommonNote < b->mostCommonNote ? true : false;
}


//--------------------------------------------------------------
void testApp::setup(){

    counter.assign(150,0);
    
    ofDirectory dir;
    dir.listDir( "../../../../audio/output/" );
    
    vector < string > files;
    
    for (int i = 0; i < dir.size(); i++){
    
        ofDirectory dir2;
        dir2.allowExt("wav");
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
    
    myPiano.setup();
    
    soundStream.setup(this, 2, 0, 44100, 256, 4);

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
                //    output[j *nChannels + i] += sin(sinAngle) * 0.05;
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
void testApp::update(){

   
}

//--------------------------------------------------------------
void testApp::draw(){
    myPiano.draw();
}

float MIDI2freq(int MIDI_note) {
	return 440.0 * powf(2.0,(((float)MIDI_note-(float)69)/(float)12));
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    if (key < 97) return;
    
    if (key == 'z') {
        myPiano.octaveDown();
    }
    
    if (key == 'x') {
        myPiano.octaveUp();
    }

    if (myPiano.keyBindings[key] != 0 || key == 'a') {
        int note = myPiano.getNote(key);
        //cout << "trying to play note " << note << endl;
        
        vector < int > anyGoods;
        for (int i = 0; i < notes.size(); i++){
            int which = i;
            if (notes[which]->mostCommonNote == note){
                anyGoods.push_back(i);
                //notes[which]->play();
                //break;
            }
        }
        
        cout << anyGoods.size() << endl;
        //if (ofRandom(0,1) > 0.3){
        if (anyGoods.size() > 0){
            
            counter[note]++;
            
            
            int which = anyGoods[ counter[note] % anyGoods.size() ];
            notes[which]->play();
        }
        //} else {
            bPlayingTone = true;
            startTime = ofGetElapsedTimef();
            sinAngle = 0;
            float freq = MIDI2freq(note);
            sinAngleAdder = (freq * TWO_PI) / 44100.0;
        
        
            
       // }
    }
    
    myPiano.keyPressed(key);
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    myPiano.keyReleased(key);
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
