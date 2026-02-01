#include "ofApp.h"
#include "ofSoundBaseTypes.h"
#include <cmath>
#include <ofxOsc.h>
//--------------------------------------------------------------

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings &settings){

}

void ofApp::setup(){
    epsilon_float = std::numeric_limits<float>::epsilon();
    //min_float = std::numeric_limits<float>::denorm_min();

    cout << "Welcome to Abstractions!" << endl;
    //This is where you make the menu to set up the senders
    settings.setOutListener(this);
    settings.setInListener(this);
    unsigned int in_device_index, out_device_index, buffer_size_index;
    ofSoundDevice in_device, out_device;
    cout << stream.getDeviceList() << endl;
    cout << "Enter index of input device:" << endl;
    std::cin >> in_device_index;
    in_device = stream.getDeviceList()[in_device_index];
    settings.setInDevice(in_device);
    in_channels = in_device.inputChannels;
    if(in_channels > 0 && senders.size() > 0){
    settings.numInputChannels = in_channels;
    in_dc = std::make_unique<float[]>(in_channels);
    in_amplitude_root = std::make_unique<float[]>(in_channels);
    in_amplitude = std::make_unique<float[]>(in_channels);
    in_cross = std::make_unique<bool[]>(in_channels);
    in_cross_count = std::make_unique<float[]>(in_channels);
    in_pitch = std::make_unique<float[]>(in_channels);
    for(int a = 0; a < in_channels; a++){
        in_dc[a] = 0.0;
        in_amplitude_root[a] = 0.0;
        in_amplitude[a] = 0.0;
        in_cross[a] = false;
        in_cross_count[a] = 0.0;
        in_pitch[a] = 1.0;
    }
    }
    else{
        input = false;
    }
    cout << "Enter index of output device:" << endl;
    if(!std::cin >> out_device_index || out_device_index >= stream.getDeviceList().size() ){

    }
    std::cin >> out_device_index;
    out_device = stream.getDeviceList()[out_device_index];
    settings.setOutDevice(stream.getDeviceList()[out_device_index]);
    out_channels = out_device.outputChannels;
    if(out_channels > 1){
    settings.numOutputChannels = out_channels;
    out_dc = std::make_unique<float[]>(out_channels);
    out_amplitude_root = std::make_unique<float[]>(out_channels);
    out_amplitude = std::make_unique<float[]>(out_channels);
    out_cross = std::make_unique<bool[]>(out_channels);
    out_cross_count = std::make_unique<float[]>(out_channels);
    out_pitch = std::make_unique<float[]>(out_channels);
    last_phase_increment = std::make_unique<float[]>(out_channels);
    phase_increment = std::make_unique<float[]>(out_channels);
    phase = std::make_unique<float[]>(out_channels);
    last_amplitude = std::make_unique<float[]>(out_channels);
    amplitude = std::make_unique<float[]>(out_channels);
    /*
    z2 = std::make_unique<float[]>(out_channels);
    z1 = std::make_unique<float[]>(out_channels);
    */
    for(int a = 0; a < out_channels; a++){
        out_dc[a] = 0.0;
        out_amplitude_root[a] = 0.0;
        out_amplitude[a] = 0.0;
        out_cross[a] = false;
        out_cross_count[a] = 0.0;
        out_pitch[a] = 1.0;
        last_phase_increment[a] = 0.0;
        phase_increment[a] = 0.0;
        phase[a] = 0.0;
        last_amplitude[a] = 0.0;
        amplitude[a] = 0.0;

        /*
        z2[a] = 0.0;
        z1[a] = 0.0;
        */
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
    */
    cout << "Enter the receiver's IP address:" << endl;
    string receiver_ip;
    std::cin >> receiver_ip;
    cout << "Enter the receiver's port number:" << endl;
    int receiver_port;
    std::cin >> receiver_port;
    receiver.setup(receiver_ip, receiver_port);
    /*
    cout << "Enter the node's IP address:" << endl;
    string node_ip;
    std::cin >> node_ip;
    cout << "Enter the node's port number:" << endl;
    int node_port;
    std::cin >> node_port;
    sender.setup(node_ip, node_port);
    */
    //static_assert(std::atomic<float>::is_always_lock_free);
    }
    else{
        output = false;
    }
    stream.setup(settings);
}

void ofApp::samplewise_updates(){
    if(update_thread){
            parameter_smoothing = 1.0 / samples_per_update;
            samples_per_update = 0.0;
            update_thread = false;
        }
        sample_count += 1.0;
        samples_per_update += 1.0;
}

void ofApp::analysis(float sample, float &dc, float &amplitude_root, float &amplitude, bool &cross, float &cross_count, float &pitch){
    dc += sample;
    float dc_adjustment = dc / sample_count;
    float amplitude_root_increment = sqrt(abs(sample - dc_adjustment)) * (1.0 - abs(dc_adjustment));
    amplitude_root += amplitude_root_increment;
    amplitude = pow(amplitude_root / sample_count, 2.0);
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
            pitch = sample_count / cross_count;
        }
}

