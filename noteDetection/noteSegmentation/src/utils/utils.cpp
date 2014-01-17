//
//  utils.cpp
//  noteSegmentation
//
//  Created by zach on 1/17/14.
//
//

#include "utils.h"




//---------------------------------------------------------------------------------------
float freq2midi(float freq) {
    
    // log 0 is not defined.
    // here, we check for really low or negative freq and return 0
    if (freq < 0.001) return 0;
    
    // else rock and roll:
    return ( 69. + 12.*log(freq/440.)/log((float)2.) );
}



//---------------------------------------------------------------------------------------
int findMostCommon (vector < int > & vals){
    
    //--------------------------------------
    //    vector < int > val;
    //    val.push_back(3);
    //    val.push_back(3);
    //    val.push_back(7);
    //    cout << findMostCommon (val) << endl;
    //    output: 3
    //--------------------------------------
    
    // find the range...
    
    int minElement = *(std::min_element(vals.begin(), vals.end()));
    int maxElement = *(std::max_element(vals.begin(), vals.end()));

    int range = maxElement - minElement + 1;
    
    // note: +1 since we need to include both min and max element
    // (its not min up to max but min up to and including max)
    
    // make a histogram:
    
    std::vector<int> histogram(range,0);
    for( int i=0; i<vals.size(); ++i ){
        ++histogram[ vals[i] - minElement ];
    }
    
    // find max of histogram:
    
    std::vector<int>::iterator result;
    result = (std::max_element( histogram.begin(), histogram.end() ));
    
    // find the value which is here the most.  also, if we want, the count (which isn't returned yet):
    
    int valMax =  std::distance(histogram.begin(), result) + minElement;
    int valMaxCount = *result;
    
    return valMax;
    
}

