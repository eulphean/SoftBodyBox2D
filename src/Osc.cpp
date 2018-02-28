#include "Osc.h"

void Osc::setup() {
  // Setup midi
  Midi::instance().setup();
  
  // Populate vector. 
  setupPitchSpace();
  
  // Allocate space for senders.
  senders.resize(sendPorts.size());
  
  // Setup sendPorts for OSC.
  for (int i = 0; i < sendPorts.size(); i++) {
    senders[i].setup(HOST, sendPorts[i]);
  }
  
  // Common OSC receiver for messages sent from Wekinator. 
  receiver.setup(RECEIVEPORT);
  
  // Set bogus previous user midi notes.
  previousUserANote = -1;
  previousUserBNote = -1;
}

void Osc::update() {
    while (receiver.hasWaitingMessages()) {
      ofxOscMessage m;
      // Set the next message.
      #pragma warning(disable: WARNING_CODE)
      
      receiver.getNextMessage(&m);
      
      // Process incoming Wekinator commands.
      processIncomingCommands(m);
    }
}

// Extract the OSC values per user value.
void Osc::processIncomingCommands(ofxOscMessage &m) {
  // User 1
  if (m.getAddress() == "/wek/1") {
    int note = pitchSpace[m.getArgAsInt(0)];
    if (previousUserANote != note) {
      Midi::instance().sendMidiNoteOnChannel(userAChannel, note);
      previousUserANote = note;
    }
  }
  
  // User 2 [NOTE] - The space is arranged in an opposite manner.
  if (m.getAddress() == "/wek/2") {
    int note = pitchSpace[m.getArgAsInt(0)];
    if (previousUserBNote != note) {
      Midi::instance().sendMidiNoteOnChannel(userBChannel, note);
      previousUserBNote = note;
    }
  }
  
  // Perimeter between them.
  if (m.getAddress() == "/wek/3") {
  
  }
}

// Send the position to Wekinator.
void Osc::sendPositionToWekinator(int idx, glm::vec3 pos) {
  if (idx < senders.size()) {
    ofxOscMessage m;
    m.setAddress(string(SENDMESSAGE));
    m.addFloatArg((float)pos.x);
    m.addFloatArg((float)pos.y);
    m.addFloatArg((float)pos.z);
    senders[idx].sendMessage(m, false);
  }
}

void Osc::setupPitchSpace() {
  // Row 1
  pitchSpace.push_back(46);
  pitchSpace.push_back(53);
  pitchSpace.push_back(48);
  //pitchSpace.push_back(67);
  //pitchSpace.push_back(74);
  //pitchSpace.push_back(81);
  
  // Row 2
  pitchSpace.push_back(42);
  pitchSpace.push_back(49);
  pitchSpace.push_back(55);
  //pitchSpace.push_back(63);
  //pitchSpace.push_back(70);
  //pitchSpace.push_back(77);
  
  // Row 3
  pitchSpace.push_back(38);
  pitchSpace.push_back(45);
  pitchSpace.push_back(52);
  //pitchSpace.push_back(59);
  //pitchSpace.push_back(66);
  //pitchSpace.push_back(72);
  
  // Row 4
  //  pitchSpace.push_back(34);
  //  pitchSpace.push_back(41);
  //  pitchSpace.push_back(48);
  //  pitchSpace.push_back(55);
  //  //pitchSpace.push_back(63);
  //  //pitchSpace.push_back(69);
  //
  //  // Row 5
  //  pitchSpace.push_back(30);
  //  pitchSpace.push_back(37);
  //  pitchSpace.push_back(32);
  //  pitchSpace.push_back(51);
  //  pitchSpace.push_back(58);
  //  pitchSpace.push_back(65);
  //
  //  // Row 6
  //  pitchSpace.push_back(27);
  //  pitchSpace.push_back(33);
  //  pitchSpace.push_back(40);
  //  pitchSpace.push_back(48);
  //  pitchSpace.push_back(54);
  //  pitchSpace.push_back(61);
}
