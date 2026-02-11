#include "ofApp.h"
#include "ofSoundBaseTypes.h"
#include <cmath>
#include <ofxOsc.h>

void ofApp::cin_refresh(){
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void ofApp::osc_setup_warning(){
    cout << "Please enter a string-castable IP address followed by an integer port, seperated by a space." << endl;
    cin_refresh();
}

void ofApp::receiver_setup(){
        cout << "Enter the receiver's IP address and port number (in that order, seperated by a space):" << endl;
        string receiver_ip;
        int receiver_port;
        
        if(std::cin >> receiver_ip >> receiver_port){
            receiver.setup(receiver_ip, receiver_port);
        }
        else{
            osc_setup_warning();
        }

}

void ofApp::add_sender(){
    string sender_ip;
    int sender_port;
    cout << "Enter the sender's IP address and port number (in that order, seperated by a space):" << endl;
    
    if(std::cin >> sender_ip >> sender_port){
        ofxOscSender new_sender;
        new_sender.setup(sender_ip, sender_port);
        senders.push_back(new_sender);
    }
    else{
        osc_setup_warning();
    }

}

void ofApp::print_array_value(int index, int value){
    cout << "[" << index << "]  " << value << endl;
}

void ofApp::unsigned_integer_warning(){
    cout << "Please enter an unsigned integer." << endl;
    cin_refresh();
}

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings &settings){

}

