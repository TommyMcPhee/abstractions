#include "ofApp.h"
#include "ofSoundBaseTypes.h"
#include <cmath>
#include <ofxOsc.h>
//--------------------------------------------------------------

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings &settings){

}

void ofApp::setup(){
    epsilon_float = std::numeric_limits<float>::epsilon();
    min_float = std::numeric_limits<float>::denorm_min();
    //should progress be min_float or 0.0?
    progress = epsilon_float;

    cout << "Welcome to Abstractions!" << endl;
    settings.setOutListener(this);
    settings.setInListener(this);
    unsigned int in_device_index, out_device_index, buffer_size_index, sample_rate_index;
    ofSoundDevice in_device, out_device;
    cout << stream.getDeviceList() << endl;
    cout << "Enter index of input device:" << endl;
    std::cin >> in_device_index;
    in_device = stream.getDeviceList()[in_device_index];
    settings.setInDevice(in_device);
    in_channels = in_device.inputChannels;
    settings.numInputChannels = in_channels;
    in_dc = std::make_unique<float[]>(in_channels);
    in_amplitude_root = std::make_unique<float[]>(in_channels);
    in_amplitude = std::make_unique<float[]>(in_channels);
    in_cross = std::make_unique<bool[]>(in_channels);
    in_cross_amplitude_root = std::make_unique<float[]>(in_channels);
    in_cross_amplitude = std::make_unique<float[]>(in_channels);
    in_cross_count = std::make_unique<float[]>(in_channels);
    in_cross_time = std::make_unique<float[]>(in_channels);
    in_pitch = std::make_unique<float[]>(in_channels);

    //reevaluate

    for(int a = 0; a < in_channels; a++){
        in_dc[a] = 0.0;
        in_amplitude_root[a] = 0.0;
        in_amplitude[a] = 0.0;
        in_cross[a] = false;
        in_cross_count[a] = 0.0;
        in_cross_time[a] = 0.0;
        in_pitch[a] = 1.0;
    }
    cout << "Enter index of output device:" << endl;
    if(!std::cin >> out_device_index || out_device_index >= stream.getDeviceList().size() ){

    }
    std::cin >> out_device_index;
    out_device = stream.getDeviceList()[out_device_index];
    settings.setOutDevice(stream.getDeviceList()[out_device_index]);
    out_channels = out_device.outputChannels;
    settings.numOutputChannels = out_channels;

    sin_amplitude = std::make_unique<float[]>(out_channels);
    phase_increment = std::make_unique<float[]>(out_channels);
    modulator_phase = std::make_unique<float[]>(out_channels);
    carrier_phase = std::make_unique<float[]>(out_channels);
    index = std::make_unique<float[]>(out_channels);

    out_dc = std::make_unique<float[]>(out_channels);
    out_amplitude_root = std::make_unique<float[]>(out_channels);
    out_amplitude = std::make_unique<float[]>(out_channels);
    out_cross = std::make_unique<bool[]>(out_channels);
    out_cross_amplitude_root = std::make_unique<float[]>(out_channels);
    out_cross_amplitude = std::make_unique<float[]>(out_channels);
    out_cross_count = std::make_unique<float[]>(out_channels);
    out_cross_time = std::make_unique<float[]>(out_channels);
    out_pitch = std::make_unique<float[]>(out_channels);
    z2 = std::make_unique<float[]>(out_channels);
    z1 = std::make_unique<float[]>(out_channels);

    for(int a = 0; a < out_channels; a++){
        out_dc[a] = 0.0;
        out_amplitude_root[a] = 0.0;
        out_amplitude[a] = 0.0;
        out_cross[a] = false;
        out_cross_count[a] = 0.0;
        out_cross_time[a] = 0.0;
        out_pitch[a] = 1.0;
        z2[a] = 0.0;
        z1[a] = 0.0;
    }
    /*
    for(int a = 0; a < in_device.sampleRates.size(); a++){
    cout << in_device.sampleRates[a] << endl;
    }
    vector<int> shared_sample_rates;
    for(int a = 0; a < in_device.sampleRates.size(); a++){
        for(int b = 0; b < out_device.sampleRates.size(); b++)
        if(in_device.sampleRates[a] == out_device.sampleRates[b]){
            shared_sample_rates.push_back(in_device.sampleRates[a]);
            cout << out_device.sampleRates[b] << endl;
            break;
        }
    }
    for(int a = 0; a < shared_sample_rates.size(); a++){
        for(int b = a + 1; b < shared_sample_rates.size(); b++){
            if(shared_sample_rates[a] == shared_sample_rates[b]){
                shared_sample_rates[b] = 0;
            }
        }
    }
    shared_sample_rates.erase(std::find(shared_sample_rates.begin(), shared_sample_rates.end(), 0));
    if(shared_sample_rates.size() == 0){
        cout << "Please restart the program using new input and output devices, there are no shared sample rates between devices." << endl;
    }
    for(int a = 0; a < shared_sample_rates.size(); a++){
        cout << "[" << a << "]  " << shared_sample_rates[a];
    }
    cout << "Enter the index of the desired sample rate from this list of shared sample rates between input and output devices:" << endl;
    std::cin >> sample_rate_index;
    settings.sampleRate = shared_sample_rates[sample_rate_index];
    */
    settings.sampleRate = sample_rate;

    for(unsigned int a = 0; a < buffer_sizes.size(); a++){
        cout << "[" << a << "]  " << buffer_sizes[a];
    }
    cout << "Enter the index of the desired buffer size (chosen buffer size must be compatible with your input and output device)" << endl;
    std::cin >> buffer_size_index;
    buffer_size = buffer_sizes[buffer_size_index];
    settings.bufferSize = buffer_sizes[buffer_size_index];
    in_frames = buffer_size * in_channels;
    out_frames = buffer_size * out_channels;
    last_in_buffer = std::make_unique<float[]>(in_frames);
    in_buffer = std::make_unique<float[]>(in_frames);

    previous_out = std::make_unique<float[]>(out_frames);
    /*
    cout << "Press any key for OSC settings, ENTER to begin the piece." << endl;
    //if (std::cin.get() == '\n'){
        //stream.setup(settings);
    //}
    //else{
        cout << "Enter the reciever's network address (press ENTER for default ""localhost""):" << endl;
        string host;
        if(std::cin.get() == '\n'){
            host = "localhost";
        }
        else{
            std::cin >> host;
        }
        cout << "Enter the reciever's port number:" << endl;
        int port;
        std::cin >> port;
        receiver.setup(host, port);
        osc_sender_loop:
        while(true){
            cout << "Press any key to add a new node, ENTER to begin the piece." << endl;
            if (std::cin.get() == '\n'){
                stream.setup(settings);
            }
            else{
                message_destination next_message_destination;
                cout << "Enter the network address of the node:" << endl;
                std::cin >> next_message_destination.message_address;
                cout << "Enter the port of the node:" << endl;
                std::cin >> next_message_destination.message_port;
                goto osc_sender_loop;
            }
        }
    //}
    *//*
    cout << "Enter the receiver's IP address:" << endl;
    string receiver_ip;
    std::cin >> receiver_ip;
    cout << "Enter the receiver's port number:" << endl;
    int receiver_port;
    std::cin >> receiver_port;
    receiver.setup(receiver_ip, receiver_port);
    cout << "Enter the node's IP address:" << endl;
    string node_ip;
    std::cin >> node_ip;
    cout << "Enter the node's port number:" << endl;
    int node_port;
    std::cin >> node_port;
    sender.setup(node_ip, node_port);
    */
    stream.setup(settings);
}

