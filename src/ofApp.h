#pragma once

#include "ofMain.h"
#include "ofSoundStream.h"
#include "ofxOscReceiver.h"
#include "ofxOscSender.h"

class ofApp : public ofBaseApp{

	public:
		ofxOscSender sender;
		ofxOscReceiver receiver;
		unsigned int in_channels, out_channels;
		void setup() override;
		void ofSoundStreamSetup(ofSoundStreamSettings &settings);
		void audioIn(ofSoundBuffer &buffer);
		void audioOut(ofSoundBuffer &buffer);
		void update() override;
		void exit() override;
		ofSoundStreamSettings settings;
		ofSoundStream stream;
		static const int bufferSize = 256;
		//fix
		float inputBuffer[bufferSize * 2];
		//std::array<std::atomic<uint_fast32_t>, UINT32_MAX / 4> sampleTable;
		//float phase, sample, lastSample = 0.0;
		std::array<std::atomic<uint_fast32_t>, 16> sampleTable;
		uint_fast32_t sampleCount = 0;
		uint_fast32_t increment = 0;

		float lastSample = 0.0, sample = 0.0, averageSample;
	
};