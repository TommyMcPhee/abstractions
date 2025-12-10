#pragma once

#include "ofMain.h"
#include "ofSoundStream.h"
#include "ofxOscReceiver.h"
#include "ofxOscSender.h"

class ofApp : public ofBaseApp{

	public:
		std::array<int, 6> buffer_sizes;
		ofxOscReceiver receiver;
		ofxOscSender sender;
		struct message_destination{
			string message_address;
			int message_port;
		};
		vector<message_destination> message_destinations;
		unsigned int in_channels, out_channels;
		void setup() override;
		void ofSoundStreamSetup(ofSoundStreamSettings &settings);
		void audioIn(ofSoundBuffer &buffer) override;
		void audioOut(ofSoundBuffer &buffer) override;
		void update() override;
		void exit() override;
		ofSoundStreamSettings settings;
		ofSoundStream stream;

		
		uint_fast32_t sampleCount = 0;
		uint_fast32_t increment = 0;

		float lastSample = 0.0, sample = 0.0, averageSample;
	
};