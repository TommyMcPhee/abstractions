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
		uint_fast32_t inputBuffer[bufferSize * inputChannels];
		uint_fast32_t sampleTable[UINT32_MAX / 4];
		float phase, sample, lastSample = 0.0;
		
		uint_fast32_t sampleCount = 0;
		
		struct sampleMap {
			uint_fast32_t bit0[2];
			uint_fast32_t bit1[4];
			uint_fast32_t bit2[8];
			uint_fast32_t bit3[16];

		}
	
};