float ofApp::mod_quotient(float in, float mod){
    return fmod(in, mod) / mod;
}

void ofApp::analysis(float sample, float &dc, float &amplitude_root, float &amplitude, bool &cross, 
        float &cross_amplitude_root, float &cross_amplitude, float &cross_count, float &cross_time, float &pitch){
    dc += sample;
    float dc_adjustment = dc / sample_count;
    float amplitude_root_increment = sqrt(abs(sample - dc_adjustment)) * (1.0 - abs(dc_adjustment));
    amplitude_root += amplitude_root_increment;
    amplitude = pow(amplitude_root / sample_count, 2.0);
    cross_amplitude_root += amplitude_root_increment;
    cross_amplitude = pow(cross_amplitude_root / sample_count, 2.0);
    bool crossed = false;
        if(cross){
            if(sample < dc_adjustment){
                crossed = true;
                cross = false;
            }
        }
        else{
            if(sample > dc_adjustment){
                crossed = true;
                cross = true;
            }
        }
        if(crossed){
            cross_count += 1.0;
            cross_time = sample_count / cross_count;
            pitch = mix(pitch, cross_time, cross_amplitude / (amplitude * cross_count + min_float));
        }
}

void ofApp::audioIn(ofSoundBuffer &buffer){
    //cout << in_pitch[0] << endl;
    for(unsigned int a = 0; a < buffer.getNumFrames(); a++){
        sample_count += 1.0;
        //revisit incrementation of progress, which will correlate with mix of output z0 samples (z1/z2 will be orthogonal function)
        //progress should be incremented based on % of maximum change per channel per parameter times epsilon
        //this may require a pow function to scale

        for(unsigned int b = 0; b < in_channels; b++){
            analysis(last_in_buffer[b], in_dc[b], in_amplitude_root[b], in_amplitude[b], 
                in_cross[b], in_cross_amplitude_root[b], in_cross_amplitude[b], in_cross_count[b], in_cross_time[b], in_pitch[b]);
        /*
        float in_sample = last_in_buffer[a * in_channels + b];
        float current_dc = in_dc[b];
        in_dc[b] = current_dc + in_sample;
        float dc_adjustment = in_dc[b] / sample_count;
        float current_roots = in_amplitude_roots[b];
        amplitude_roots[b] = current_roots + sqrt(abs(in_sample - dc_adjustment) * (1.0 - abs(dc_adjustment)));
        amplitude[b] = pow(amplitude_roots[b] / sample_count, 2.0);
        bool crossed = false;
        if(cross[b]){
            if(in_sample < dc_adjustment){
                crossed = true;
                cross[b] = false;
            }
        }
        else{
            if(in_sample > dc_adjustment){
                crossed = true;
                cross[b] = true;
            }
        }
        if(crossed){
            cross_count += 1.0;
            pitch = sample_count / cross_count;
            //pitch = fmod(pitch, (float)sample_rate);
            //update analysis
            cross_sample_count = 1.0;
        }
            */
        int index = a * in_channels + b;
        last_in_buffer[index] = in_buffer[index];
        in_buffer[index] = buffer[a * in_channels + b];
        }
    }
}

