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
		std::array<int, 4> sample_rates = {44100, 48000, 88200, 96000};
		ofxOscReceiver receiver;
		ofxOscSender sender;
		struct message_destination{
			string message_address;
			int message_port;
		};
		vector<message_destination> message_destinations;
		unsigned int in_channels, out_channels, buffer_size, in_frames, out_frames;

		static const int sample_rate = 48000;
		  
		void ofSoundStreamSetup(ofSoundStreamSettings &settings);

		float epsilon_float, min_float;
		bool input = true;

		void setup() override;
		void samplewise_updates();
		float mod_quotient(float in, float mod);
		//float goetzel(float samples, float z0, float z1, float z2);
		std::atomic<bool> update_thread = false;
		std::atomic<float> sample_count = 0.0, samples_per_update = 0.0;
		std::unique_ptr<float[]> in_dc, in_amplitude_root, in_amplitude, in_cross_amplitude_root, in_cross_amplitude, in_cross_count, in_cross_time, in_pitch; 
		std::unique_ptr<bool[]> in_cross;
		std::atomic<float> average_in_amplitude, spread_in_amplitude, average_in_cross, spread_in_cross, average_in_pitch, spread_in_pitch;
		void analysis(float sample, float &dc, float &amplitude_root, float &amplitude, bool &cross,
			float &cross_amplitude_root, float &cross_amplitude, float &cross_count, float &cross_time, float &pitch);
		void audioIn(ofSoundBuffer &buffer) override;
		float mix(float inA, float inB, float mix);
		float unipolar_sin(float phase);
		std::unique_ptr<float[]> z2, z1, out_dc, out_amplitude_root, out_amplitude, out_cross_amplitude_root, out_cross_amplitude, out_cross_count, out_cross_time, out_pitch, 
			sin_amplitude;
		std::unique_ptr<bool[]> out_cross;
		std::atomic<float> average_out_amplitude, spread_out_amplitude, average_out_cross, spread_out_cross, average_out_pitch, spread_out_pitch;
		void audioOut(ofSoundBuffer &buffer) override;
		std::atomic<float> last_sample_count;
		float update_count;
		void update() override;
		void exit() override;
		ofSoundStreamSettings settings;
		ofSoundStream stream;

		std::unique_ptr<float[]> phase_increment, modulator_phase, carrier_phase, index;

		std::unique_ptr<float[]> last_in_buffer, in_buffer, previous_out;

		std::atomic<float> progress;

		//above average change (total) -> more low pass filtration for parameters and vice versa?
		//consider segregating local vs OSC variables by weighting OSC more heavily when local is unstable

		//adjusted pitch will affect which in stream is heard the most in the beginning, but later amp/pitch
		//average change in progress- more change can corrospond to less amp/pitch fluctuation?
		//this contradicts how the adjusted pitch will have more role as the piece progresses via orthogonal function to filter
		//there should be a mechanism of differing outputs more when inputs are more similar, and vice versa
		float last_sample = 0.0;
	
};