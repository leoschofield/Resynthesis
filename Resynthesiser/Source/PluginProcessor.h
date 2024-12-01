/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SineSynth.h"

//==============================================================================
/**
*/
class ResynthesiserAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ResynthesiserAudioProcessor();
    ~ResynthesiserAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
//    void reset() override { synth.reset(); }

    void handleIncomingMidiMessage(const juce::MidiMessage& message);
    void addMidiMessage(const juce::MidiMessage& message);
    
    juce::String lastNoteText;
    
private:
    SineSynth mySineSynth;
    
    // Utility method to convert MIDI note to note name
   static juce::String getNoteNameFromMidiNumber(int midiNoteNumber)
   {
       juce::String noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
       int octave = (midiNoteNumber / 12) - 1;
       int noteIndex = midiNoteNumber % 12;
       return noteNames[noteIndex] + juce::String(octave);
   }
//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResynthesiserAudioProcessor)
};
