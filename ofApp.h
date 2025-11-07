#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		ofSoundStreamSettings settings;
		ofSoundStream stream;
		void ofSoundStreamSetup(ofSoundStreamSettings &settings);
		void audioIn(ofSoundBuffer &inBuffer);
		void audioOut(ofSoundBuffer &outBuffer);
		void setup() override;
		void update() override;
		void exit() override;

};