float ofApp::mix(float inA, float inB, float mix){
    return (1.0 - mix) * inA + (inB * mix);
}

void ofApp::audioOut(ofSoundBuffer &buffer){
    
        for(int a = 0; a < buffer.getNumFrames(); a++){
            float ring_pitch = 1.0;
            for(int b = 0; b < in_channels; b++){
                ring_pitch += in_pitch[a];
            }
            ring_pitch /= in_channels;
            //cout << ring_pitch << endl;
            for(int b = 0; b < out_channels; b++){
                //to determine how much each output channel corresponds to an input, subtract the input parameter one by one from the output parameters (0-1)
                //multiply each by the target parameters and divide by the sum of the differences for an average
                //ratio of local to OSC-derived influence should essentially be a byproduct of the rate of change
                //take a measurement of how different per-channel values are to determine contrast for outputs (this can be a simple abs - 0.5 derived value)
                //perhaps rather than "filtering" parametric changes, they can just change more the more progress goes!

                /*
                //float phase_increment = 1.0 - (0.5 * abs(in_sample - previous_out[b]));
                float phase_increment = 0.0125;
                phase[b] += phase_increment;
                //float sample = sin(phase[b]) * (1.0 - (0.5 * abs(z0[b] - previous_out[b])));
                float sample = goetzel(32.0, z0[b], z1[b], z2[b]);
                */
                //phase_increment[b] = 0.1 / (pitch + min_float);
                /*
                modulator_phase[b] += 0.01;
                modulator_phase[b] = fmod(modulator_phase[b], std::numeric_limits<float>::max());
                index[b] = pow(progress * (1.0 - phase_increment[b]) * (float)sample_rate, 0.5);
                float modulator_amplitude = sin(modulator_phase[b]);
                phase_increment[b] = 1.0 / (in_pitch[b] + 99);
                //carrier_phase[b] += phase_increment[b] + phase_increment[b] * modulator_amplitude * index[b];
                carrier_phase[b] += phase_increment[b];
                carrier_phase[b] = fmod(carrier_phase[b], 1.0);
                //carrier_phase[b] += phase_increment[b];
                //float sin_amplitude = sin(carrier_phase[b]) * pow(1.0 - phase_increment[b], 2.0);
                
                float last_in_sample = last_in_buffer[index];
                float in_sample = in_buffer[index];
                float last_sin_amplitude = sin_amplitude[b];
                sin_amplitude[b] = sin(carrier_phase[b]);
                //sin_amplitude[b] = mix(last_sin_amplitude, sin_amplitude[b], 0.9);
                //sin_amplitude[b] = sin(last_in_sample * HALF_PI * sin_amplitude[b] / (in_amplitude[b] + min_float));
                //sin_amplitude[b] = mix(last_sin_amplitude, sin(HALF_PI * sin(carrier_phase[b]) * (1.0 / (amplitude[b] + min_float))), 0.1);
                //buffer[index] = glm::mix(sin_amplitude, last_in_sample * sin_amplitude[b], pow(amplitude[b], progress));
                //float out_sample = ofClamp(last_in_sample * pow(1.0 / (in_amplitude[b] + min_float), 0.25), -1.0, 1.0);
                //float out_sample = mix(sin_amplitude[b], previous_out[b], pow(0.5 * abs(sin_amplitude[b] - previous_out[b]), 0.05));
                */
                phase_increment[b] = 1.0 / (in_pitch[b] + 9);
                float out_sample = sin(carrier_phase[b]);
                int index = a * out_channels + b;
                buffer[index] = out_sample;
                z2[b] = z1[b];
                z1[b] = out_sample;
                //analysis(out_sample, out_dc[b], out_amplitude_root[b], out_amplitude[b], 
                    //out_cross[b], out_cross_amplitude_root[b], out_cross_amplitude[b], out_cross_count[b], out_cross_time[b], out_pitch[b]);
            }
        }       
    }      

//--------------------------------------------------------------
void ofApp::update(){
    //rate of sent OSC messages should be based on percentage of local change to possible maximum
    //update average of all "external" parameters every update cycle
    //this should literally be done with no discernement of the message address, as frequency should correlate directly with weighting (just keep a count of recieved messages)
    /*
    ofxOscMessage m;
    m.setAddress( "/test" );
	m.addIntArg( 1 );
	m.addFloatArg( 3.5f );
	m.addStringArg( "hello" );
	m.addFloatArg( ofGetElapsedTimef() );
	sender.sendMessage( m );
    if(receiver.hasWaitingMessages()){
        amp = 0.5;
    }
        */

}

//--------------------------------------------------------------
void ofApp::exit(){
    ofSoundStreamClose();
}