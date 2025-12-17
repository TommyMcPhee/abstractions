#pragma once

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
		unsigned int in_channels, out_channels;

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

		float lastSample = 0.0, input_sample = 0.0, sample = 0.0, averageSample, pointerValue;

		std::atomic<float> amp = 0.2;

		std::unique_ptr<float[]> input_buffer;

		std::array<float, 8> test_fir_kernel;
		std::array<float, 64> test_input_array;

		float inputBuffer[4096];

		float *testpointer = &input_sample;
	
};