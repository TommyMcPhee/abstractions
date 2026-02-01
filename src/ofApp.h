#pragma once

#include "json.hpp"
#include "ofMain.h"
#include "ofSoundStream.h"
#include "ofxOscReceiver.h"
#include "ofxOscSender.h"

#include <cmath>

class ofApp : public ofBaseApp{

	public:
		std::vector<ofxOscSender> senders;
		ofxOscReceiver receiver;
		/*
		struct message_destination{
			string message_address;
			int message_port;
		};
		vector<message_destination> message_destinations;
		*/
		std::array<int, 6> buffer_sizes = {64, 128,256, 512, 1024, 2048};
		std::array<int, 4> sample_rates = {44100, 48000, 88200, 96000};
		unsigned int in_channels, out_channels, buffer_size, in_frames, out_frames;
	//TEMP SAMPLE RATE
		static const int sample_rate = 48000;
		  
		void ofSoundStreamSetup(ofSoundStreamSettings &settings);

		float epsilon_float, min_float;
		bool input = true, output = true;

		void setup() override;
		void samplewise_updates();
		std::atomic<bool> update_thread = false;
		std::atomic<float> sample_count = 0.0, parameter_smoothing = 1.0, samples_per_update = 0.0;
		std::unique_ptr<float[]> in_dc, in_amplitude_root, in_amplitude, in_cross_count, in_pitch; 
		std::unique_ptr<bool[]> in_cross;
		void analysis(float sample, float &dc, float &amplitude_root, float &amplitude, bool &cross,
			float &cross_count, float &pitch);
		float last_average_in_amplitude = 0.5, last_average_in_pitch = 0.5, last_spread_in_amplitude = 0.5, last_spread_in_pitch = 0.5;
		void audioIn(ofSoundBuffer &buffer) override;
		float mix(float inA, float inB, float mix);
		float unipolar_sin(float phase);
		//revisit calculate_ring vs calculate_value to minimize number of functions
		float calculate_ring(float progress);
		float calculate_value(float last_value, float average_in, float out, float spread_in, float ring);
		std::unique_ptr<float[]> z2, z1, out_dc, out_amplitude_root, out_amplitude, out_cross_count, out_pitch, 
			last_phase_increment, phase_increment, phase, last_amplitude, amplitude;
		std::unique_ptr<bool[]> out_cross;
		std::atomic<float> average_amplitude = 0.5, spread_amplitude = 0.5, average_pitch = 0.5, spread_pitch = 0.5;
		void audioOut(ofSoundBuffer &buffer) override;
		float update_count = 0.0, average_in_amplitude, spread_in_amplitude, average_in_pitch, spread_in_pitch, amplitude_update = 0.0, pitch_update = 0.0;
		void update() override;
		void exit() override;
		ofSoundStreamSettings settings;
		ofSoundStream stream;

		std::unique_ptr<float[]> last_in_buffer, in_buffer, previous_out;

		std::atomic<float> pitch_progress, amplitude_progress;

		//above average change (total) -> more low pass filtration for parameters and vice versa?
		//consider segregating local vs OSC variables by weighting OSC more heavily when local is unstable

		//adjusted pitch will affect which in stream is heard the most in the beginning, but later amp/pitch
		//average change in progress- more change can corrospond to less amp/pitch fluctuation?
		//this contradicts how the adjusted pitch will have more role as the piece progresses via orthogonal function to filter
		//there should be a mechanism of differing outputs more when inputs are more similar, and vice versa
		float last_sample = 0.0;
	
};