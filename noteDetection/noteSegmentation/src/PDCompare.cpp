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
    
    scrollingGraph tempGraph;
    tempGraph.setup(graphWidth, 0, 0, graphMax);
    for (int i = 0; i < PDM->size(); i++) {
        medianGraphs.push_back(tempGraph);
    }
    
    means.assign(PDM->size(), 0.0);
    stdDevs.assign(PDM->size(), 0.0);
    
    
}

void PDCompare::update(){
    means.assign(PDM->size(), 0.0);
    stdDevs.assign(PDM->size(), 0.0);
    
    for (int i = 0; i < PDM->size(); i++) {
        smoothers[i].addValue(PDM->getPitch(i));
        medianGraphs[i].addValue(smoothers[i].getMedian());
        for (int j = graphWidth - nFrames; j < graphWidth; j++) {
            means[i]+=medianGraphs[i].valHistory[j];
        }
        means[i] /= nFrames;
        stdDevs[i] = computeStdDev(medianGraphs[i].valHistory.end()-nFrames, medianGraphs[i].valHistory.end(), means[i]);
        
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
        
        //mean
        float meanScaled = ofMap(means[i], 0, graphMax, height, 0, true);
        ofSetColor(0);
        ofLine(ofGetWidth() - nFrames, meanScaled, ofGetWidth(), meanScaled);
        
        //stdDEv
        float stdDevScaled = ofMap(stdDevs[i], 0, graphMax, 0, height, true);
        ofSetColor(25,25,25,100);
        ofRect(ofGetWidth() - nFrames, meanScaled - stdDevScaled, ofGetWidth(), stdDevScaled * 2);
        
        
        ofSetColor(0);
        ofDrawBitmapStringHighlight(PDM->pitchDetectors[i]->name, ofPoint(ofGetWidth()/2, height / 2));
        
        ofPopMatrix();
    }
    
    ofSetColor(255,255,255,100);
    ofRect(ofGetWidth() - nFrames, 0, nFrames, ofGetHeight());
}