//
//  pitchDetector.h
//  aubioSSTPD
//
//  Created by Jason Levine on 2014-01-13.
//
//

#include "aubio.h"

class pitchDetector {
public:
    void setup(char_t * unit, char_t * method);
    void process_pitch(fvec_t * in);
    smpl_t getPitch() { return pitchFound; }
    void cleanup();
    
    //pitch detection
    aubio_pitch_t *o;
    aubio_wavetable_t *wavetable;
    fvec_t *pitch;
    
    uint_t samplerate;
    float pitch_tolerance;
    float silence_threshold;
    char_t * pitch_unit;
    char_t * pitch_method;
    int blocks = 0;
    smpl_t pitchFound;

};
