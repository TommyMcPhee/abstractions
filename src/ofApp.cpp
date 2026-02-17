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
        
        while(true){
            if(std::cin >> receiver_ip >> receiver_port){
                receiver.setup(receiver_ip, receiver_port);
                break;
            }
            else{
                osc_setup_warning();
            }
        }

}

void ofApp::add_sender(){
    string sender_ip;
    int sender_port;
    cout << "Enter the sender's IP address and port number (in that order, seperated by a space):" << endl;
    
    while(true){
        if(std::cin >> sender_ip >> sender_port){
            ofxOscSender new_sender;
            new_sender.setup(sender_ip, sender_port);
            senders.push_back(new_sender);
            break;
        }
        else{
            osc_setup_warning();
        }
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
    reverse_epsilon = 1.0 - epsilon_float;
    min_float = std::numeric_limits<float>::denorm_min();

    cout << "Welcome to Abstractions!" << "\n" << endl;
    cout << "Enter any character(s) for detailed information; Press ENTER without input to proceed with setup." << endl;
    char user_input;
    
    help_loop:
    bool help = false;
    
    while(std::cin.get(user_input) && user_input != '\n'){
        help = true;
    }

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
        cout << "A single OSC receiver IP address and port is required for nodes with > 0 output channels. Receiver ports cannot be shared between nodes." << endl;
        cout << "If input channels are > 0, you will be required to specify at least one IP address and port for OSC communication." << endl;
        cout << "You may specify any number of IP addresses and ports for sending input data; receiving data is limited to a single address and port." << endl;
        cout << "In specifying send address/port destinations, one may send to all receivers or only some, choosing thier desired network architecture." << endl;
        cout << "\n" << endl;
        cout << "Abstractions ends when a recursive system within each output channel is allowed to zero; there is no OSC-synchronized ending." << endl;
        cout << "If every device is OSC-connected to each other directly, Abstractions should theoretically have a loosely synchronous ending." << endl;
        cout << "If some output nodes are recieving different OSC data than others, the endings of Abstractions will be asynchronous." << endl;
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
    out_channels_float = (float)out_channels;
    
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

        modulator_phase = std::make_unique<float[]>(out_channels);
        last_pitch = std::make_unique<float[]>(out_channels);
        pitch = std::make_unique<float[]>(out_channels);
        phase = std::make_unique<float[]>(out_channels);
        last_amplitude = std::make_unique<float[]>(out_channels);
        amplitude = std::make_unique<float[]>(out_channels);
        last_delta = std::make_unique<float[]>(out_channels);
        delta = std::make_unique<float[]>(out_channels);
        last_slope = std::make_unique<float[]>(out_channels);
        slope = std::make_unique<float[]>(out_channels);
        for(int a = 0; a < out_channels; a++){
            out_z2[a] = 0.0;
            out_z1[a] = 0.0;
            out_dc[a] = 0.0;
            out_amplitude_root[a] = 0.0;
            out_delta[a] = 0.0;
            out_slope[a] = 0.0;
            out_cross[a] = false;
            out_cross_count[a] = 0.0;
            out_pitch[a] = 0.0;
            modulator_phase[a] = 0.0;
            pitch[a] = 0.0;
            phase[a] = 0.0;
            amplitude[a] = 0.0;
            delta[a] = 0.0;
            slope[a] = 0.0;
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
                in_z2[b] = 0.0;
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
    reciprocal_sample_count = 1.0 / sample_count;
}

float ofApp::calculate_delta(float previous, float current){
    return 0.5 * (previous - current);
}

void ofApp::average(float &value, float new_value){
    value = mix(value, new_value, reciprocal_sample_count);
}

void ofApp::analysis(float z2, float z1, float sample, float &dc, float &amplitude_root, float &amplitude,
    float &delta, float &slope, bool &cross, float &cross_count, float &pitch){  
    
    dc += sample;
    float dc_adjustment = dc / sample_count;
    float inverse_dc_magnitude = 1.0 - abs(dc_adjustment);
    float amplitude_root_increment = sqrt(abs(sample - dc_adjustment) * inverse_dc_magnitude);
    amplitude_root += amplitude_root_increment;
    amplitude = pow(amplitude_root / sample_count, 2.0);
    float delta0 = calculate_delta(z1, sample);
    average(delta, abs(delta0));
    average(slope, abs(calculate_delta(calculate_delta(z2, z1), delta0)));
    float threshold = amplitude * inverse_dc_magnitude;
    bool crossed = false;
        
        if(cross){
            
            if(sample < dc_adjustment - threshold){
                crossed = true;
            }

        }
        else{
            
            if(sample > dc_adjustment + threshold){
                crossed = true;
            }

        }
        
        if(crossed){
            cross = !cross;
            cross_count += 1.0;
            pitch = cross_count / sample_count;
        }

}

void ofApp::audioIn(ofSoundBuffer &buffer){
    for(int a = 0; a < buffer.getNumFrames(); a++){
        samplewise_updates();
        std::array<float, 4> total_in, last_average_in, change_average, change_spread;
        for(int b = 0; b < 4; b++){
            total_in[b] = 0.0;
            last_average_in[b] = average_in[b];
        }
        for(int b = 0; b < in_channels; b++){
            float in_sample = buffer[a * in_channels + b];
            analysis(in_z2[b], in_z1[b],in_sample, in_dc[b], in_amplitude_root[b], in_parameters[b][0],
                in_parameters[b][1], in_parameters[b][2], in_cross[b], in_cross_count[b], in_parameters[b][3]);
            in_z2[b] = in_z1[b];
            in_z1[b] = in_sample;
            for(int c = 0; c < 2; c++){
                total_in[c] += in_parameters[b][c];
                total_in[c + 2] += in_parameters[b][c + 2] * in_parameters[b][0];
            }
        }
        for(int b = 0; b < 2; b++){
            average_in[b] = total_in[b] / in_channels_float;
            average_in[b + 2] = total_in[b + 2] / (total_in[0] + min_float);
        }
        for(int b = 0; b < 4; b++){
            change_average[b] = abs(last_average_in[b] - average_in[b]);
            change_spread[b] = 0.0;
        }

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

        for(int b = 0; b < 4; b++){
            float current_update = update_in[b];
            float update_increment = sqrt(sample_count.load()) * pow(change_average[b], 1.0 - change_spread[b]);
            update_in[b] = current_update + update_increment;
        }
    }
}

void ofApp::add_difference(float &difference, float &current, float total){
    float signed_difference = current * out_channels_float - total;
    difference = abs(signed_difference);
    current = mix(current, signed_difference, difference);
}

float ofApp::mix(float inA, float inB, float mix){
    mix = ofClamp(mix, 0.0, 1.0);
    return (1.0 - mix) * inA + (inB * mix);
}

float ofApp::calculate_value(float last_value, float average_in, float parameter_smoothing, float out, float spread_in, float out_difference){
    return mix(mix(last_value, average_in, parameter_smoothing * (1.0 - progress)), out, pow(spread_in, out_difference));
    
    //return mix(mix(last_value, average_in, pow(parameter_smoothing, alteration) * (1.0 - progress)), out, spread_in * alteration);
    
    //return mix(mix(last_value, average_in, pow(parameter_smoothing, alteration) * (1.0 - progress)), out * alteration, spread_in);
    
    //return mix(mix(last_value, average_in, pow(parameter_smoothing, MAX(filter, sqrt(alteration)))), out * alteration, spread_in);
}

void ofApp::audioOut(ofSoundBuffer &buffer){
    for(int a = 0; a < buffer.getNumFrames(); a++){;
        
        if(!input){
            samplewise_updates();
        }

        progress_phase = ofClamp(sample_count * epsilon_float, 0.0, M_PI);
        progress = progress_phase / M_PI;
        filter = 1.0 - sin(progress_phase);

        float total_amplitude = 0.0;
        float total_delta = 0.0;
        float total_slope = 0.0;
        float total_pitch = 0.0;
        for(int b = 0; b < out_channels; b++){
            last_amplitude[b] = amplitude[b];
            amplitude[b] = calculate_value(last_amplitude[b], average_amplitude, parameter_smoothing[0], out_amplitude[b], spread_amplitude, amplitude_difference);
            total_amplitude += amplitude[b];
            last_delta[b] = delta[b];
            delta[b] = calculate_value(last_delta[b], average_delta, parameter_smoothing[1], out_delta[b], spread_delta, delta_difference);
            total_delta += delta[b];
            last_slope[b] = slope[b];
            slope[b] = calculate_value(last_slope[b], average_slope, parameter_smoothing[2], out_slope[b], spread_slope, slope_difference);
            total_slope += slope[b];
            last_pitch[b] = pitch[b];
            pitch[b] = calculate_value(last_pitch[b], average_pitch, parameter_smoothing[3], out_pitch[b], spread_pitch, pitch_difference);
            total_pitch += pitch[b];
        }
        for(int b = 0; b < out_channels; b++){
            add_difference(amplitude_difference, amplitude[b], total_amplitude);
            add_difference(delta_difference, delta[b], total_delta);
            add_difference(slope_difference, slope[b], total_slope);
            add_difference(pitch_difference, pitch[b], total_pitch);
        }
        for(int b = 0; b < out_channels; b++){
            modulator_phase[b] += slope[b];
            modulator_phase[b] = fmod(modulator_phase[b], 1.0);
            phase[b] += (1.0 - filter) * sin(modulator_phase[b]) * (1.0 - pitch[b]) * sqrt(delta[b]) + pitch[b];
            phase[b] = fmod(phase[b], 1.0);
            float amplitude_root = sqrt(amplitude[b]);
            float new_sample = sin(sin(phase[b]) * HALF_PI / (amplitude_root + min_float)) * amplitude_root;
            float resonance = 0.5 - (0.5 * calculate_delta(slope[b], delta[b]));
            float a2 = -1.0 * pow(resonance, 2.0);
            float a1 = 2.0 * resonance * cos(TWO_PI * pitch[b]);
            float out_sample = mix(new_sample, mix(a2, a1, MAX(0.5 - (0.5 * calculate_delta(delta[b], amplitude[b])), progress)), pow(filter, 3.0));
            buffer[a * out_channels + b] = out_sample;
            analysis(out_z2[b], out_z1[b], out_sample, out_dc[b], out_amplitude_root[b], out_amplitude[b], out_delta[b], out_slope[b], 
                out_cross[b], out_cross_count[b], out_pitch[b]);
            
            if(filter < 1.0){
                out_z2[b] = out_z1[b];
                out_z1[b] = out_sample;
            }
            else{
                out_z2[b] = out_z1[b] * reverse_epsilon;
                out_z1[b] = out_sample * reverse_epsilon;
            }

        }
    }      
}

void ofApp::update(){

    for(int a = 0; a < 4; a++){
        if(update_in[a] > 1.0){
            ofxOscMessage message;
            message.setAddress(addresses[a]);
            message.addFloatArg(average_in[a]);
            message.addFloatArg(spread_in[a]);
            for(int a = 0; a < senders.size(); a++){
                senders[a].sendMessage(message);;
            }
            update_in[a] -= 1.0;
        }

    }
    
    if(output){
        
        if(receiver.hasWaitingMessages()){
            ofxOscMessage received_message;
            receiver.getNextMessage( received_message);
            string address = received_message.getAddress();
            
            if(address == "/amplitude"){
                update_thread[0] = true;
                average_amplitude = received_message.getArgAsFloat(0);
                spread_amplitude = received_message.getArgAsFloat(1);

                cout << "amplitude" << " " << average_amplitude << " " << spread_amplitude << endl;
            }

            if(address == "/delta"){
                update_thread[1] = true;
                average_delta = received_message.getArgAsFloat(0);
                spread_delta = received_message.getArgAsFloat(1);

                cout << "delta" << " " << average_delta << " " << spread_delta << endl;
            }

            if(address == "/slope"){
                update_thread[2] = true;
                average_slope = received_message.getArgAsFloat(0);
                spread_slope = received_message.getArgAsFloat(1);

                cout << "slope" << " " << average_slope << " " << spread_slope << endl;
            }

            if(address == "/pitch"){
                update_thread[3] = true;
                average_pitch = received_message.getArgAsFloat(0);
                spread_pitch = received_message.getArgAsFloat(1);
                
                cout << "pitch" << " " << average_pitch << " " << spread_pitch << endl;
            }

        }

    }

}

void::ofApp::ofSoundStreamClose(){

}

void ofApp::exit(){
    
}