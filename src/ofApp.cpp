#include "ofApp.h"
#include "ofSoundBaseTypes.h"
#include <cstdint>
#include <ofxOsc.h>

//--------------------------------------------------------------

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings &settings){

}

void ofApp::setup(){
    for(int a = 0; a < test_fir_kernel.size(); a++){
        test_fir_kernel[a] = 1.0;
        //test_fir_kernel[a] = ofRandomf();
    }
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
    input_buffer = std::make_unique<float[]>(in_frames);
    padded_input_buffer = std::make_unique<float[]>(in_frames + test_fir_kernel.size());
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

void ofApp::audioIn(ofSoundBuffer &buffer){
    for(unsigned int a = 0; a < buffer.getNumFrames(); a++){
        for(unsigned int b = 0; b < in_channels; b++){
        padded_input_buffer[a * in_channels + b] = buffer[a * in_channels + b];
        /*
        if(a < test_input_array.size()){
            test_input_array[a] = input_buffer[a];
        }
            */
        //inputBuffer[a * in_channels + b] = buffer[a * in_channels + b];
        }
    }
    //input_buffer -= buffer.getNumFrames();
}

void ofApp::audioOut(ofSoundBuffer &buffer){
    //cout << pointerValue << endl;
        for(unsigned int a = 0; a < buffer.getNumFrames(); a++){
            for(unsigned int b = 0; b < out_channels; b++){
                sample = std::inner_product(test_fir_kernel.begin(), test_fir_kernel.end(), &padded_input_buffer[a * in_channels + b], 0) / (float)test_fir_kernel.size();
                //pointer++;
                //buffer[a * out_channels + b] = *input_buffer;
                //input_buffer++;
                //sample = input_buffer[a * in_channels + b];
                buffer[a * out_channels + b] = sample;
                lastSample = lastSample + sample;
            }
        }
        lastSample = lastSample / (float)buffer.getNumFrames();
        
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