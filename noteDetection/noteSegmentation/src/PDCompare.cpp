//
//  PDCompare.cpp
//  noteSegmentation
//
//  Created by Jason Levine on 2014-01-21.
//
//

#include "PDCompare.h"


void PDCompare::setup(pitchDetectorManager * _PDM) {
    PDM = _PDM;
    
    nFrames = 10;
    
    smoother tempSmoother;
    tempSmoother.setNumPValues(11);
    for (int i = 0; i < PDM->size(); i++) {
        smoothers.push_back(tempSmoother);
    }
    
    graphWidth = ofGetWidth()/2;
    graphHeight = ofGetHeight()/3;
    graphMax = 120;
    
    scrollingGraph tempGraph;
    tempGraph.setup(graphWidth, 0, 0, graphMax);
    for (int i = 0; i < PDM->size(); i++) {
        medianGraphs.push_back(tempGraph);
    }
    
}

void PDCompare::update(){
    
    for (int i = 0; i < PDM->size(); i++) {
        smoothers[i].addValue(PDM->getPitch(i));
        medianGraphs[i].addValue(smoothers[i].getMedian());
    }
    
}


void PDCompare::draw(){
    ofSetColor(255,0,0);
    int height = ofGetHeight() / (PDM->size()+1);
    for (int i = 0; i < PDM->size(); i++) {
        ofPushMatrix();
        ofTranslate(0, i * height);
        medianGraphs[i].draw(height);
        ofDrawBitmapStringHighlight(PDM->pitchDetectors[i]->name, ofPoint(ofGetWidth()/2, height / 2));
        
        ofPopMatrix();
    }
}