#pragma once
#include "ofMain.h"
#include "ofxOsc.h"
#include "Midi.h"

// Send OSC data to Wekinator.
#define HOST "localhost"
#define SENDMESSAGE "/wek/inputs"
#define SENDPORT 6448

// Receiving OSC data from the Wekinator.
#define RECEIVEMESSAGE "/wek/outputs"
#define RECEIVEPORT 8000

class Osc {
  public:
    void setup();
    void update();
  
    // Setup the pitch array.
    void setupPitchSpace();
  
    // Wekinator commands.
    void sendPositionToWekinator(glm::vec2 pos);
  
    // Incoming commands.
    void processIncomingCommands(ofxOscMessage &m);
  
    // Channels.
    const int midiChannel = 3;
    
  private:
  
    // Wekinator OSC receiver.
    ofxOscReceiver receiver;
  
    // Wekinator OSC sender.
    ofxOscSender sender;
  
    // Track previous notes so as to not send the same note again. 
    int previousMidiNote;
  
    // A pitch space. 
    vector<int> pitchSpace;
};
