//
//  segmentationManager.cpp
//  noteSegmentation
//
//  Created by zach on 1/17/14.
//
//

#include "segmentationManager.h"
#include "testApp.h"

void segmentationManager::setup( int numPitchDetectors, int _bufferSize ){
    
    bufferSize = _bufferSize;
    
    coarseThreshold = fineThreshold = 1.0;
    
    minDuration = 25;
    drawMarkers = true;
    bAmRecording = false;
    minPitch = 20;
    
    
    //segment playback
    sinAngle = 0;
    audioVol = 1.0;
    sinVol = 0.0;
    samplerOctavesUp = sinOctavesUp = 0;
    
    currentNote.nFramesRecording = 0;
}

void segmentationManager::update(float * samples, int sampleTime){
    
     // count how many frames in a row the vel is below the threshold
    if ( PDM->velGraphs[PDM->PDMethod].getLast() < (bVelFine ? fineThreshold : coarseThreshold)) {
        //count buffers
        noteRun++;
        
        // record samples
        for (int i = 0; i < bufferSize; i++ ) {
            currentNote.samples.push_back(samples[i]);
        }
        
        if (currentNote.nFramesRecording == 0){
            currentNote.startTime = sampleTime;
        }
        
        currentNote.endTime = sampleTime + bufferSize;
        currentNote.nFramesRecording++;
        
        //record pitches
        currentNote.analysisFrames.push_back(PDM->medianGraphs[PDM->PDMethod].getLast());

    }
    else  {
        
        // if the vel is above the thresh then check if the current run is longer than the min duration. If so save the note.  Regardless, set the run count to zero.
        if ( noteRun > minDuration) {
            
            marker segment;
            segment.start = PDM->graphWidth - 1 - noteRun;
            segment.end = PDM->graphWidth - 1;
            
            float avg = 0;
            for (int i = 0; i < currentNote.analysisFrames.size(); i++){
                avg += currentNote.analysisFrames[i];
            }
            avg /= (MAX(1.0, currentNote.analysisFrames.size()));
            
            // zero periods look like 9, 10... etc...
            if (avg > minPitch){
                //add markers
                markers.push_back(segment);
                //add correspnding note
                currentNote.mostCommonPitch = findMostCommonPitch(currentNote);
                
                
                float duration = (currentNote.endTime - currentNote.startTime ) / 44100. ;
                // sometimes, when we wrap over a loop, bad stuff happens, let's be careful:
                if (duration > 0 && currentNote.mostCommonPitch > 0){
                    
                    notes.push_back(currentNote);
                    
                    cout << "wout context" << duration << endl;
                    ((testApp *) ofGetAppPtr()) -> addNote(currentNote.startTime, currentNote.endTime, currentNote.mostCommonPitch);
                }
                
            }
            cout << "note recorded - min duration = " << currentNote.startTime << endl << endl;
        }
        //reset
        noteRun = 0;
        currentNote.samples.clear();
        currentNote.analysisFrames.clear();
        currentNote.nFramesRecording = 0;
        currentNote.startTime = 0;
        currentNote.endTime = 0;
    }

    scrollMarkers();

}

void segmentationManager::draw(){
    
    //min duration measure
    ofSetColor(25);
    ofLine(ofGetWidth() - minDuration * 2, 90, ofGetWidth(), 90);
    ofLine(ofGetWidth() - minDuration * 2, 92, ofGetWidth() - minDuration * 2, 88);
    
    //current run
    (noteRun < minDuration) ? ofSetColor(255,0,0) : ofSetColor(0,255,0);
    ofLine(ofGetWidth() - noteRun * 2, 95, ofGetWidth(), 95);
    
    //median graph of chosen PD
    ofPushMatrix();
    ofTranslate(0, 100);
    ofSetColor(255, 10, 10);
    
    PDM->medianGraphs[ PDM->getMethod() ].draw(PDM->graphHeight);

    //markers
    for (int i = 0; i < markers.size(); i++) {
        ofSetColor(255,255,255,127);
        ofRect(markers[i].start * 2, 0, markers[i].end * 2 - markers[i].start * 2, PDM->graphHeight);
    }

    ofPopMatrix();
    
    //velocity graphs
    ofPushMatrix();
    ofTranslate(0, PDM->graphHeight + 100);
    ofSetColor(255,0,0);

    if (bVelFine) {
        PDM->velGraphs[ PDM->getMethod() ].draw(PDM->graphHeight, 0.0, 2.0);
    }
    else {
        PDM->velGraphs[ PDM->getMethod() ].draw(PDM->graphHeight);
    }
    
    //thresh line
    ofSetColor(0);
    float threshLineHeight = PDM->graphHeight - ((bVelFine ? fineThreshold/2*PDM->graphMax : coarseThreshold ) / PDM->graphMax * PDM->graphHeight);
    ofLine(0, threshLineHeight, ofGetWidth(), threshLineHeight);
    
    
    //cut lines
    ofSetColor(0,0,0,127);
    for (int j = 0; j < PDM->velGraphs[ PDM->getMethod() ].valHistory.size(); j++) {
        if ( PDM->velGraphs[ PDM->getMethod() ].valHistory[j] > (bVelFine ? fineThreshold : coarseThreshold ) ) {
            ofLine(j * 2, -PDM->graphHeight, j * 2, PDM->graphHeight);
        }
    }

    
    ofPopMatrix();
}



void segmentationManager::scrollMarkers(){

    if (markers.size() > 0) {
        for (int i = 0; i < markers.size(); i++) {
            markers[i].start--;
            markers[i].end--;
        }
    }
}


void segmentationManager::playSegments(vector<float> &output){
    

    for (int i = 0; i < notes.size(); i++){
        
        //play sampler
        if ( !notes[i].bWasPlaying && notes[i].bPlaying ) {
            AU->startNote(notes[i].mostCommonPitch + samplerOctavesUp * 12);
        }
        else if ( notes[i].bWasPlaying && !notes[i].bPlaying ) {
            AU->stopNote(notes[i].mostCommonPitch + samplerOctavesUp * 12);
        }
        
        //while audio clips are not finshed playing
        if (notes[i].bPlaying == true && (notes[i].playhead + bufferSize) < notes[i].samples.size() ){ //
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
            
        }
        else {
            notes[i].bPlaying = false;
        }
        
        notes[i].bWasPlaying = notes[i].bPlaying;
    }

}


float segmentationManager::findMostCommonPitch(audioNote note){
    
    vector < int > properPitches;
    
    for (int i = 0; i < note.analysisFrames.size(); i++){
        float detectedPitch = note.analysisFrames[i];
        if (detectedPitch > minPitch && detectedPitch < 150) properPitches.push_back(detectedPitch);
    }
    // see utils.h
    
    int mostCommon =findMostCommon(properPitches);
    
    //cout << "-----------" << endl;
    float avg = 0;
    for (int i = 0; i < properPitches.size(); i++){
      //  cout << properPitches[i] << endl;
        avg +=properPitches[i];
    }
    avg /= (float)properPitches.size();
    
    //cout << avg << " " << mostCommon << endl;
    int count = 0;
    for (int i = 0; i < properPitches.size(); i++){
        if (properPitches[i] == mostCommon){
            count ++;
        }
    }
    
    float pct = (float)count / (float)(MAX(1, properPitches.size()));
    
    
    if (pct < 0.35){
        return -1;
    } else
    
    return (int)mostCommon;
    
}