void ofApp::audioIn(ofSoundBuffer &buffer){
    
    for(unsigned int a = 0; a < buffer.getNumFrames(); a++){
        samplewise_updates();
        //revisit incrementation of progress, which will correlate with mix of output z0 samples (z1/z2 will be orthogonal function)
        //progress should be incremented based on % of maximum change per channel per parameter times epsilon
        //this may require a pow function to scale
        
        for(unsigned int b = 0; b < in_channels; b++){
            
        analysis(in_buffer[b], in_dc[b], in_amplitude_root[b], in_amplitude[b], 
            in_cross[b], in_cross_count[b], in_pitch[b]);

        int index = a * in_channels + b;
        //Delaying the input is unnecessary unless the input audio is directly referenced in synthesis
        //last_in_buffer[index] = in_buffer[index];
        in_buffer[index] = buffer[a * in_channels + b];
        }
    }
    
}

float ofApp::mix(float inA, float inB, float mix){
    mix = ofClamp(mix, 0.0, 1.0);
    return (1.0 - mix) * inA + (inB * mix);
}

float ofApp::unipolar_sin(float phase){
    return sin(phase) * 0.5 + 0.5;
}

float ofApp::calculate_ring(float progress){
    float ring_return = sin(HALF_PI * progress);
    if(progress > 1.0){
        ring_return = 0.0;
    }
    return ring_return;
}

float ofApp::calculate_value(float last_value, float average_in, float out, float spread_in, float ring){
    return mix(last_value, mix(mix(average_in, out, spread_in), average_in * out, ring), parameter_smoothing);
}

void ofApp::audioOut(ofSoundBuffer &buffer){
        //cout << progress << endl;
    for(int a = 0; a < buffer.getNumFrames(); a++){;
            if(!input){
                samplewise_updates();
            }
        for(int b = 0; b < out_channels; b++){
            last_phase_increment[b] = phase_increment[b];
            phase_increment[b] = calculate_value(last_phase_increment[b], average_pitch, out_pitch[b], spread_pitch, calculate_ring(pitch_progress));
            phase[b] += phase_increment[b];
            phase[b] = fmod(phase[b], 1.0);
            last_amplitude[b] = amplitude[b];
            amplitude[b] = calculate_value(last_amplitude[b], average_amplitude, out_amplitude[b], spread_amplitude, calculate_ring(amplitude_progress));
            float out_sample = sin(phase[b]) * amplitude[b]; 
            
            int index = a * out_channels + b;
            buffer[index] = out_sample;
            /*
            z2[b] = z1[b];
            z1[b] = out_sample;
            */
            analysis(out_sample, out_dc[b], out_amplitude_root[b], out_amplitude[b], 
                out_cross[b], out_cross_count[b], out_pitch[b]);
        }
    }       
}      

