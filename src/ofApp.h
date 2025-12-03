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
		//std::array<std::atomic<uint_fast32_t>, UINT32_MAX / 4> sampleTable;
		//float phase, sample, lastSample = 0.0;
		std::array<std::atomic<uint_fast32_t>, 16> sampleTable;
		uint_fast32_t sampleCount = 0;
		uint_fast32_t increment = 0;

		float lastSample = 0.0, sample = 0.0, averageSample;
	
};
