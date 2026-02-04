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

void ofApp::unsigned_integer_warning(){
    cout << "Please enter an unsigned integer." << endl;
    cin_refresh();
}

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings &settings){

}

void ofApp::setup(){
    epsilon_float = std::numeric_limits<float>::epsilon();
    //min_float = std::numeric_limits<float>::denorm_min();

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
    //this is where to set up senders

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
    complete_setup:
    //provide default options and validate inputs
    unsigned int buffer_sizes_size = buffer_sizes.size();
    for(unsigned int a = 0; a < buffer_sizes_size; a++){
        cout << "[" << a << "]  " << buffer_sizes[a] << "\n";
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
        cout << "[" << a << "]  " << sample_rates[a] << "\n";
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
    
    if(update_thread){
        parameter_smoothing = 1.0 / samples_per_update;
        samples_per_update = 0.0;
        update_thread = false;
    }
        
    sample_count += 1.0;
    samples_per_update += 1.0;
}

void ofApp::analysis(float sample, float &dc, float &amplitude_root, float &amplitude, bool &cross, float &cross_count, float &pitch){
    //ERROR: Function not reporting amplitude or pitch
    
    dc += sample;
    float dc_adjustment = dc / sample_count;
    float amplitude_root_increment = sqrt(abs(sample - dc_adjustment)) * (1.0 - abs(dc_adjustment));
    amplitude_root += amplitude_root_increment;
    amplitude = pow(amplitude_root / sample_count, 2.0);
    bool crossed = false;
        
        if(cross){
            
            if(sample < dc_adjustment){
                crossed = true;
            }

        }
        else{
            
            if(sample > dc_adjustment){
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
    //cout << average_in_amplitude << " " << average_in_pitch << endl;
    for(unsigned int a = 0; a < buffer.getNumFrames(); a++){
        samplewise_updates();
        float total_in_amplitude = 0.0;
        float total_in_pitch = 0.0;      
        for(unsigned int b = 0; b < in_channels; b++){
            analysis(buffer[a * in_channels + b], in_dc[b], in_amplitude_root[b], in_amplitude[b], 
                in_cross[b], in_cross_count[b], in_pitch[b]);
            total_in_amplitude += in_amplitude[b];
            total_in_pitch += in_pitch[b];
        }
        average_in_amplitude = total_in_amplitude / in_channels_float;
        average_in_pitch = total_in_pitch / in_channels_float;
        float spread_in_amplitude = 0.0;
        float spread_in_pitch = 0.0;
        float delta_spread_amplitude = 0.25;
        float delta_spread_pitch = 0.25;
        
        if(spread){
            // ERROR: Correct update incrementation fails

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
        float current_amplitude_update = amplitude_update;
        //amplitude_update = current_amplitude_update + 0.1;
        amplitude_update = current_amplitude_update + abs(average_in_amplitude - last_average_in_amplitude) * delta_spread_amplitude;
        last_average_in_amplitude = average_in_amplitude;
        float current_pitch_update = pitch_update; 
        //pitch_update = current_pitch_update + 0.01;
        pitch_update = current_pitch_update + abs(average_in_pitch - last_average_in_pitch) * delta_spread_pitch;
        last_average_in_pitch = average_in_pitch;
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
        playback = false;
    }
    
    return ring_return;
}

float ofApp::calculate_value(float last_value, float average_in, float out, float spread_in, float ring){
    return mix(last_value, mix(mix(average_in, out, spread_in), average_in * out, ring), parameter_smoothing);
}

void ofApp::audioOut(ofSoundBuffer &buffer){
    if(playback){
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
                //float out_sample = ofRandomf();
                int index = a * out_channels + b;
                buffer[index] = out_sample;
                //buffer[index] = out_sample;
                /*
                z2[b] = z1[b];
                z1[b] = out_sample;
                */
                analysis(out_sample, out_dc[b], out_amplitude_root[b], out_amplitude[b], 
                    out_cross[b], out_cross_count[b], out_pitch[b]);
            }
        }
    }       
}      

//--------------------------------------------------------------
void ofApp::update(){
    update_thread = true;
    update_count++;
    
    //cout << sample_count << endl;

    if(amplitude_update > 1.0){
        ofxOscMessage amplitude_message;
        amplitude_message.setAddress("/amplitude");
        amplitude_message.addFloatArg(average_in_amplitude);
        amplitude_message.addFloatArg(spread_in_amplitude);
        for(int a = 0; a < senders.size(); a++){
            senders[a].sendMessage(amplitude_message);
        }
        amplitude_update -= 1.0;
    }

    if(pitch_update > 1.0){
        ofxOscMessage pitch_message;
        pitch_message.setAddress("/pitch");
        pitch_message.addFloatArg(average_in_pitch);
        pitch_message.addFloatArg(spread_in_pitch);
        for(int a = 0; a < senders.size(); a++){
            senders[a].sendMessage(pitch_message);
        }
        pitch_update -= 1.0;
    }

    if(output){
        if(playback){
            if(receiver.hasWaitingMessages()){
                ofxOscMessage received_message;
                receiver.getNextMessage( received_message);
                string address = received_message.getAddress();
                if(address == "/amplitude"){
                    average_amplitude = received_message.getArgAsFloat(0);
                    spread_amplitude = received_message.getArgAsFloat(1);
                    amplitude_progress += epsilon_float / parameter_smoothing;
                    //cout << "amplitude" << average_amplitude << " " << spread_amplitude << " " << amplitude_progress << endl;
                }
                if(address == "/pitch"){
                    average_pitch = received_message.getArgAsFloat(0);
                    spread_pitch = received_message.getArgAsFloat(1);
                    pitch_progress += epsilon_float / parameter_smoothing;
                    //cout << "pitch" << average_pitch << " " << spread_pitch << " " << pitch_progress << endl;
                }
            }
        }
        else if (end_message){
            cout << "These outputs have completed the piece!" << endl;
            if(input){
                cout << "Input analysis and messaging will continue until the program is terminated." << endl;
            }
            else{
                cout << "Since no audio input channels are present, the program will now attempt to automatically terminate." << endl;
                ofSoundStreamClose();
                ofExit();
            }
            end_message = false;
        }
    }
}

void::ofApp::ofSoundStreamClose(){

}

//--------------------------------------------------------------
void ofApp::exit(){
    
}