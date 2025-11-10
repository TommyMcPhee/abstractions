#pragma once

#include "ofMain.h"
#include "ofSoundStream.h"

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void ofSoundStreamSetup(ofSoundStreamSettings &settings);
		void audioIn(ofSoundBuffer &buffer);
		void audioOut(ofSoundBuffer &buffer);
		void update() override;
		void exit() override;
		ofSoundStreamSettings settings;
		unsigned int inDeviceIndex, outDeviceIndex;
		ofSoundStream stream;
		static const int bufferSize = 256;
		static const int inputChannels = 2;
		static const int outputChannels = 2;
		float inputBuffer[bufferSize * inputChannels];
		float phase, sample, lastSample = 0.0;
		//this test code will have alternate behavior if inputChannels != outputChannels
};
