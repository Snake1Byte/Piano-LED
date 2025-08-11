#include "MidiHostManager.h"
#include "PianoLedConfig.h"

MidiHostManager::MidiHostManager()
    : onNoteOnCallback(nullptr),
      onNoteOffCallback(nullptr),
      onControlChangeCallback(nullptr),
      onHostConnectedCallback(nullptr),
      callbacks(*this),
      hub(usb)
{
}

MidiHostManager::LedMidiCallbacks::LedMidiCallbacks(MidiHostManager &o)
    : owner(o) {}

boolean MidiHostManager::hostConnected = false;

void MidiHostManager::LedMidiCallbacks::onNoteOn(Channel channel, uint8_t note, uint8_t velocity, Cable cable)
{
    if (velocity == 0)
    {
        onNoteOff(channel, note, 0, cable);
        return;
    }

    if (!owner.VerifyChannel(&channel))
    {
        return;
    }

    if (owner.onNoteOnCallback)
    {
        owner.onNoteOnCallback(note, velocity);
    }
}

void MidiHostManager::LedMidiCallbacks::onNoteOff(Channel channel, uint8_t note, uint8_t velocity, Cable cable)
{
    if (!owner.VerifyChannel(&channel))
    {
        return;
    }

    if (owner.onNoteOffCallback)
    {
        owner.onNoteOffCallback(note, velocity);
    }
}

void MidiHostManager::LedMidiCallbacks::onControlChange(Channel channel, uint8_t cc, uint8_t value, Cable cable)
{
    if (!owner.VerifyChannel(&channel))
    {
        return;
    }

    if (owner.onControlChangeCallback)
    {
        owner.onControlChangeCallback(cc, value);
    }
}

boolean MidiHostManager::VerifyChannel(Channel *channel)
{
    int channelNumber = channel->getRaw() + 1; // Convert to 1-based channel number
    if (std::find(PianoLedConfig::globalConfig.midiChannelsToListen.begin(), PianoLedConfig::globalConfig.midiChannelsToListen.end(), channelNumber) == PianoLedConfig::globalConfig.midiChannelsToListen.end())
    {
        return false;
    }
    return true;
}

void MidiHostManager::begin()
{
    usb.begin();
    hostmidi.setCallbacks(callbacks);
    devicemidi.setCallbacks(callbacks);
    hostmidi | p | devicemidi;
    if (!hostmidi.backend.backend)
    {
        delay(1500);
        if (onHostConnectedCallback)
        {
            onHostConnectedCallback(false);
        }
    }
}

void MidiHostManager::loop()
{
    if (hostmidi.backend.backend && !hostConnected)
    {
        hostConnected = true;
        if (onHostConnectedCallback)
        {
            onHostConnectedCallback(hostConnected);
        }
    }
    else if (!hostmidi.backend.backend && hostConnected)
    {
        hostConnected = false;
        if (onHostConnectedCallback)
        {
            onHostConnectedCallback(hostConnected);
        }
    }

    if (!hostConnected)
    {
        return;
    }

    hostmidi.update();
    devicemidi.update();
}