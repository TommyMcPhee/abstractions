#include "ofApp.h"
#include "ofSoundBaseTypes.h"
#include <cmath>
#include <ofxOsc.h>
//--------------------------------------------------------------
void ofApp::towerOfHanoi(int n, char from_rod, char to_rod,
                  char aux_rod)
{
    if (n == 0) {
        return;
    }
    towerOfHanoi(n, from_rod, aux_rod, to_rod);
    /*
    cout << "Move disk " << n << " from rod " << from_rod
         << " to rod " << to_rod << endl;
         */
    hanoi[n][from_rod] = false;
    hanoi[n][to_rod] = true;
    towerOfHanoi(n, aux_rod, to_rod, from_rod);
}

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings &settings){

}

void ofApp::setup(){
    min_float = std::numeric_limits<float>::epsilon();
    progress = min_float;
    /*
    for(int a = 0; a < 23; a++){
        hanoi[a][0] = true;
        hanoi[a][1] = false;
        hanoi[a][2] = false;
    }
    towerOfHanoi(5, 'A', 'B', 'C');
    */
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
        dc = std::make_unique<float[]>(in_channels);
        amplitude_roots = std::make_unique<float[]>(in_channels);
        amplitude = std::make_unique<float[]>(in_channels);
        cross = std::make_unique<bool[]>(in_channels);
        //reevaluate
        sin_amplitude = std::make_unique<float[]>(in_channels);
        for(unsigned int a = 0; a < in_channels; a++){
            dc[a] = 0.0;
            amplitude_roots[a] = 0.0;
            amplitude[a] = 0.0;
            cross[a] = false;
        }
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
        phase_increment = std::make_unique<float[]>(out_channels);
        modulator_phase = std::make_unique<float[]>(out_channels);
        carrier_phase = std::make_unique<float[]>(out_channels);
        index = std::make_unique<float[]>(out_channels);
        for(int a = 0; a < out_channels; a++){
        phase_increment[a] = min_float * 3.0;
        }
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

    for(unsigned int a = 0; a < buffer_sizes.size(); a++){
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
    last_in_buffer = std::make_unique<float[]>(in_frames);
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
/*
float ofApp::goetzel(float samples, float z0, float z1, float z2){
    //fix if using
    float r_constant = 2.0 * cos(TWO_PI / samples);
    float i_constant = sin(TWO_PI / samples);
    return pow(0.5 * r_constant * z1 + z2, 2) + pow(i_constant * z1, 2);
}
*/

void ofApp::audioIn(ofSoundBuffer &buffer){
    //cout << amplitude[0] << endl;
    for(unsigned int a = 0; a < buffer.getNumFrames(); a++){
        sample_count += 1.0;
        cross_sample_count += 1.0;
        progress += min_float;
        for(unsigned int b = 0; b < in_channels; b++){ 
        float in_sample = buffer[a * in_channels + b];
        float current_dc = dc[b];
        dc[b] = current_dc + in_sample;
        float dc_adjustment = dc[b] / sample_count;
        float current_roots = amplitude_roots[b];
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
    //cout << cross_sample_count << endl;
        for(unsigned int a = 0; a < buffer.getNumFrames(); a++){
            float ring_sample = 1.0;
            for(unsigned int b = 0; b < in_channels; b++){
                ring_sample *= in_buffer[a * in_channels];
            }
            for(unsigned int b = 0; b < out_channels; b++){
                /*
                float phase_increment_copy = phase_increment[b];
                phase_increment[b] += pow(phase_increment_copy, 1.0 - phase_increment_copy);
                phase_increment[b] /= 1.71;
                //phase_increment[b] += min_float;
                //phase_increment[b] = fmod(phase_increment[b], 1.0);
                //float phase_increment = (float)(sample_count % sample_rate) / (float)sample_rate;
                */
                /*
                float in_sample = in_buffer[a * in_channels + b];
                              
                
                float min_term = MIN(abs(sin_amplitude), abs(in_sample));
                float max_term = MAX(abs(sin_amplitude), abs(in_sample));
                */
                /*
                //float phase_increment = 1.0 - (0.5 * abs(in_sample - previous_out[b]));
                float phase_increment = 0.0125;
                phase[b] += phase_increment;
                //float sample = sin(phase[b]) * (1.0 - (0.5 * abs(z0[b] - previous_out[b])));
                float sample = goetzel(32.0, z0[b], z1[b], z2[b]);
                */
                phase_increment[b] = 0.1 / (pitch + min_float);
                //phase_increment[b] = 0.003;
                modulator_phase[b] += phase_increment[b];
                modulator_phase[b] = fmod(modulator_phase[b], std::numeric_limits<float>::max());
                index[b] = pow(progress * (1.0 - phase_increment[b]) * (float)sample_rate, 0.5);
                float modulator_amplitude = sin(modulator_phase[b]);
                //carrier_phase[b] += phase_increment[b] + phase_increment[b] * modulator_amplitude * index[b];
                carrier_phase[b] += phase_increment[b];
                carrier_phase[b] = fmod(carrier_phase[b], 1.0);
                //carrier_phase[b] += phase_increment[b];
                //float sin_amplitude = sin(carrier_phase[b]) * pow(1.0 - phase_increment[b], 2.0);
                int index = a * out_channels + b;
                float last_in_sample = last_in_buffer[index];
                float in_sample = in_buffer[index];
                float last_sin_amplitude = sin_amplitude[b];
                sin_amplitude[b] = sin(carrier_phase[b]);
                //sin_amplitude[b] = mix(last_sin_amplitude, sin_amplitude[b], 0.9);
                sin_amplitude[b] = sin(last_in_sample * HALF_PI * sin_amplitude[b] / (amplitude[b] + min_float));
                //sin_amplitude[b] = mix(last_sin_amplitude, sin(HALF_PI * sin(carrier_phase[b]) * (1.0 / (amplitude[b] + min_float))), 0.1);
                //buffer[index] = glm::mix(sin_amplitude, last_in_sample * sin_amplitude[b], pow(amplitude[b], progress));
                float sample = ofClamp(last_in_sample * pow(1.0 / (amplitude[b] + min_float), 0.25), -1.0, 1.0);
                for(int d = 0; d < 1; d++){
                    //sample = glm::mix(sample * sin_amplitude, sin_amplitude, 0.5);
                    //sample *= sin_amplitude;
                }
                //float test_filter = sin_amplitude[b];
                //sample = sin(HALF_PI * test_filter * (1.0 / (amplitude[b] + min_float)));
                //sample = sin(HALF_PI * test_filter * 40.0);
                buffer[index] = sin_amplitude[b];
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