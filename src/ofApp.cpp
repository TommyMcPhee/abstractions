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
    cout << stream.getDeviceList() << endl;
    cout << "Enter index of input device:" << endl;
    std::cin >> inDeviceIndex;
    inDevice = stream.getDeviceList()[inDeviceIndex];
    settings.setInDevice(inDevice);
    cout << "Enter index of output device:" << endl;
    std::cin >> outDeviceIndex;
    outDevice = stream.getDeviceList()[outDeviceIndex];
    //make dynamic to the device
    settings.numInputChannels = inputChannels;
    //inputcode
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

void ofApp::audioIn(ofSoundBuffer &buffer){
    for(int a = 0; a < buffer.getNumFrames(); a++){
        for(int b = 0; b < outputChannels; b++){
        inputBuffer[a * inputChannels + b] = buffer[a * inputChannels + b];
        }
    }
}

void ofApp::audioOut(ofSoundBuffer &buffer){
    //cout << phase << endl;
    cout << testInt << endl;
        
    for(int a = 0; a < buffer.getNumFrames(); a++){
        for(int b = 0; b < outputChannels; b++){
        phase += pow(abs(inputBuffer[a * outputChannels + b] - lastSample) * 0.5, 4.0) * M_PI;
        phase = fmod(phase, TWO_PI);
        sample = sin(phase);
        buffer[a * outputChannels + b] = sample;
        lastSample = sample;
        testInt = 0;
        //buffer[a * outputChannels + b] =  inputBuffer[a * outputChannels + b];
        }
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    //is the update function needed
    testInt++;
}

//--------------------------------------------------------------
void ofApp::exit(){

}
