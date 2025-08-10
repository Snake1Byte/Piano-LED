#ifndef MIDIHOSTMANAGER_H
#define MIDIHOSTMANAGER_H

#include <Control_Surface.h>
#include <MIDI_Interfaces/USBHostMIDI_Interface.hpp>

class MidiHostManager
{
public:
    MidiHostManager();

    std::function<void(uint8_t note, uint8_t velocity)> onNoteOnCallback;
    std::function<void(uint8_t note, uint8_t velocity)> onNoteOffCallback;
    std::function<void(uint8_t cc, uint8_t value)> onControlChangeCallback;
    std::function<void(bool connected)> onHostConnectedCallback;

    void begin();
    void loop();

private:
    struct LedMidiCallbacks : FineGrainedMIDI_Callbacks<LedMidiCallbacks>
    {
        explicit LedMidiCallbacks(MidiHostManager &owner);
        void onNoteOn(Channel channel, uint8_t note, uint8_t velocity, Cable cable);
        void onNoteOff(Channel channel, uint8_t note, uint8_t velocity, Cable cable);
        void onControlChange(Channel channel, uint8_t cc, uint8_t value, Cable cable);
        MidiHostManager &owner;
    } callbacks;

    boolean VerifyChannel(Channel *channel);

    USBHost usb;
    USBHub hub;
    GenericUSBMIDI_Interface<USBHostMIDIBackend<512>> hostmidi{usb};
    USBMIDI_Interface devicemidi;
    BidirectionalMIDI_Pipe p;

    static boolean hostConnected;
};

#endif // MIDIHOSTMANAGER_H