//--------------------------------------------------------------
void ofApp::update(){
    update_thread = true;
    update_count++;
    
    //this is really redundant

    if(input){
    float in_channels_float = (float)in_channels;
    float average_in_amplitude = 0.0;
    float average_in_pitch = 0.0;
    for(int a = 0; a < in_channels; a++){
        average_in_amplitude += in_amplitude[a];
        average_in_pitch += in_pitch[a];
    }
    average_in_amplitude /= in_channels_float;
    average_in_pitch /= in_channels_float;
    float spread_in_amplitude = 0.0;
    float spread_in_pitch = 0.0;
    float delta_spread_amplitude = 0.25;
    float delta_spread_pitch = 0.25;
    if(in_channels > 1){
        float average_in_amplitude_squared = 0.0;
        float average_in_pitch_squared = 0.0;
        for(int a = 0; a < in_channels; a++){
        average_in_amplitude_squared += pow(in_amplitude[a], 2.0);
        average_in_pitch_squared += pow(in_pitch[a], 2.0);
        }
        average_in_amplitude_squared /= in_channels_float;
        average_in_pitch_squared /= in_channels_float;
        spread_in_amplitude = sqrt(abs(average_in_amplitude - average_in_amplitude_squared));
        delta_spread_amplitude = abs(spread_in_amplitude - last_spread_in_amplitude);
        last_spread_in_amplitude = spread_in_amplitude;
        spread_in_pitch = sqrt(abs(average_in_pitch - average_in_pitch_squared));
        delta_spread_pitch = abs(spread_in_pitch - last_spread_in_pitch);
        last_spread_in_pitch = spread_in_pitch;
    }  
    
    amplitude_update += abs(average_in_amplitude - last_average_in_amplitude) * delta_spread_amplitude;
    last_average_in_amplitude = average_in_amplitude;
        if(amplitude_update > 1.0){
            ofxOscMessage amplitude_message;
            amplitude_message.setAddress("/pitch");
            amplitude_message.addFloatArg(average_in_amplitude);
            amplitude_message.addFloatArg(spread_in_amplitude);
            for(int a = 0; a < senders.size(); a++){
                senders[a].sendMessage(amplitude_message);
            }
            amplitude_update--;
        }
    
    pitch_update += abs(average_in_pitch - last_average_in_pitch) * delta_spread_pitch;
    last_average_in_pitch = average_in_pitch;
        if(pitch_update > 1.0){
            ofxOscMessage pitch_message;
            pitch_message.setAddress("/pitch");
            pitch_message.addFloatArg(average_in_pitch);
            pitch_message.addFloatArg(spread_in_pitch);
            for(int a = 0; a < senders.size(); a++){
                senders[a].sendMessage(pitch_message);
            }
            pitch_update--;
        }
    }
    if(output){
        if(receiver.hasWaitingMessages()){
            ofxOscMessage received_message;
		    receiver.getNextMessage( &received_message );
            string address = received_message.getAddress();
            if(address == "amplitude"){
                average_amplitude = received_message.getArgAsFloat(0);
                spread_amplitude = received_message.getArgAsFloat(1);
                amplitude_progress += epsilon_float / parameter_smoothing;
            }
            if(address == "pitch"){
                average_pitch = received_message.getArgAsFloat(0);
                spread_pitch = received_message.getArgAsFloat(1);
                pitch_progress += epsilon_float / parameter_smoothing;
            }
    }
}
    //progress
    
    
    //samples_per_update.store(0.0);
    //cout << samples_per_update << endl;
    //samples_per_update = 0.0;
    //cout << last_sample_count.load() << sample_count.load() << endl;
    //last_sample_count = sample_count.load();
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

    //the update thread is where all averaging calculations (even local) should be taken
    //if the device only has one channel, just send the value as the average and don't send a spread

    //find a way to do this which is agnostic to total networked devices (for both computational complexity and algorithmic interest)
        //furthermore, think about how this could be used to affect the form of the piece
}

//--------------------------------------------------------------
void ofApp::exit(){
    ofSoundStreamClose();
}