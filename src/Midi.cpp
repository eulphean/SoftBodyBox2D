#include "Midi.h"

void Midi::setup() {
  // MIDI setup.
  midiOut.openVirtualPort("ofxMidiOut"); // open a virtual port
  channelMidiNote = 2;
  channelControlChangeRotary = 3;
  channelControlChangeDishes = 4;
}

// Scene selection.
void Midi::sendMidiNoteOn(int midiNote) {
  midiOut.sendNoteOn(channelMidiNote, midiNote, 64);
}

void Midi::sendMidiNoteOff(int midiNote) {
  midiOut.sendNoteOff(channelMidiNote, midiNote, 64);
}

void Midi::sendMidiNoteOnChannel(int channel, int midiNote) {
  midiOut.sendNoteOn(channel, midiNote, 100);
}

// Rotary button mapping.
void Midi::sendMidiControlChangeRotary(int device, float val) {
    // Map rotary values to Midi signals. Value lies between 0 - 127
    int midiVal = ofMap(val, 0, 1, 0, 127, true);

    switch (device) {
      case 0: {
        // Channel, control, midi value
        midiOut.sendControlChange(channelControlChangeRotary, 10, midiVal);
        break;
      }
      
      case 1: {
        // Channel, control, midi value
        midiOut.sendControlChange(channelControlChangeRotary, 11, midiVal);
        break;
      }
      
      case 2: {
        // override Midi val for Arprate
        midiVal = ofMap(val, 0, 1, 48, 90);
        // Channel, control, midi value
        midiOut.sendControlChange(channelControlChangeRotary, 12, midiVal);
        break;
      }
      
      case 3: {
        // Channel, control, midi value
        midiOut.sendControlChange(channelControlChangeRotary, 13, midiVal);
        break;
      }
      
      default:
        break;
    }
  //}
}

void Midi::exit() {
  midiOut.closePort();
}

Midi &Midi::instance() {
  return m;
}

// For a static class, variable needs to be
// initialized in the implementation file.
Midi Midi::m;
