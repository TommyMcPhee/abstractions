#include "ofApp.h"
#include "ofSoundBaseTypes.h"
#include <cstdint>
#include <algorithm>
#include <ofxOsc.h>

//--------------------------------------------------------------

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings &settings){

}

void ofApp::setup(){
    cout << "Welcome to Abstractions!" << endl;
    settings.setOutListener(this);
    settings.setInListener(this);
    unsigned int in_device_index, proposed_in_channels, out_device_index, proposed_out_channels, sample_rate_index, buffer_size_index;
    ofSoundDevice in_device, out_device;
    cout << stream.getDeviceList() << endl;
    cout << "Enter index of input device:" << endl;
    std::cin >> in_device_index;
    in_device = stream.getDeviceList()[in_device_index];
    settings.setInDevice(in_device);
    cout << "Enter number of input channels:" << endl;
    in_channels_loop:
    std::cin >> proposed_in_channels;
    if(proposed_in_channels > 0 && proposed_in_channels <= in_device.inputChannels){
        in_channels = proposed_in_channels;
        settings.numInputChannels = in_channels;
    }
    else{
        cout << "Please enter a valid number of input channels:" << endl;
        goto in_channels_loop;
    }
    cout << "Enter index of output device:" << endl;
    std::cin >> out_device_index;
    out_device = stream.getDeviceList()[out_device_index];
    settings.setOutDevice(stream.getDeviceList()[out_device_index]);
    cout << "Enter number of output channels:" << endl;
    out_channels_loop:
    std::cin >> proposed_out_channels;
    if(proposed_out_channels > 0 && proposed_out_channels <= out_device.outputChannels){
        out_channels = proposed_out_channels;
        settings.numOutputChannels = out_channels;
        phase = std::make_unique<float[]>(out_channels);
        //fix channels
    }
    else{
        cout << "Please enter a valid number of output channels:" << endl;
        goto out_channels_loop;
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

    for(int a = 0; a < buffer_sizes.size(); a++){
        cout << "[" << a << "]  " << buffer_sizes[a];
    }
    cout << "Enter the index of the desired buffer size (chosen buffer size must be compatible with your input and output device)" << endl;
    std::cin >> buffer_size_index;
    buffer_size = buffer_sizes[buffer_size_index];
    settings.bufferSize = buffer_sizes[buffer_size_index];
    in_frames = buffer_size * in_channels;
    filter_frames = 3 * in_channels;
    cout << filter_frames << endl;
    out_frames = buffer_size * out_channels;
    in_buffer = std::make_unique<float[]>(in_frames);
    filter_buffer = std::make_unique<float[]>(filter_frames);
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

float ofApp::goetzel(float samples, float z0, float z1, float z2){
    float constant = 2.0 * cos(TWO_PI / samples);
    return (z0 + constant * z1 - z2) / 3.0;
}

void ofApp::audioIn(ofSoundBuffer &buffer){
    for(unsigned int a = 0; a < buffer.getNumFrames(); a++){
        for(unsigned int b = 0; b < in_channels; b++){ 
        //in_buffer[a * in_channels + b] = buffer[a * in_channels + b];
        for(unsigned int c = filter_frames - (in_channels - b); c >= in_channels; c -= in_channels){
            filter_buffer[c] = filter_buffer[c - 1];
        }
        filter_buffer[b] = goetzel(1.0 / (buffer[a * in_channels + b] + std::numeric_limits<float>::min()), buffer[a * in_channels + b], filter_buffer[in_channels + b], filter_buffer[in_channels * 2 + b]);
        in_buffer[a * in_channels + b] = filter_buffer[b];
        /*
        if(a < test_input_array.size()){
            test_input_array[a] = input_buffer[a];
        }
            */
        //inputBuffer[a * in_channels + b] = buffer[a * in_channels + b];
        }
    }
}

void ofApp::audioOut(ofSoundBuffer &buffer){
        for(unsigned int a = 0; a < buffer.getNumFrames(); a++){
            sample_count++;
            for(unsigned int b = 0; b < out_channels; b++){
                float sample = ofRandomf() * in_buffer[a * in_channels+ b];
                /*
                //float phase_increment = 1.0 - (0.5 * abs(in_sample - previous_out[b]));
                float phase_increment = 0.0125;
                phase[b] += phase_increment;
                //float sample = sin(phase[b]) * (1.0 - (0.5 * abs(z0[b] - previous_out[b])));
                float sample = goetzel(32.0, z0[b], z1[b], z2[b]);
                */
                buffer[a * out_channels + b] = sample;
                //fix
                /*
                for(unsigned int c = out_frames - 1; c > 0; c--){
                    previous_out[c] = previous_out[c - 1];
                };
                previous_out[b] = sample;
                */
            }
        }       
    }      

//--------------------------------------------------------------
void ofApp::update(){
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