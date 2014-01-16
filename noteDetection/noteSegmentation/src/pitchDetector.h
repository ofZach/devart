//
//  pitchDetector.h
//  aubioSSTPD
//
//  Created by Jason Levine on 2014-01-13.
//
//

#include "aubio.h"
#include "ofMain.h"


//---------------------------------------------------------------------------------
class basePitchDetector {
    
    virtual void calculatePitch(float * buffer, int bufferSize, int bufferSamplePos){}
    virtual float getPitch(){};
    
};


//---------------------------------------------------------------------------------
class aubioPitchDetector : public basePitchDetector {

public:
    
    void setup(char_t * unit, char_t * method, uint_t _win_s, uint_t _hop_s);
    
    fvec_t * inputBuffer;
    //= new_fvec (hop_s); // input buffer
    
    
    virtual void calculatePitch(float * buffer, int bufferSize, int bufferSamplePos){
        
        // TODO: put this into a fvec_t;;;
//        if (inputBuffer == NULL){
//            inputBuffer = new_fvec (bufferSize);
//        }
        
        for (int i = 0; i < bufferSize; i++){
            inputBuffer->data[i] = buffer[i];
        }
        
        process_pitch(inputBuffer);
    }
    
    void process_pitch(fvec_t * in);
    float getPitch() { return pitchFound; }
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


//---------------------------------------------------------------------------------
class filePitchDetector : public basePitchDetector {
public:
    
    void loadAssociatedFile( string fileName );
    float getPitch();
    
};
