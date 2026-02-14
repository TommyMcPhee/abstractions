#pragma once

#include "ofMain.h"
#include "ofSoundStream.h"
#include "ofxOscReceiver.h"
#include "ofxOscSender.h"

#include <cmath>

class ofApp : public ofBaseApp{

	public:	
		float epsilon_float, min_float, in_channels_float;
		ofSoundStreamSettings settings;
		int in_channels, out_channels;
		bool output = true, input = true, spread = true;
		void cin_refresh();
		ofxOscReceiver receiver;
		void osc_setup_warning();
		void receiver_setup();
		std::vector<ofxOscSender> senders;
		void add_sender();
		std::array<int, 6> buffer_sizes = {64, 128,256, 512, 1024, 2048};
		std::array<int, 4> sample_rates = {44100, 48000, 88200, 96000};
		void print_array_value(int index, int value);
		void unsigned_integer_warning();
		ofSoundStream stream;
		void ofSoundStreamSetup(ofSoundStreamSettings &settings);
		void setup() override;
		std::array<std::atomic<bool>, 4> update_thread = { false, false, false, false };
		std::array<std::atomic<float>, 4> parameter_smoothing, samples_per_update, average_in, spread_in, update_in;
		std::atomic<float> sample_count = 0.0, reciprocal_sample_count;
		void samplewise_updates();
		std::unique_ptr<float[]> in_z2, in_z1, in_dc, in_amplitude_root, in_cross_count;
		std::array<std::unique_ptr<float[]>, 4> in_parameters;
		std::unique_ptr<bool[]> in_cross;

		//std::atomic<float> average_in_amplitude, spread_in_amplitude, average_in_pitch, spread_in_pitch, amplitude_update, pitch_update;
		float calculate_delta(float previous, float current);
		void average(float &value, float new_value);
		void analysis(float z2, float z1, float sample, float &dc, float &amplitude_root, float &amplitude, 
			float &delta, float &slope, bool &cross, float &cross_count, float &pitch);

		//float last_average_in_amplitude = 0.5, last_average_in_pitch = 0.5, last_spread_in_amplitude = 0.5, last_spread_in_pitch = 0.5;
		void audioIn(ofSoundBuffer &buffer) override;
		float mix(float inA, float inB, float mix);
		float unipolar_sin(float phase);
		//revisit calculate_ring vs calculate_value to minimize number of functions
		float calculate_ring(float progress);
		float calculate_value(float last_value, float average_in, float parameter_smoothing, float out, float spread_in);

		std::unique_ptr<float[]> out_z2, out_z1, out_dc, out_amplitude_root, out_amplitude, out_delta, out_slope, out_cross_count, out_pitch, 
			modulator_phase, last_phase_increment, phase_increment, phase, last_amplitude, amplitude, last_delta, delta, last_slope, slope;
		
		std::unique_ptr<bool[]> out_cross;

		std::atomic<float> average_amplitude = 0.5, spread_amplitude = 0.5, average_delta = 0.5, spread_delta = 0.5, 
			average_slope = 0.5, spread_slope = 0.5, average_pitch = 0.5, spread_pitch = 0.5;

		void audioOut(ofSoundBuffer &buffer) override;
		float update_count = 0.0;
		float progress_increment(float last_average, float average, float last_spread, float spread);
		//void receive_message(ofxOscMessage message, float &average, float &spread, float &progress, float &last_average, float &last_spread)
		std::array<string, 4> addresses = {"/amplitude", "/delta", "/slope", "/pitch"};
		void update() override;
		void ofSoundStreamClose();
		void exit() override;

		std::atomic<float> pitch_progress, amplitude_progress;

		//above average change (total) -> more low pass filtration for parameters and vice versa?
		//consider segregating local vs OSC variables by weighting OSC more heavily when local is unstable

		//adjusted pitch will affect which in stream is heard the most in the beginning, but later amp/pitch
		//average change in progress- more change can corrospond to less amp/pitch fluctuation?
		//this contradicts how the adjusted pitch will have more role as the piece progresses via orthogonal function to filter
		//there should be a mechanism of differing outputs more when inputs are more similar, and vice versa
	
};