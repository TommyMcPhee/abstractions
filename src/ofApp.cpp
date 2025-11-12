#include "ofApp.h"
#include "ofSoundBaseTypes.h"

//--------------------------------------------------------------

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings &settings){

}

void ofApp::setup(){
    cout << "Welcome to Abstractions!" << endl;
    settings.setOutListener(this);
    settings.setInListener(this);
    settings.bufferSize = 256;
    settings.sampleRate = 48000;
    cout << stream.getDeviceList() << endl;
    cout << "Enter index of input device:" << endl;
    std::cin >> inDeviceIndex;
    //input code
    settings.setInDevice(stream.getDeviceList()[inDeviceIndex]);
    cout << "Enter index of output device:" << endl;
    std::cin >> outDeviceIndex;
    //make dynamic to the device
    settings.numInputChannels = inputChannels;
    //inputcode
    settings.setOutDevice(stream.getDeviceList()[outDeviceIndex]);
    //make dynamic to the device
    settings.numOutputChannels = outputChannels;
    //set up supported sample rates and buffer sizes
    
    //API??

    stream.setup(settings);
}

void ofApp::audioIn(ofSoundBuffer &buffer){
    for(int a = 0; a < buffer.getNumFrames(); a++){
        for(int b = 0; b < outputChannels; b++){
        inputBuffer[a * inputChannels + b] = buffer[a * inputChannels + b];
        }
    }
}

void ofApp::audioOut(ofSoundBuffer &buffer){
    cout << phase << endl;
    for(int a = 0; a < buffer.getNumFrames(); a++){
        for(int b = 0; b < outputChannels; b++){
        phase += pow(abs(inputBuffer[a * outputChannels + b] - lastSample) * 0.5, 4.0) * M_PI;
        phase = fmod(phase, TWO_PI);
        sample = sin(phase);
        buffer[a * outputChannels + b] = sample;
        lastSample = sample;
        //buffer[a * outputChannels + b] =  inputBuffer[a * outputChannels + b];
        }
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    //is the update function needed
}

//--------------------------------------------------------------
void ofApp::exit(){

}