void ofApp::setup(){
    epsilon_float = std::numeric_limits<float>::epsilon();
    min_float = std::numeric_limits<float>::denorm_min();

    cout << "Welcome to Abstractions!" << "\n" << endl;
    cout << "Enter any character(s) for detailed information; Press ENTER without input to proceed with setup." << endl;
    char user_input;
    
    help_loop:
    bool help = false;
    
    while(std::cin.get(user_input) && user_input != '\n'){
        help = true;
    }
    
    //update help info

    if(help){
        cout << "\n" << "Here is some general abstractions architecture and setup information; consult the README and source code for more detail." << endl;
        cout << "\n" << endl;
        cout << "Each node (instance) of Abstractions can include one audio output device, one audio input device, or one device for each." << endl;
        cout << "This node-based architecture allows for cross-platform use of multiple audio devices locally or on a local network." << endl;
        cout << "Setup will consist of establishing output and input devices, along with OSC IP addresses and ports for this node." << endl;
        cout << "Regardless of whether this node is responsible for audio input and/or output, a device will be *selected* for each function." << endl;
        cout << "Functionally, each node of Abstractions must have either input or output channels to contribute (hence, a sound device is required)." << endl;
        cout << "Should a user decide to \"hack\" or expand upon the piece with externally-generated OSC messages, consult the source code for usage." << endl;
        cout << "\n" << endl;
        cout << "To nullify output or input, simply select a compatible sound device with 0 available respective channels for that function." << endl;
        cout << "After this information is provided, you may manually adjust sample rates (pending API/device compatibility) and buffer sizes." << endl;
        cout << "If at any point, a setup error is made, simply restart the program (this is to keep the program streamlined and lightweight)." << endl;
        cout << "Furthermore, the program only scanns audio devices once during setup. To add a newly available device, you must restart the program." << endl;
        cout << "\n" << endl;
        cout << "A single OSC receiver IP address and port is required unless a node is input-only and responsible for starting the piece." << endl;
        cout << "If input channels are > 0, you will be required to specify at least one IP address and port for OSC communication." << endl;
        cout << "You may specify any number of IP addresses and ports for sending input data; receiving data is limited to a single address and port." << endl;
        cout << "If input functionality is not present, send addresses may optionally be specified exclusively to send the start signal." << endl;;
        cout << "In specifying send address/port destinations, one may send to all receivers or only some, at thier discretion." << endl;
        cout << "Besides sending and receiving the START signal, OSC data is only sent from audio input nodes and received by audio output nodes." << endl;
        cout << "\n" << endl;
        cout << "Abstractions ends when a recursive system within each output channel is allowed to zero; there is no OSC-synchronized ending." << endl;
        cout << "If every device is OSC-connected to each other directly, Abstractions should theoretically have a loosely synchronous ending." << endl;
        cout << "If some output nodes are recieving different OSC data than others, the endings of Abstractions will be asynchronous." << endl;
        cout << "Output-only nodes will attempt to self-terminate upon completion, all other nodes must be manually terminated after the piece." << endl;
        cout << "\n" << endl;
        cout << "For nodes with inputs and outputs to respond to thier own inputs, the use of a local IP address and port is required." << endl;
        cout << "This implementation is to deliberately democratize the I/O procedures and optimize worst-case efficiency as much as possible." << endl;
        cout << "For more information, consult the README, source code, and research paper on this piece and topic. Press ENTER to proceed with setup!" << endl;
        goto help_loop;
    }  
    
    unsigned int device_list_size, out_device_index, in_device_index, buffer_size_index, sample_rate_index;
    auto device_list = stream.getDeviceList();
    device_list_size = device_list.size();
    ofSoundDevice out_device, in_device;
    cout << device_list << endl;
    cout << "Enter index of output device:" << endl;
    
    while(true){
        
        if(std::cin >> out_device_index && out_device_index < device_list_size ){
            break;
        }
        else{
            cout << "Please enter a valid ouput device index (unsigned integer) within the listed range." << endl;
            cin_refresh();
        }

    }
    
    out_device = device_list[out_device_index];
    settings.setOutDevice(device_list[out_device_index]);
    out_channels = out_device.outputChannels;
    
    if(out_channels > 0){
        settings.numOutputChannels = out_channels;
        out_z2 = std::make_unique<float[]>(out_channels);
        out_z1 = std::make_unique<float[]>(out_channels);
        out_dc = std::make_unique<float[]>(out_channels);
        out_amplitude_root = std::make_unique<float[]>(out_channels);
        out_amplitude = std::make_unique<float[]>(out_channels);
        out_delta = std::make_unique<float[]>(out_channels);
        out_slope = std::make_unique<float[]>(out_channels);
        out_cross = std::make_unique<bool[]>(out_channels);
        out_cross_count = std::make_unique<float[]>(out_channels);
        out_pitch = std::make_unique<float[]>(out_channels);
        last_phase_increment = std::make_unique<float[]>(out_channels);
        phase_increment = std::make_unique<float[]>(out_channels);
        phase = std::make_unique<float[]>(out_channels);
        last_amplitude = std::make_unique<float[]>(out_channels);
        amplitude = std::make_unique<float[]>(out_channels);
<<<<<<< HEAD
=======
        z1 = std::make_unique<float[]>(out_channels);
        z2 = std::make_unique<float[]>(out_channels);
>>>>>>> 67c353aa6fb21794fd3ee1ea59157fdf5e55495b
        for(int a = 0; a < out_channels; a++){
            out_z1[a] = 0.0;
            out_dc[a] = 0.0;
            out_amplitude_root[a] = 0.0;
            out_delta[a] = 0.0;
            out_slope[a] = 0.0;
            out_cross[a] = false;
            out_cross_count[a] = 0.0;
            out_pitch[a] = 0.0;
            
            //last_phase_increment[a] = 0.0;
            phase_increment[a] = 0.0;
            //phase[a] = 0.0;
            //last_amplitude[a] = 0.0;
            amplitude[a] = 0.0;
        }
        receiver_setup();
    }
    else{
        output = false;
    }

    cout << '\n' << device_list << endl;
    cout << "Enter index of input device:" << endl;
    
    while(true){
        if(std::cin >> in_device_index && in_device_index < device_list_size ){
            break;
        }
        else{
            cout << "Please enter a valid input device index (unsigned integer) within the listed range." << endl;
            cin_refresh();
        }
    }
    
    in_device = device_list[in_device_index];
    settings.setInDevice(in_device);
    in_channels = in_device.inputChannels;
    in_channels_float = (float)in_channels;
    
    if(in_channels > 0){

        if(in_channels == 1){
            spread = false;
        }

        settings.numInputChannels = in_channels;
        in_dc = std::make_unique<float[]>(in_channels);
        in_amplitude_root = std::make_unique<float[]>(in_channels);
        in_cross = std::make_unique<bool[]>(in_channels);
        in_cross_count = std::make_unique<float[]>(in_channels);
        in_z1 = std::make_unique<float[]>(in_channels);
        in_z2 = std::make_unique<float[]>(in_channels);
        for(int a = 0; a < 4; a++){
            in_parameters[a] = std::make_unique<float[]>(in_channels);
            for(int b = 0; b < in_channels; b++){
                in_z1[b] = 0.0;
                in_dc[b] = 0.0;
                in_amplitude_root[b] = 0.0;
                in_cross[b] = false;
                in_cross_count[b] = 0.0;
                in_parameters[a][b] = 0.0;
            }
        }
    }
    else{
        input = false;
        additional_sender_loop:
        char user_inputb;
        cout << "Enter any character(s) to add an OSC destination; press ENTER without input to skip." << endl;
        while(true){
            std::cin.ignore();
            std::cin.get(user_inputb);
            if(user_inputb == '\n'){
                goto complete_setup;
            }
            else{
                add_sender();
                goto additional_sender_loop;
            }
        }
    }
    
    add_sender();
    goto additional_sender_loop;

    complete_setup:
    unsigned int buffer_sizes_size = buffer_sizes.size();
    for(unsigned int a = 0; a < buffer_sizes_size; a++){
        print_array_value(a, buffer_sizes[a]);
    }
    cout << "Enter the index of the desired buffer size (chosen buffer size must be compatible with your API and device(s)):" << endl;
    cout << "Enter any integer greater than or equal to " << buffer_sizes_size << " to use your current default buffer size." << endl;
    
    if(std::cin >> buffer_size_index){
        if(buffer_size_index < buffer_sizes_size){
            settings.bufferSize = buffer_sizes[buffer_size_index];
        }
    }
    else{
        unsigned_integer_warning();
    }
    
    unsigned int sample_rates_size = sample_rates.size();
    for(unsigned int a = 0; a < sample_rates_size; a++){
        print_array_value(a, sample_rates[a]);
    }
    cout << "Enter the index of the desired sample rate (chosen sample rate must be compatible with your API and device(s)):" << endl;
    cout << "Enter any integer greater than or equal to " << sample_rates_size << " to use your current default sample rate." << endl;
    
    if(std::cin >> sample_rate_index){
        
        if(sample_rate_index < sample_rates_size){
            settings.sampleRate = sample_rates[sample_rate_index];
        }

    }
    else{
        unsigned_integer_warning();
    }

    if(!output){
        cout << "Since this node contains no outputs, you may use it to begin the piece without initializing an OSC receiver." << endl;
        cout << "To initialize an OSC receiver (required if starting the piece from another node), enter any character(s) before pressing ENTER." << endl;
        receiver_setup();
    }

    settings.setOutListener(this);
    settings.setInListener(this);
    stream.setup(settings);
}

