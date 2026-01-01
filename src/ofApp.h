#pragma once

#include "json.hpp"
#include "ofMain.h"
#include "ofSoundStream.h"
#include "ofxOscReceiver.h"
#include "ofxOscSender.h"

class ofApp : public ofBaseApp{

	public:
		std::array<int, 6> buffer_sizes = {64, 128,256, 512, 1024, 2048};
		ofxOscReceiver receiver;
		ofxOscSender sender;
		struct message_destination{
			string message_address;
			int message_port;
		};
		vector<message_destination> message_destinations;
		unsigned int in_channels, out_channels, buffer_size, in_frames, out_frames, sample_count = 0;

		static const int sample_rate = 48000;


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

		float phase = 0.0;

		float sample = 0.0;

		//std::atomic<float> amp = 0.2;

		std::unique_ptr<float[]> in_buffer, previous_out, coefficients;
	
};