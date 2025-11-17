#pragma once

#include "ofMain.h"
#include "ofSoundStream.h"

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		ofSoundDevice inDevice, outDevice;
		void ofSoundStreamSetup(ofSoundStreamSettings &settings);
		uint_fast32_t uintConversion(float input);
		float floatConversion(uint_fast32_t input);
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
<<<<<<< HEAD
		uint_fast32_t sampleTable[UINT32_MAX / 4];
		float phase, sample, lastSample = 0.0;
		
		uint_fast32_t sampleCount = 0;
		
		/*
		struct sampleTimes {

		}
		*/
	
=======
		float phase, sample, lastSample = 0.0;
		//this test code will have alternate behavior if inputChannels != outputChannels

		unsigned int testInt;
>>>>>>> e14e65864e416c57799e77db1a676a9ac95ad13d
};
