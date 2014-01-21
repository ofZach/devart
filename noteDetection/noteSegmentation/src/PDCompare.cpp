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
        noteFound.push_back(tempGraph);
    }
    
    means.assign(PDM->size(), 0.0);
    stdDevs.assign(PDM->size(), 0.0);
    
    stdDevThresh = 5;
    
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
        
        noteFound[i].addValue((stdDevs[i] < stdDevThresh) ? graphMax : 0);
//        if (stdDevs[i] < stdDevThresh) {
//            for (int j = nFrames; j > 0; j--) {
//                noteFound[i].valHistory[graphWidth-j] = graphMax;
//            }
//        }
        
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
        noteFound[i].draw(height, 0, graphMax, true);
        
        
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
        
        ofPushMatrix();
        ofTranslate(0, (PDM->size()) * height);
        ofSetColor(25,200,25,50);
        noteFound[i].draw(height, 0, graphMax, true);
        ofPopMatrix();

        
    }
    
    
    
    ofSetColor(255,255,255,100);
    ofRect(ofGetWidth() - nFrames * 2, 0, nFrames * 2, ofGetHeight());
}