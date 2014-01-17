#include "testApp.h"
#include "aubio.h"
#include "utils.h"



// create some vectors
//fvec_t * in       = new_fvec (hop_s); // input buffer




//--------------------------------------------------------------
void testApp::setup(){
    
    
    
    samplerate = 44100;
    windowSize = 2048;
    hopSize = 1024;
    
    PDM.setup(windowSize, hopSize);
    
    
    smoother tempSmoother;
    tempSmoother.setNumPValues(11);
    for (int i = 0; i < PDM.size(); i++) {
        smoothers.push_back(tempSmoother);
    }
    
    
    graphWidth = ofGetWidth()/2;
    graphHeight = ofGetHeight()/3;
    graphMax = 120;
    
    scrollingGraph tempGraph;
    tempGraph.setup(graphWidth, 0, 0, graphMax);
    for (int i = 0; i < PDM.size(); i++) {
        pitchGraphs.push_back(tempGraph);
        medianGraphs.push_back(tempGraph);
        velGraphs.push_back(tempGraph);
    }
    
    
    for (int i = 0; i < PDM.size(); i++) {
        ofColor tempColor;
        tempColor.setHsb(i*40, 255, 180, 200);
        graphColors.push_back(tempColor);
    }
    
    runs.setup(graphWidth, 0, 0, 1);
    
    coarseThreshold = fineThreshold = 1.0;
    minDuration = 25;
    maxDuration = 50;
    drawMarkers = true;
    
    bAmRecording = false;
    
    setupGUI();
    
    
    sinAngle = 0;

    
    minPitch = 20;
    
    au.setup("lankra.wav", hopSize);
    au.playFile();
    
    
    ss.setup(this, 1, 1, samplerate, hopSize, 4);
}



//--------------------------------------------------------------
void testApp::update(){
    
    
    
    for (int i = 0; i < PDM.size(); i++) {
        pitchGraphs[i].addValue(PDM.getPitch());
        smoothers[i].addValue(PDM.getPitch());
        medianGraphs[i].addValue(smoothers[i].getMedian());
        
        
        float lastVal = medianGraphs[i].valHistory[medianGraphs[i].valHistory.size() - 1];
        float secondLastVal = medianGraphs[i].valHistory[medianGraphs[i].valHistory.size() - 2];
        float vel = abs(lastVal - secondLastVal);
        velGraphs[i].addValue(vel);
    }
    
        

    
    // count how many frames in a row the vel is below the threshold
    if ( velGraphs[PDM.PDMethod].getLast() < (bVelFine ? fineThreshold : coarseThreshold) && !bBelowMinPitch ) {
        noteRun++;
        bAmRecording = true;
    }
    else  {
        // if the vel is above the thresh then check if the current run is longer than the min duration. If so save the note.  Regardless, set the run count to zero.
        
        
        bAmRecording = false;
        
        if ( noteRun > minDuration && !bBelowMinPitch) {

            marker segment;
            segment.start = graphWidth - 1 - noteRun;
            segment.end = graphWidth - 1;
            markers.push_back(segment);
            
            currentNote.playhead = 0;
            currentNote.bPlaying = true;
            currentNote.bWasPlaying = false;
            currentNote.mostCommonPitch = findMostCommonPitch(currentNote);
            notes.push_back(currentNote);
            
//            cout << "note recorded - min duration = " << minDuration << endl << endl;
            
        }
        
        noteRun = 0;
        bBelowMinPitch = false;
    }

//    cout << noteRun << " " << bAmRecording << " vel = " << velGraphs[PDMethod].getLast() << " thresh = " << threshold << endl;
    
    runs.addValue(bAmRecording);
    
//    scroll markers
    if (markers.size() > 0) {
        for (int i = 0; i < markers.size(); i++) {
            markers[i].start--;
            markers[i].end--;
        }
    }
    
}

//--------------------------------------------------------------
void testApp::draw(){

    // TODO: name
    //ofSetColor(255, 10, 10);
    //ofDrawBitmapString(PDM.pitchDetectors[PDMethod]->name, ofGetWidth() - 100, 50);

    ofSetColor(25);
    ofLine(ofGetWidth() - minDuration * 2, 90, ofGetWidth(), 90);
    ofLine(ofGetWidth() - minDuration * 2, 92, ofGetWidth() - minDuration * 2, 88);
    
    (noteRun < minDuration) ? ofSetColor(255,0,0) : ofSetColor(0,255,0);
    ofLine(ofGetWidth() - noteRun * 2, 95, ofGetWidth(), 95);

    ofPushMatrix();
    ofTranslate(0, 100);
    ofSetColor(255, 10, 10);
    medianGraphs[ PDM.getMethod() ].draw(graphHeight);
//        ofSetColor(25);
//        runs.draw(graphHeight);

    if (drawMarkers) {
        
        for (int i = 0; i < markers.size(); i++) {
            ofSetColor(255,255,255,127);
            ofRect(markers[i].start * 2, 0, markers[i].end * 2 - markers[i].start * 2, graphHeight);
        }
    }
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(0, graphHeight + 100);
    ofSetColor(255,0,0);
    
    if (bVelFine) {
        velGraphs[ PDM.getMethod() ].draw(graphHeight, 0.0, 2.0);
    }
    else {
        velGraphs[ PDM.getMethod() ].draw(graphHeight);
    }
    
    if (drawMarkers) {
        
        ofSetColor(0);
        float threshLineHeight = graphHeight - ((bVelFine ? fineThreshold/2*graphMax : coarseThreshold ) / graphMax * graphHeight);
        ofLine(0, threshLineHeight, ofGetWidth(), threshLineHeight);
        
        ofSetColor(0,0,0,127);
        for (int j = 0; j < velGraphs[ PDM.getMethod() ].valHistory.size(); j++) {
            if ( velGraphs[ PDM.getMethod() ].valHistory[j] > (bVelFine ? fineThreshold : coarseThreshold ) ) {
                ofLine(j * 2, -graphHeight, j * 2, graphHeight);
            }
        }
        
    }

    ofPopMatrix();


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
    au.getTapSamples(samples);

    // process:
    PDM.processPitchDetectors(samples, bufferSize, au.getSampleTime());
    
    //recording
    if (bAmRecording){
        for (int i = 0; i < bufferSize; i++){
            currentNote.samples.push_back(samples[i]);
        }
        
        float pitch = PDM.getPitch();
        if (pitch < minPitch) bBelowMinPitch = true;
        else currentNote.analysisFrames.push_back(pitch);
        
    } else  {
        currentNote.samples.clear();
        currentNote.analysisFrames.clear();
    }
    
}

