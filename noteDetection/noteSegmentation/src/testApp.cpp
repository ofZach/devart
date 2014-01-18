#include "testApp.h"
#include "aubio.h"
#include "utils.h"


void testApp::loadAudio( string fileName ){
    
    // this assumes either blah.mp3 or blah.wav
    // if mp3, convert to wav
    
    string extension = fileName.substr(fileName.find_last_of(".") + 1);
    string preExtension = fileName.substr(0, fileName.find_last_of("."));
    
    // if MP3, make wave with 1 channel and 44100:
    // TODO: better compression options for afconvert?
    
    if (extension == "mp3"){

        string wavFile = preExtension + ".wav";
        
        // check if the wav file exists
        ofFile file(wavFile);
        
        // if not, make it!
        if (!file.exists()){
            string command = "afconvert -f \'WAVE\' -c 1 -d I16@44100 -o ";
            command += "\'" + wavFile + "\'" ;
            command += " ";
            command += "\'" + fileName + "\'" ;
            system(command.c_str());
        }
        // now we process the wavefile...
        fileName = wavFile;
    }
    
    // get ready to do analysis
    string analysisFile = preExtension + ".vals.txt";
    string dataPathToVamp = ofToDataPath("") + "../../../../utils/vampCommandLine/";
    string command = dataPathToVamp + "vampTestDebug -s mtg-melodia:melodia:melody " + fileName + " -o " + analysisFile;
    string soundFileGood = "\'" + fileName + "\'";
    string analysisFileGood = "\'" + analysisFile + "\'";
    string commandStr = "python ../../../data/vampRunner.py " + soundFileGood + " " + analysisFileGood;
    
    // if analysis doesn't exist, do it:
    ofFile file(analysisFile);
    if (!file.exists()){
        system(commandStr.c_str());
    }
    
    
    
    AU.player.setFile(fileName);
    PDM.fpd->loadAssociatedFile(analysisFile);
    AU.player.play();
    
    //setFile(ofToDataPath(filename)); //Marc Terenzi - Love To Be Loved By You [692].mp3
    
    
}


//--------------------------------------------------------------
void testApp::setup(){
    
    
    
    samplerate = 44100;
    windowSize = 2048;
    hopSize = 1024;
    
    PDM.setup(windowSize, hopSize);

    

    AU.setup(getAudioDirectory() + "lankra.wav", hopSize);
    AU.playFile();
    
    SM.setup( PDM.size(), hopSize );
    SM.PDM = &PDM;
    SM.AU = &AU;
    
    setupGUI();

    ss.setup(this, 1, 1, samplerate, hopSize, 4);
    
    ofSetVerticalSync(false);
}



//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

    PDM.draw();
    SM.draw();
  

}

void testApp::exit(){
    
    // stop the audio thread first, this was causing the crashes:
    ss.stop();

//    for (int i = 0; i < numAPDs; i++) {
//        aubioPitchDetector * APD = static_cast<aubioPitchDetector*>(pitchDetectors[i]);
//        APD->cleanup();
//    }
    
    aubio_cleanup();

}



//--------------------------------------------------------------
void testApp::audioIn(float * input, int bufferSize, int nChannels){
    //get samples
    float samples[bufferSize];
    AU.getTapSamples(samples);

    int sampleTime = AU.getSampleTime();
    
    PDM.processPitchDetectors(samples, bufferSize, sampleTime);
    SM.update(samples, sampleTime);
    
}

void testApp::audioOut(float * output, int bufferSize, int nChannels){
//    cout << "BEFORE" << endl;
//    for (int i = 0; i < bufferSize; i++) {
//        cout << i << " " << output[i] << endl;
//    }
    vector<float> outputSamples;
    outputSamples.assign(bufferSize, 0.0);
    SM.playSegments(outputSamples);
//    cout << "-------------------------" << endl;
    for (int i = 0; i < bufferSize; i++) {
        output[i] = outputSamples[i];
//        cout << i << "  " << output[i] << endl;
    }
    
    
    
    
//    cout << "AFTER" << endl;
//    for (int i = 0; i < bufferSize; i++) {
//        cout << i << " " << output[i] << endl;
//    }
//    
    
}


void testApp::setupGUI(){
    

    
    SM.bVelFine = false;

    //init gui dims
    float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 255-xInit;
    
    //gui!
    gui = new ofxUICanvas(0, 0, length+xInit, ofGetHeight());
    
    gui->addFPSSlider("FPS SLIDER", length-xInit, dim*.25, 60);
    
    gui->addSpacer(length-xInit, 1);
    gui->addLabel("FILTERING");
    gui->addSlider("LPF cutoff", 1000, 20000, 20000, length-xInit, dim);
    gui->addSlider("LPF resonance", -20.0, 40.0, 0.0, length-xInit, dim);
    gui->addIntSlider("MF numPValues", 3, 33, 11, length-xInit, dim);
    gui->addSpacer(length-xInit, 1);
    gui->addLabel("SEGMENTATION");
    gui->addSlider("Coarse Threshold", 0.0, SM.graphMax, &SM.coarseThreshold, length-xInit, dim);
    gui->addSlider("Fine Threshold", 0.0, 2.0, &SM.fineThreshold, length-xInit, dim);
    gui->addLabelToggle("Coarse/Fine", &SM.bVelFine);
    gui->addSlider("Min duration", 1, 60, &SM.minDuration, length-xInit, dim);
    gui->addIntSlider("Min pitch", 0, 30, &SM.minPitch, length-xInit, dim);
    gui->addSpacer(length-xInit, 1);
    gui->addLabel("AUDIO OUTPUT");
    gui->addSlider("Audio Volume", 0.0, 1.0, &SM.audioVol, length-xInit, dim);
    gui->addSlider("Sampler volume", 0.0, 1.0, 1.0, length-xInit, dim);
    gui->addSlider("Sine wave volume", 0.0, 1.0, &SM.sinVol, length-xInit, dim);
    gui->addIntSlider("Sampler octvs up", 0, 4, &SM.samplerOctavesUp, length-xInit, dim);
    gui->addIntSlider("Sine wave octvs up", 0, 4, &SM.sinOctavesUp, length-xInit, dim);
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
}

void testApp::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
	int kind = e.widget->getKind();
    
    if(name == "LPF cutoff") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        AU.lpf.setParameter(kLowPassParam_CutoffFrequency, kAudioUnitScope_Global, slider->getScaledValue());
    }
    else if (name == "LPF resonance") {
     // Global, dB, -20->40, 0
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        AU.lpf.setParameter(kLowPassParam_Resonance, kAudioUnitScope_Global, slider->getScaledValue());
    }
    else if (name == "MF numPValues") {
        ofxUIIntSlider *slider = (ofxUIIntSlider *) e.widget;
        for (int i = 0; i < PDM.size(); i++) {
            SM.smoothers[i].setNumPValues(slider->getValue());
        }
    }
    else if (name == "Sampler volume") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        AU.mixer.setInputVolume(slider->getScaledValue() * 0.2, 1);
    }

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    switch (key) {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':    
        {
            PDM.setPitchMethod( key - 49 );
            break;
        }
            
        case 'm':
            SM.drawMarkers = !SM.drawMarkers;
            break;
    }

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

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

    loadAudio(dragInfo.files[0]);
    
}
