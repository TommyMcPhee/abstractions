#include "ofApp.h"
#include "ofSoundBaseTypes.h"
#include <cstdint>

//--------------------------------------------------------------

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings &settings){

}

void ofApp::setup(){
    cout << "Welcome to Abstractions!" << endl;
    settings.setOutListener(this);
    settings.setInListener(this);
    settings.bufferSize = 256;
    cout << stream.getDeviceList() << endl;
    cout << "Enter index of input device:" << endl;
    std::cin >> inDeviceIndex;
    inDevice = stream.getDeviceList()[inDeviceIndex];
    settings.setInDevice(inDevice);
    cout << "Enter index of output device:" << endl;
    std::cin >> outDeviceIndex;
    outDevice = stream.getDeviceList()[outDeviceIndex];
    settings.numInputChannels = inputChannels;
    settings.setOutDevice(stream.getDeviceList()[outDeviceIndex]);
    for(int a = 0; a < inDevice.sampleRates.size(); a++){
    cout << inDevice.sampleRates[a] << endl;
    }
    
    //make dynamic to the device
    settings.numOutputChannels = outputChannels;
    //set up supported sample rates and buffer sizes
    //FIX
    cout << "Press any key for advanced settings, ENTER to begin the piece." << endl;
    if (std::cin.get() == '\n'){
        cout << "Good job.\n";
    }
    else{
    cout << "I meant ONLY the ENTER key... Oh well.\n";
    }
    //API??
    settings.sampleRate = 48000;
    stream.setup(settings);
}

uint_fast32_t ofApp::uintConversion(float input){
    //cout << static_cast<uint_fast32_t>(input * std::numeric_limits<float>::max()) << endl;
    //return static_cast<uint_fast32_t>(input * std::numeric_limits<float>::max());
    float output;
    memcpy(&output, &input, sizeof(uint_fast32_t));
    return output;
}

float ofApp::floatConversion(uint_fast32_t input){
    //cout << static_cast<float>(input * std::numeric_limits<float>::min()) << endl;
    float output;
    memcpy(&output, &input, sizeof(uint_fast32_t));
    return output;
}

void ofApp::audioIn(ofSoundBuffer &buffer){
    for(int a = 0; a < buffer.getNumFrames(); a++){
        //sampleCount++;
        for(int b = 0; b < outputChannels; b++){
        float inputSample = buffer[a * inputChannels + b];
        if(std::fpclassify(inputSample) == FP_SUBNORMAL){
            inputSample = 0.0;
            //cout << "Subnormal" << endl;
        }
        uint_fast32_t inputUint = uintConversion(inputSample);
        inputBuffer[a * inputChannels + b] = inputUint;
        sampleTable[inputUint] = sampleCount;
        }
    }
}

void ofApp::audioOut(ofSoundBuffer &buffer){      
    for(int a = 0; a < buffer.getNumFrames(); a++){
        for(int b = 0; b < outputChannels; b++){
        sampleCount++;
        /*
        phase += pow(abs(inputBuffer[a * outputChannels + b] - lastSample) * 0.5, 4.0) * M_PI;
        phase = fmod(phase, TWO_PI);
        sample = sin(phase);
        */
        //lastSample = sample;
        //sample = floatConversion(sampleCount);
        sample = floatConversion(sampleTable[inputBuffer[a * outputChannels + b]]);
        averageSample += sample;
        buffer[a * outputChannels + b] = sample;
        }
    }
    averageSample = averageSample / 256.0;
    cout << averageSample << endl;
    averageSample = 0.0;
}

//--------------------------------------------------------------
void ofApp::update(){
    /*
    while(true){
        increment++;
        sampleTable[increment] = rand();
        //cout << increment << endl;
        if(increment >= sampleTable.size()){
            increment = 0;
        }
    }
        */
}

//--------------------------------------------------------------
void ofApp::exit(){

}