void testApp::audioOut(float * output, int bufferSize, int nChannels){
    

    
    for (int i = 0; i < notes.size(); i++){
        
        //play sampler
        if ( !notes[i].bWasPlaying && notes[i].bPlaying ) {
            au.startNote(notes[i].mostCommonPitch + samplerOctavesUp * 12);
        }
        else if ( notes[i].bWasPlaying && !notes[i].bPlaying ) {
            au.stopNote(notes[i].mostCommonPitch + samplerOctavesUp * 12);
        }

        notes[i].bWasPlaying = notes[i].bPlaying;
        
        //clear buffer
        for (int j = 0; j < bufferSize; j++){
            output[j] = 0;
        }
        
        //while audio clips are not finshed playing
        if (notes[i].bPlaying == true && (notes[i].playhead + bufferSize) < notes[i].samples.size()){
            //play audio
            int playhead = notes[i].playhead;
            for (int j = 0; j < bufferSize; j++){
                output[j] += notes[i].samples[playhead + j] * 0.2 * audioVol;
            }
            notes[i].playhead += bufferSize ;
            
            //play sine wave
            int frame = playhead / bufferSize;
            int midiNote = notes[i].analysisFrames[frame];
            
            float freq = pow(2, float(midiNote-69)/12.0)*440;
            freq *= pow(2.0, sinOctavesUp);
//            cout << frame << " / " << notes[i].analysisFrames.size() << " midi " << midiNote << " freq " << freq << endl;
            //fm  =  2(m−69)/12(440 Hz)
            float sinAngleAdder = freq * TWO_PI / 44100.0;
            
            for (int j = 0; j < bufferSize; j++){
                
                output[j] += sin(sinAngle) * 0.2 * sinVol;
                
                sinAngle+= sinAngleAdder;
                
            }
            
            while (sinAngle > PI) sinAngle -= TWO_PI;
            
        } else {
            notes[i].bPlaying = false;
        }
        
    }
    
}

float testApp::findMostCommonPitch(audioNote note){
    
    vector < int > notes;
    for (int i = 0; i < note.analysisFrames.size(); i++){
        float freq = note.analysisFrames[i];
        
        if (freq > 0){
            int note = freq; //freq2midi(freq);
            if (note > 0 && note < 150) notes.push_back(note);
        }
    }
    
    // see utils.h
    
    return findMostCommon(notes);
    
}

void testApp::setupGUI(){
    //init params
    audioVol = 1.0;
    sinVol = 0.0;
    samplerOctavesUp = sinOctavesUp = 0;
    
    bVelFine = false;
    
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
    gui->addSlider("Coarse Threshold", 0.0, graphMax, &coarseThreshold, length-xInit, dim);
    gui->addSlider("Fine Threshold", 0.0, 2.0, &fineThreshold, length-xInit, dim);
    gui->addLabelToggle("Coarse/Fine", &bVelFine);
    gui->addSlider("Min duration", 1, 60, &minDuration, length-xInit, dim);
    gui->addIntSlider("Min pitch", 0, 30, &minPitch, length-xInit, dim);
    gui->addSpacer(length-xInit, 1);
    gui->addLabel("AUDIO OUTPUT");
    gui->addSlider("Audio Volume", 0.0, 1.0, &audioVol, length-xInit, dim);
    gui->addSlider("Sampler volume", 0.0, 1.0, 1.0, length-xInit, dim);
    gui->addSlider("Sine wave volume", 0.0, 1.0, &sinVol, length-xInit, dim);
    gui->addIntSlider("Sampler octvs up", 0, 4, &samplerOctavesUp, length-xInit, dim);
    gui->addIntSlider("Sine wave octvs up", 0, 4, &sinOctavesUp, length-xInit, dim);
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
}

void testApp::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
	int kind = e.widget->getKind();
    
    if(name == "LPF cutoff") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        au.lpf.setParameter(kLowPassParam_CutoffFrequency, kAudioUnitScope_Global, slider->getScaledValue());
    }
    else if (name == "LPF resonance") {
     // Global, dB, -20->40, 0
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        au.lpf.setParameter(kLowPassParam_Resonance, kAudioUnitScope_Global, slider->getScaledValue());
    }
    else if (name == "MF numPValues") {
        ofxUIIntSlider *slider = (ofxUIIntSlider *) e.widget;
        for (int i = 0; i < PDM.size(); i++) {
            smoothers[i].setNumPValues(slider->getValue());
        }
    }
    else if (name == "Sampler volume") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        au.mixer.setInputVolume(slider->getScaledValue() * 0.2, 1);
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
            drawMarkers = !drawMarkers;
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

}
