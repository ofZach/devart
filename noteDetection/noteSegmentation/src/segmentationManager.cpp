//
//  segmentationManager.cpp
//  noteSegmentation
//
//  Created by zach on 1/17/14.
//
//

#include "segmentationManager.h"
#include "testApp.h"
#include "computeStats.h"

void segmentationManager::setup( int numPitchDetectors, int _bufferSize ){
    
    bufferSize = _bufferSize;
    
    coarseThreshold = fineThreshold = 1.0;
    
    minDuration = 25;
    drawMarkers = true;
    bAmRecording = false;
    minPitch = 20;
    
    currentNote.nFramesRecording = 0;
}

void segmentationManager::update(float * samples, int sampleTime){
    
     // count how many frames in a row the vel is below the threshold
    if ( PDM->velGraphs[PDM->PDMethod].getLast() < (bVelFine ? fineThreshold : coarseThreshold)) {
        //count buffers
        noteRun++;
        
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
            
            float avg = 0;
            for (int i = 0; i < currentNote.analysisFrames.size(); i++){
                avg += currentNote.analysisFrames[i];
            }
            avg /= (MAX(1.0, currentNote.analysisFrames.size()));
            
            // zero periods look like 9, 10... etc...
            if (avg > minPitch){
                
                float duration = (currentNote.endTime - currentNote.startTime ) / 44100. ;
                
                currentNote.mostCommonPitch = findMostCommonPitch(currentNote);
                // sometimes, when we wrap over a loop, bad stuff happens, let's be careful:
                if (duration > 0 && currentNote.mostCommonPitch > 0){
                    
                    //add markers
                    marker segment;
                    segment.start = PDM->graphWidth - 1 - noteRun;
                    segment.end = PDM->graphWidth - 1;
                    markers.push_back(segment);
                    
                    //note metadata
                    calcPDStdDev(segment.start, segment.end);
                    calcPDAgreement(segment.start, segment.end);

                    notes.push_back(currentNote);
                    
                    ((testApp *) ofGetAppPtr()) -> addNote(currentNote.startTime, currentNote.endTime, currentNote.mostCommonPitch);
                }
                
            }
            cout << "note recorded - min duration = " << currentNote.startTime << endl << endl;
        }
        //reset
        noteRun = 0;
//        currentNote.samples.clear();
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


void segmentationManager::calcPDStdDev(int start, int end) {
    vector<float> yinValues, yinFFTValues, meloValues;
    float yinMean = 0;
    float yinFFTMean = 0;
    float meloMean = 0;
    
    for (int i = start; i < end; i++) {
        yinValues.push_back(PDM->medianGraphs[0].valHistory[i]);
        yinMean += PDM->medianGraphs[0].valHistory[i];
        
        yinFFTValues.push_back(PDM->medianGraphs[1].valHistory[i]);
        yinFFTMean += PDM->medianGraphs[1].valHistory[i];
        
        meloValues.push_back(PDM->medianGraphs[2].valHistory[i]);
        meloMean += PDM->medianGraphs[2].valHistory[i];
    }
    yinMean /= yinValues.size();
    yinFFTMean /= yinFFTValues.size();
    meloMean /= meloValues.size();
    
    currentNote.yinStdDev = computeStdDev(yinValues.begin(), yinValues.end(), yinMean);
    currentNote.yinFFTStdDev = computeStdDev(yinFFTValues.begin(), yinFFTValues.end(), yinFFTMean);
    currentNote.meloStdDev = computeStdDev(meloValues.begin(), meloValues.end(), meloMean);

    
    cout << "yin stddev " << currentNote.yinStdDev << " yinFFT stddev " << currentNote.yinFFTStdDev << " melo stddev " << currentNote.meloStdDev << endl;

}

void segmentationManager::calcPDAgreement(int start, int end) {
    float yinAgreement = 0;
    float yinFFTAgreement = 0;
    
    for (int i = start; i < end; i++) {
        yinAgreement += PDC->noteFound[0].valHistory[i];
        yinFFTAgreement += PDC->noteFound[1].valHistory[i];
    }
    
    yinAgreement /= (end-start);
    yinFFTAgreement /= (end-start);
    
    currentNote.yinAgree = yinAgreement;
    currentNote.yinFFTAgree = yinFFTAgreement;
    
    cout << "yin agree " << currentNote.yinAgree << " yinfft agree " << currentNote.yinFFTAgree << endl;

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
