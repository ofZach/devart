//
//  scrollingGraph.cpp
//  audioshadersmesh
//
//  Created by Jason Levine on 2014-01-06.
//
//

#include "scrollingGraph.h"

void scrollingGraph::setup(int _size, float init, float min, float max) {
    size = _size;
    valHistory.assign(size, init);
    minVal = min;
    maxVal = max;
}

void scrollingGraph::addValue(float val) {
    valHistory.push_back(val);
    if (valHistory.size() > size) valHistory.erase(valHistory.begin());
}

void scrollingGraph::draw(float height, float min, float max) {
    if ( min == 0 && max == 0 ) {
        min = minVal;
        max = maxVal;
    }
    
    ofNoFill();
    ofBeginShape();
    for (unsigned int j = 0; j < valHistory.size(); j++){
//        float valNormalized = ofMap(valHistory[j], minVal, maxVal, 0.0, 1.0);
        float y = ofMap(valHistory[j], min, max, height, 0, true);
        ofVertex(j*2, y);
    }
    ofEndShape(false);
    ofFill();
}


/*
 //--------------------------------------------------------------
 void audioAnalytics::drawAmpHistory(int track, float height){
 ofBeginShape();
 for (unsigned int j = 0; j < ampHistory[track].size(); j++){
 if( j == 0 ) ofVertex(j + 100, height);
 ofVertex(j + 100, height - ampHistory[track][j] * height);
 
 if( j == ampHistory[track].size() -1 ) ofVertex(j + 100, height);
 }
 ofEndShape(false);
 
 ofRect(ofGetWidth() - 90, height, 80, -amp[track] * height);
 //    float size = ampNormalized * height;
 //ofCircle(ofGetWidth() - 50, height - size / 2, size/2);
 
 }
*/