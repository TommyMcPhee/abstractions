#pragma once

#include "json.hpp"
#include "ofMain.h"
#include "ofSoundStream.h"
#include "ofxOscReceiver.h"
#include "ofxOscSender.h"

#include <cmath>

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
		unsigned int in_channels, out_channels, buffer_size, in_frames, filter_frames, out_frames;

		static const int sample_rate = 48000;

		void towerOfHanoi(int n, char from_rod, char to_rod,
                  char aux_rod);
		std::array<std::array<bool, 3>, 24> hanoi;
		  
		void ofSoundStreamSetup(ofSoundStreamSettings &settings);

		float min_float, pitch = 1.0;

		void setup() override;
		float mod_quotient(float in, float mod);
		//float goetzel(float samples, float z0, float z1, float z2);
		std::atomic<float> sample_count = 0.0, cross_sample_count = 0.0, cross_count = 0.0;
		std::unique_ptr<float[]> dc, amplitude_roots, amplitude, sin_amplitude;
		std::unique_ptr<bool[]> cross;
		void crossing();
		void audioIn(ofSoundBuffer &buffer) override;
		float mix(float inA, float inB, float mix);
		void audioOut(ofSoundBuffer &buffer) override;
		void update() override;
		void exit() override;
		ofSoundStreamSettings settings;
		ofSoundStream stream;

		std::unique_ptr<float[]> phase_increment, modulator_phase, carrier_phase, index;

		std::unique_ptr<float[]> last_in_buffer, in_buffer, filter_buffer, previous_out;

		std::atomic<float> progress;
	
};