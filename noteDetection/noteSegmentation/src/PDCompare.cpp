//
//  PDCompare.cpp
//  noteSegmentation
//
//  Created by Jason Levine on 2014-01-21.
//
//

#include "PDCompare.h"
#include "computeStats.h"


void PDCompare::setup(pitchDetectorManager * _PDM) {
    PDM = _PDM;
    
    nFrames = 25;
    
    smoother tempSmoother;
    tempSmoother.setNumPValues(11);
    for (int i = 0; i < PDM->size(); i++) {
        smoothers.push_back(tempSmoother);
    }
    
    graphWidth = ofGetWidth()/2;
    graphHeight = ofGetHeight()/3;
    graphMax = 120;
    
    scrollingGraph medianGraph;
    medianGraph.setup(graphWidth, 0, 0, graphMax);
    
    scrollingGraph noteFoundGraph;
    noteFoundGraph.setup(graphWidth, 0, 0, 1.0);
    for (int i = 0; i < PDM->size(); i++) {
        medianGraphs.push_back(medianGraph);
        noteFound.push_back(noteFoundGraph);
    }
    
    agreedNotes.assign(graphWidth, 0.0);
    means.assign(PDM->size(), 0.0);
    stdDevs.assign(PDM->size(), 0.0);
    
    stdDevThresh = 5;
    
    sum = true;
}

void PDCompare::update(){
    //clear stats
    means.assign(PDM->size(), 0.0);
    stdDevs.assign(PDM->size(), 0.0);
    
    for (int i = 0; i < PDM->size(); i++) {
        //update graphs
        smoothers[i].addValue(PDM->getPitch(i));
        medianGraphs[i].addValue(smoothers[i].getMedian());
        
        //calculate stats
        for (int j = graphWidth - nFrames; j < graphWidth; j++) {
            means[i]+=medianGraphs[i].valHistory[j];
        }
        means[i] /= nFrames;
        stdDevs[i] = computeStdDev(medianGraphs[i].valHistory.end()-nFrames, medianGraphs[i].valHistory.end(), means[i]);
        
        //if wwe are in a note go back nframes and make them all "in a note"
        int isNote = (stdDevs[i] < stdDevThresh && means[i] > 0) ? 1 : 0;
        noteFound[i].addValue(isNote);
        if (isNote == 1) {
            for (int j = nFrames; j > 0; j--) {
                noteFound[i].valHistory[graphWidth - j] = 1;
            }
        }
        
    }
    
    //find the intersection of PDs
    for (int i = 0; i < graphWidth; i++) {
        int agreeCount = 0;
        for (int j = 0; j < noteFound.size(); j++) {
            agreeCount += noteFound[j].valHistory[i];
        }
        agreedNotes[i] = (agreeCount == noteFound.size() ? 1 : 0);
    }
    
    //if the frames before graphWidth - nframes is not a note and the frame right after is a note, then we have found the start.
    if (agreedNotes[graphWidth-nFrames-1] == 0 && agreedNotes[graphWidth-nFrames-1] == 1) {
        
    }
    
    //if the last frame is a note and the current is not, we have foudn the end of a note
    if (agreedNotes[graphWidth-2] == 1 && agreedNotes[graphWidth-1] == 0) {
        
    }

}


void PDCompare::draw(){
    
    int height = ofGetHeight() / (PDM->size()+1);
    for (int i = 0; i < PDM->size(); i++) {
        ofPushMatrix();
        ofTranslate(0, i * height);
        
        //graphs
        ofSetColor(255,0,0);
        medianGraphs[i].draw(height);
        
        ofSetColor(25,200,25,50);
        noteFound[i].draw(height, 0, 1, true);
        
        
        //mean
        float meanScaled = ofMap(means[i], 0, graphMax, height, 0, true);
        ofSetColor(0);
        ofLine(ofGetWidth() - nFrames * 2, meanScaled, ofGetWidth(), meanScaled);
        
        //stdDEv
        float stdDevScaled = ofMap(stdDevs[i], 0, graphMax, 0, height, true);
        (stdDevs[i] < stdDevThresh) ? ofSetColor(25,200,25,100) : ofSetColor(200,25,25,100);
        ofRect(ofGetWidth() - nFrames * 2, meanScaled - stdDevScaled, ofGetWidth(), stdDevScaled * 2);
        
        
        ofSetColor(0);
        ofDrawBitmapStringHighlight(PDM->pitchDetectors[i]->name, ofPoint(ofGetWidth()/2, height / 2));
        
        ofPopMatrix();
        
        if(sum) {
            ofPushMatrix();
            ofTranslate(0, (PDM->size()) * height);
            ofSetColor(25,200,25,100);
            noteFound[i].draw(height, 0, 1, true);
            ofPopMatrix();
        }

        
    }
    
    if (!sum) {
        ofPushMatrix();
        ofTranslate(0, (PDM->size()) * height);
        ofSetColor(25,200,25,200);
        for (int i = 0; i < graphWidth; i++) {
            ofRect(i*2, 0, 2, agreedNotes[i] * height);
        }
        ofPopMatrix();
    }

    
    ofSetColor(255,255,255,100);
    ofRect(ofGetWidth() - nFrames * 2, 0, nFrames * 2, ofGetHeight());
}