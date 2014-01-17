//
//  segmentationManager.cpp
//  noteSegmentation
//
//  Created by zach on 1/17/14.
//
//

#include "segmentationManager.h"


void segmentationManager::setup( int numPitchDetectors ){
    
    int nPds = numPitchDetectors;
    smoother tempSmoother;
    tempSmoother.setNumPValues(11);
    for (int i = 0; i < nPds; i++) {
        smoothers.push_back(tempSmoother);
    }
    
    graphWidth = ofGetWidth()/2;
    graphHeight = ofGetHeight()/3;
    graphMax = 120;
    
    scrollingGraph tempGraph;
    tempGraph.setup(graphWidth, 0, 0, graphMax);
    for (int i = 0; i < nPds; i++) {
        pitchGraphs.push_back(tempGraph);
        medianGraphs.push_back(tempGraph);
        velGraphs.push_back(tempGraph);
    }
    
    
    for (int i = 0; i < nPds; i++) {
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
    minPitch = 20;
}

void segmentationManager::update(){
    
    for (int i = 0; i < PDM->size(); i++) {
        pitchGraphs[i].addValue(PDM->getPitch(i));
        smoothers[i].addValue(PDM->getPitch(i));
        medianGraphs[i].addValue(smoothers[i].getMedian());
        
        
        float lastVal = medianGraphs[i].valHistory[medianGraphs[i].valHistory.size() - 1];
        float secondLastVal = medianGraphs[i].valHistory[medianGraphs[i].valHistory.size() - 2];
        float vel = abs(lastVal - secondLastVal);
        velGraphs[i].addValue(vel);
    }
    
    
    
    
    // count how many frames in a row the vel is below the threshold
    if ( velGraphs[PDM->PDMethod].getLast() < (bVelFine ? fineThreshold : coarseThreshold)) {
        noteRun++;
        bAmRecording = true;
        pitchesForRecording.push_back(medianGraphs[PDM->PDMethod].getLast());
    }
    else  {
        // if the vel is above the thresh then check if the current run is longer than the min duration. If so save the note.  Regardless, set the run count to zero.
        
        
        bAmRecording = false;
        
        if ( noteRun > minDuration) {
            
            marker segment;
            segment.start = graphWidth - 1 - noteRun;
            segment.end = graphWidth - 1;
            
            float avg = 0;
            for (int i = 0; i < pitchesForRecording.size(); i++){
                avg += pitchesForRecording[i];
            }
            avg /= (MAX(1.0, pitchesForRecording.size()));
            
            pitchesForRecording.clear();
            // zero periods look like 9, 10... etc...
            if (avg > minPitch){
                markers.push_back(segment);
            }
//            currentNote.playhead = 0;
//            currentNote.bPlaying = true;
//            currentNote.bWasPlaying = false;
//            currentNote.mostCommonPitch = findMostCommonPitch(currentNote);
//            notes.push_back(currentNote);
            
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

void segmentationManager::draw(){
    
    ofSetColor(25);
    ofLine(ofGetWidth() - minDuration * 2, 90, ofGetWidth(), 90);
    ofLine(ofGetWidth() - minDuration * 2, 92, ofGetWidth() - minDuration * 2, 88);
    
    (noteRun < minDuration) ? ofSetColor(255,0,0) : ofSetColor(0,255,0);
    ofLine(ofGetWidth() - noteRun * 2, 95, ofGetWidth(), 95);
    
    ofPushMatrix();
    ofTranslate(0, 100);
    ofSetColor(255, 10, 10);
    medianGraphs[ PDM->getMethod() ].draw(graphHeight);
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
        velGraphs[ PDM->getMethod() ].draw(graphHeight, 0.0, 2.0);
    }
    else {
        velGraphs[ PDM->getMethod() ].draw(graphHeight);
    }
    
    if (drawMarkers) {
        
        ofSetColor(0);
        float threshLineHeight = graphHeight - ((bVelFine ? fineThreshold/2*graphMax : coarseThreshold ) / graphMax * graphHeight);
        ofLine(0, threshLineHeight, ofGetWidth(), threshLineHeight);
        
        ofSetColor(0,0,0,127);
        for (int j = 0; j < velGraphs[ PDM->getMethod() ].valHistory.size(); j++) {
            if ( velGraphs[ PDM->getMethod() ].valHistory[j] > (bVelFine ? fineThreshold : coarseThreshold ) ) {
                ofLine(j * 2, -graphHeight, j * 2, graphHeight);
            }
        }
        
    }
    
    ofPopMatrix();

    
}