void ofApp::samplewise_updates(){
    for(int a = 0; a < update_thread.size(); a++){    
        if(update_thread[a]){
            parameter_smoothing[a] = 1.0 / samples_per_update[a];
            samples_per_update[a] = 0.0;
            update_thread[a] = false;
        }
        samples_per_update[a] += 1.0;
    }
        
    sample_count += 1.0;
<<<<<<< HEAD
    reciprocal_sample_count = 1.0 / sample_count;
    //revisit
=======
    samples_per_update += 1.0;

>>>>>>> 67c353aa6fb21794fd3ee1ea59157fdf5e55495b
    progress = unipolar_sin(sample_count * epsilon_float);
}

void ofApp::analysis(float z2, float z1, float sample, float &dc, float &amplitude_root, float &amplitude,
    float &delta, float &slope, bool &cross, float &cross_count, float &pitch){  
    
    dc += sample;
    float dc_adjustment = dc / sample_count;
    float inverse_dc_magnitude = 1.0 - abs(dc_adjustment);
    float amplitude_root_increment = sqrt(abs(sample - dc_adjustment) * inverse_dc_magnitude);
    amplitude_root += amplitude_root_increment;
    amplitude = pow(amplitude_root / sample_count, 2.0);
    if(amplitude != 0.0){
        delta = mix(delta, 0.5 * abs(z1 - sample) / amplitude, reciprocal_sample_count);
    }
    if(z1 != 0.0){
        slope = mix(slope, acos(z2 + sample / (2.0 * z1)) / TWO_PI, reciprocal_sample_count);
    }
    float threshold = amplitude * inverse_dc_magnitude;
    bool crossed = false;
        
        if(cross){
            
<<<<<<< HEAD
            if(sample < dc_adjustment - threshold){
=======
            if(sample < dc_adjustment - (amplitude * inverse_dc_magnitude)){
            //if(sample < 0.0){
>>>>>>> 67c353aa6fb21794fd3ee1ea59157fdf5e55495b
                crossed = true;
            }

        }
        else{
            
<<<<<<< HEAD
            if(sample > dc_adjustment + threshold){
=======
            if(sample > dc_adjustment + (amplitude * inverse_dc_magnitude)){
            //if(sample > 0.0){
>>>>>>> 67c353aa6fb21794fd3ee1ea59157fdf5e55495b
                crossed = true;
            }

        }
        
        if(crossed){
            cross = !cross;
            cross_count += 1.0;
            pitch = cross_count / sample_count;
            //cout << pitch << endl;
        }

}
/*
void ofApp::process_input(){
    float spread_in = 0.0;
    //revisit
    float delta_spread = 0.25;
    if(spread){
        float average_squared = 0.0;
        for(int a = 0; a < in_channels; a++){
            average_squared = 0.0;
        }
    }
}
*/

void ofApp::audioIn(ofSoundBuffer &buffer){
    for(int a = 0; a < buffer.getNumFrames(); a++){
        samplewise_updates();
<<<<<<< HEAD
        std::array<float, 4> total_in, last_average_in, change_average;
        for(int b = 0; b < 4; b++){
            total_in[b] = 0.0;
            last_average_in[b] = average_in[b];
        }
        for(int b = 0; b < in_channels; b++){
            analysis(in_z2[b], in_z1[b],buffer[a * in_channels + b], in_dc[b], in_amplitude_root[b], in_parameters[b][0],
                in_parameters[b][1], in_parameters[b][2], in_cross[b], in_cross_count[b], in_parameters[b][3]);
            for(int c = 0; c < 2; c++){
                total_in[c] += in_parameters[b][c];
                total_in[c + 2] += in_parameters[b][c] * in_parameters[b][0];
            }
        }
        for(int b = 0; b < 2; b++){
            average_in[b] = total_in[b] / in_channels;
            average_in[b + 2] = total_in[b + 2] / total_in[0];
        }
        for(int b = 0; b < 4; b++){
            change_average[b] = abs(last_average_in[b] - average_in[b]);
        }
=======
        float total_in_amplitude = 0.0;
        float total_in_pitch = 0.0;      
        for(unsigned int b = 0; b < in_channels; b++){
            analysis(buffer[a * in_channels + b], in_dc[b], in_amplitude_root[b], in_amplitude[b], 
                in_cross[b], in_cross_count[b], in_pitch[b]);
            total_in_amplitude += in_amplitude[b];
            total_in_pitch += in_pitch[b] * in_amplitude[b];
        }
        average_in_amplitude = total_in_amplitude / in_channels_float;
        average_in_pitch = total_in_pitch / total_in_amplitude;
        float spread_in_amplitude = 0.0;
        float spread_in_pitch = 0.0;
        float delta_spread_amplitude = 0.25;
        float delta_spread_pitch = 0.25;
>>>>>>> 67c353aa6fb21794fd3ee1ea59157fdf5e55495b
        
        if(spread){
            std::array<float, 4> average_in_squared, last_spread_in, change_spread;
            for(int b = 0; b < 4; b++){
                average_in_squared[b] = 0.0;
                for(int c = 0; c < in_channels; c++){
                    average_in_squared[b] += pow(in_parameters[c][b], 2.0);
                }
                average_in_squared[b] /= in_channels_float;
                last_spread_in[b] = spread_in[b];
                spread_in[b] = sqrt(abs(average_in[b] - average_in_squared[b]));
                change_spread[b] = abs(last_spread_in[b] - spread_in[b]);
            }
        }
<<<<<<< HEAD

        for(int b = 0; b < 4; b++){
            update_in[b] = update_in[b] + 0.00001;
        }
=======
        float update_rate = sqrt(sample_count);
        float current_amplitude_update = amplitude_update;
        amplitude_update = current_amplitude_update + epsilon_float + (abs(average_in_amplitude - last_average_in_amplitude) * update_rate);
        //amplitude_update = current_amplitude_update + 0.1;
        //amplitude_update = current_amplitude_update + pow(abs(average_in_amplitude - last_average_in_amplitude) * delta_spread_amplitude, update_power);
        last_average_in_amplitude = average_in_amplitude;
        float current_pitch_update = pitch_update;
        pitch_update = current_pitch_update + 0.00001; 
        //pitch_update = current_pitch_update + epsilon_float + (abs(average_in_pitch - last_average_in_pitch) * update_rate);
        //pitch_update = current_pitch_update + pow(abs(average_in_pitch - last_average_in_pitch), average_in_pitch) * update_rate;
        last_average_in_pitch = average_in_pitch;
>>>>>>> 67c353aa6fb21794fd3ee1ea59157fdf5e55495b
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
        ring_return = 1.0;
        //playback = false;
    }
    
    return ring_return;
}

float ofApp::calculate_value(float last_value, float average_in, float parameter_smoothing, float out, float spread_in){
    //return mix(last_value, mix(mix(average_in, out, spread_in), average_in * out, ring), 0.95);
<<<<<<< HEAD
    return mix(mix(last_value, average_in, parameter_smoothing), out, spread_in);
=======
    return mix(mix(last_value, average_in, parameter_smoothing * (1.0 - ring)), out, spread_in);
>>>>>>> 67c353aa6fb21794fd3ee1ea59157fdf5e55495b
}

void ofApp::audioOut(ofSoundBuffer &buffer){
    if(playback){
        for(int a = 0; a < buffer.getNumFrames(); a++){;
            
            if(!input){
                samplewise_updates();
            }
            
            for(int b = 0; b < out_channels; b++){
                last_phase_increment[b] = phase_increment[b];
                phase_increment[b] = calculate_value(last_phase_increment[b], average_pitch, parameter_smoothing[3], out_pitch[b], spread_pitch);
                phase[b] += phase_increment[b];
                phase[b] = fmod(phase[b], 1.0);
                last_amplitude[b] = amplitude[b];
<<<<<<< HEAD
                amplitude[b] = calculate_value(last_amplitude[b], average_amplitude, parameter_smoothing[0], out_amplitude[b], spread_amplitude);
                float new_sample = sin(phase[b]) * pow(1.0 - amplitude[b], 2.0); 
                float out_sample = ((2.0 * progress * cos(TWO_PI * phase_increment[b]) * out_z1[b]) - (out_z2[b] * pow(progress, 2.0)) + new_sample * (1.0 - progress));
                int index = a * out_channels + b;
                buffer[index] = out_sample;
                out_z2[b] = out_z1[b];
                out_z1[b] = out_sample;
                analysis(out_z2[b], out_z1[b], out_sample, out_dc[b], out_amplitude_root[b], out_amplitude[b], out_delta[b], out_slope[b], 
=======
                amplitude[b] = calculate_value(last_amplitude[b], average_amplitude, out_amplitude[b], spread_amplitude, calculate_ring(amplitude_progress));
                //float new_sample = sin(phase[b]) * pow(1.0 - amplitude[b], 2.0); 
                float new_sample = ofRandomf();
                float out_sample = ((2.0 * progress * cos(TWO_PI * phase_increment[b]) * z1[b]) - (z2[b] * pow(progress, 2.0)) + new_sample * (1.0 - progress));
                int index = a * out_channels + b;
                buffer[index] = out_sample;
                z2[b] = z1[b];
                z1[b] = out_sample;
                analysis(out_sample, out_dc[b], out_amplitude_root[b], out_amplitude[b], 
>>>>>>> 67c353aa6fb21794fd3ee1ea59157fdf5e55495b
                    out_cross[b], out_cross_count[b], out_pitch[b]);
            }
        }
    }       
}

float ofApp::progress_increment(float last_average, float average, float last_spread, float spread){
    return pow(abs(last_average - average), 1.0 - abs(last_spread - spread)) * sample_count * epsilon_float;
}
/*
void ofApp::receive_message(ofxOscMessage message, float &average, float &spread, float &progress, float &last_average, float &last_spread){
    average = message.getArgAsFloat(0);
    spread = message.getArgAsFloat(1);
    progress += pow(abs(last_average - average), 1.0 - abs(last_spread - spread)) * samples_per_update * epsilon_float;
    last_average = average;
    last_spread = spread;
}
*/

//--------------------------------------------------------------
void ofApp::update(){
    for(int a = 0; a < 4; a++){
        
        if(update_in[a] > 1.0){
            ofxOscMessage message;
            message.setAddress(addresses[a]);
            message.addFloatArg(average_in[a]);
            message.addFloatArg(spread_in[a]);
            for(int a = 0; a < senders.size(); a++){
                senders[a].sendMessage(message);
                update_in[a] -= 1.0;
            }
        }

    }
    
    if(output){
        
        if(receiver.hasWaitingMessages()){

            //1. REVISIT FUNCTION STRUCTURE
            //2. Consider a structure in which "last" is always set first throughout the program. Slightly cleaner and more efficient.

            ofxOscMessage received_message;
            receiver.getNextMessage( received_message);
            string address = received_message.getAddress();
            if(address == "/amplitude"){
                update_thread[0] = true;
                //receive_message(received_message, average_amplitude, spread_amplitude, amplitude_progress, last_average_amplitude, last_spread_amplitude);
                average_amplitude = received_message.getArgAsFloat(0);
                spread_amplitude = received_message.getArgAsFloat(1);
                amplitude_progress += pow(progress_increment(last_average_amplitude, average_amplitude, last_spread_amplitude, spread_amplitude), 1.0 - amplitude_progress);
                last_average_amplitude = average_amplitude.load();
                last_spread_amplitude = spread_amplitude.load();
                cout << "amplitude" << average_amplitude << " " << spread_amplitude << " " << amplitude_progress << endl;
            }
            if(address == "/pitch"){
                update_thread[3] = true;
                average_pitch = received_message.getArgAsFloat(0);
                spread_pitch = received_message.getArgAsFloat(1);
                pitch_progress += pow(progress_increment(last_average_pitch, average_pitch, last_spread_pitch, spread_pitch), 1.0 - pitch_progress);
                last_average_pitch = average_pitch.load();
                last_spread_pitch = spread_pitch.load();
                cout << "pitch" << average_pitch << " " << spread_pitch << " " << pitch_progress << endl;
            }
        }

    }
}

void::ofApp::ofSoundStreamClose(){

}

//--------------------------------------------------------------
void ofApp::exit(){
    
}