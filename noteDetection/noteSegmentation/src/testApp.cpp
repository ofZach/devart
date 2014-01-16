#include "testApp.h"
#include "aubio.h"


uint_t samplerate = 44100;
uint_t win_s = 2048; // window size
uint_t hop_s = 1024;  // hop size

// create some vectors
fvec_t * in       = new_fvec (hop_s); // input buffer




//--------------------------------------------------------------
void testApp::setup(){

    
    numPDs = 6;
    
    aubioPitchDetector tempPD;
    for (int i = 0; i < numPDs; i++) {
        pitchDetectors.push_back(tempPD);
    }
    
    methods.push_back("yin");
    methods.push_back("yinfft");
    methods.push_back("specacf");
    methods.push_back("schmitt");
    methods.push_back("mcomb");
    methods.push_back("fcomb");
    
    for (int i = 0; i < numPDs; i++) {
        pitchDetectors[i].setup("midi", methods[i]);
    }
    
    smoother tempSmoother;
    tempSmoother.setNumPValues(11);
    for (int i = 0; i < numPDs; i++) {
        smoothers.push_back(tempSmoother);
    }
    
    
    graphWidth = ofGetWidth()/2;
    graphHeight = ofGetHeight()/3;
    graphMax = 120;
    
    scrollingGraph tempGraph;
    tempGraph.setup(graphWidth, 0, 0, graphMax);
    for (int i = 0; i < numPDs; i++) {
        pitchGraphs.push_back(tempGraph);
        medianGraphs.push_back(tempGraph);
        velGraphs.push_back(tempGraph);
    }
    
    
    for (int i = 0; i < numPDs; i++) {
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
    //AU
    
    lpf = ofxAudioUnit(kAudioUnitType_Effect, kAudioUnitSubType_LowPassFilter);
    lpf.setParameter(kLowPassParam_CutoffFrequency, kAudioUnitScope_Global, 20000);

    player.setFile(ofToDataPath("Marc Terenzi - Love To Be Loved By You [692].mp3")); //Marc Terenzi - Love To Be Loved By You [692].mp3
    
    tap.setBufferLength(1024);
    
    sampler = ofxAudioUnitSampler('aumu', 'dls ', 'appl');
    sampler.setBank(0, 0);
    sampler.setProgram(0);
    
    mixer.setInputBusCount(2);
    mixer.setInputVolume(0.0, 0);
    mixer.setInputVolume(0.2, 1);
    
    player.connectTo(lpf).connectTo(tap).connectTo(mixer, 0);
    sampler.connectTo(mixer, 1);
    mixer.connectTo(output);
    
    output.start();
    player.loop();
    
    samples.assign(hop_s, 0.0);
    ss.setup(this, 1, 1, samplerate, hop_s, 4);
}



//--------------------------------------------------------------
void testApp::update(){
    
    
    
    for (int i = 0; i < numPDs; i++) {
        pitchGraphs[i].addValue(pitchDetectors[i].getPitch());
        smoothers[i].addValue(pitchDetectors[i].getPitch());
        medianGraphs[i].addValue(smoothers[i].getMedian());
        
        
        float lastVal = medianGraphs[i].valHistory[medianGraphs[i].valHistory.size() - 1];
        float secondLastVal = medianGraphs[i].valHistory[medianGraphs[i].valHistory.size() - 2];
        float vel = abs(lastVal - secondLastVal);
        velGraphs[i].addValue(vel);
    }
    
        

    
    // count how many frames in a row the vel is below the threshold
    if ( velGraphs[PDMethod].getLast() < (bVelFine ? fineThreshold : coarseThreshold) ) {
        noteRun++;
        bAmRecording = true;
//        currentNote.analysisFrames.push_back(medianGraphs[PDMethod].getLast());
    }
    else  {
        // if the vel is above the thresh then check if the current run is longer than the min duration. If so save the note.  Regardless, set the run count to zero.
        
        
        bAmRecording = false;
        
        if ( noteRun > minDuration ) {

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


    ofSetColor(255, 10, 10);
    ofDrawBitmapString(methods[PDMethod], ofGetWidth() - 100, 50);

    ofSetColor(25);
    ofLine(ofGetWidth() - minDuration * 2, 90, ofGetWidth(), 90);
    ofLine(ofGetWidth() - minDuration * 2, 92, ofGetWidth() - minDuration * 2, 88);
    
    (noteRun < minDuration) ? ofSetColor(255,0,0) : ofSetColor(0,255,0);
    ofLine(ofGetWidth() - noteRun * 2, 95, ofGetWidth(), 95);

    ofPushMatrix();
    ofTranslate(0, 100);
    ofSetColor(255, 10, 10);
    medianGraphs[PDMethod].draw(graphHeight);
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
        velGraphs[PDMethod].draw(graphHeight, 0.0, 2.0);
    }
    else {
        velGraphs[PDMethod].draw(graphHeight);
    }
    
    if (drawMarkers) {
        
        ofSetColor(0);
        float threshLineHeight = graphHeight - ((bVelFine ? fineThreshold/2*graphMax : coarseThreshold ) / graphMax * graphHeight);
        ofLine(0, threshLineHeight, ofGetWidth(), threshLineHeight);
        
        ofSetColor(0,0,0,127);
        for (int j = 0; j < velGraphs[PDMethod].valHistory.size(); j++) {
            if ( velGraphs[PDMethod].valHistory[j] > (bVelFine ? fineThreshold : coarseThreshold ) ) {
                ofLine(j * 2, -graphHeight, j * 2, graphHeight);
            }
        }
        
    }

    ofPopMatrix();


}

void testApp::exit(){
    
    // stop the audio thread first:
    ss.stop();

    
    for (int i = 0; i < numPDs; i++) {
        pitchDetectors[i].cleanup();
    }
    
    aubio_cleanup();

}



//--------------------------------------------------------------
void testApp::audioIn(float * input, int bufferSize, int nChannels){
    
    tap.getSamples(samples);
    
   // cout << samples.size() << endl;
    
    if (samples.size() > 0) {
        for (int i = 0; i < bufferSize; i++){
            //        in->data[i] = input[i*nChannels];
            in->data[i] = samples[i];
            
        }

        for (int i = 0; i < numPDs; i++) {
            pitchDetectors[i].process_pitch(in);
        }
    }
    
    
    if (bAmRecording){
        for (int i = 0; i < samples.size(); i++){
            currentNote.samples.push_back(samples[i]);
        }
        currentNote.analysisFrames.push_back(pitchDetectors[PDMethod].getPitch());
    } else  {
        currentNote.samples.clear();
        currentNote.analysisFrames.clear();
    }
    
}

void testApp::audioOut(float * output, int bufferSize, int nChannels){
    

    
    for (int i = 0; i < notes.size(); i++){
        
        //play sampler
        if ( !notes[i].bWasPlaying && notes[i].bPlaying ) {
            sampler.midiNoteOn(notes[i].mostCommonPitch + samplerOctavesUp * 12, 127);
        }
        else if ( notes[i].bWasPlaying && !notes[i].bPlaying ) {
            sampler.midiNoteOff(notes[i].mostCommonPitch + samplerOctavesUp * 12, 127);
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
    std::vector<int> histogram(150,0);
    for( int i=0; i<notes.size(); ++i )
        ++histogram[ notes[i] ];
    float maxElement =std::max_element( histogram.begin(), histogram.end() )  - histogram.begin();
    int maxCount =histogram[maxElement];
    float pct = (float)maxCount/ (float)notes.size();
    
    return maxElement;
//    mostCommonNote = maxElement;
//    mostCommonNotePct = pct;
    //cout << maxElement << " " << pct << endl;
    
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
    gui->addSlider("LPF cutoff", 1000, 20000, 20000, length-xInit, dim);
    gui->addSlider("LPF resonance", -20.0, 40.0, 0.0, length-xInit, dim);
    gui->addIntSlider("MF numPValues", 3, 33, 11, length-xInit, dim);
    gui->addSpacer(length-xInit, 1);
    gui->addSlider("Coarse Threshold", 0.0, graphMax, &coarseThreshold, length-xInit, dim);
    gui->addSlider("Fine Threshold", 0.0, 2.0, &fineThreshold, length-xInit, dim);
    gui->addLabelToggle("Coarse/Fine", &bVelFine);
    gui->addSlider("Min duration", 1, 60, &minDuration, length-xInit, dim);
    gui->addSpacer(length-xInit, 1);
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
        lpf.setParameter(kLowPassParam_CutoffFrequency, kAudioUnitScope_Global, slider->getScaledValue());
    }
    else if (name == "LPF resonance") {
     // Global, dB, -20->40, 0
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        lpf.setParameter(kLowPassParam_Resonance, kAudioUnitScope_Global, slider->getScaledValue());
    }
    else if (name == "MF numPValues") {
        ofxUIIntSlider *slider = (ofxUIIntSlider *) e.widget;
        for (int i = 0; i < numPDs; i++) {
            smoothers[i].setNumPValues(slider->getValue());
        }
    }
    else if (name == "Sampler volume") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        mixer.setInputVolume(slider->getScaledValue() * 0.2, 1);
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
        {
            PDMethod = key - 49;
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
