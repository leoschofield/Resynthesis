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
    
    
    float getFundamentalFrequency()
    {
        if (readyToProcessFFT)
        {
            readyToProcessFFT = false;
            window.multiplyWithWindowingTable(fftBuffer.data(), fftSize);
            fft.performFrequencyOnlyForwardTransform(fftBuffer.data());

            return findFundamentalFrequency();
        }

        return 0.0f; // No new frequency computed
    }
    
    int frequencyToNearestMidiNote(float frequencyHz)
    {
        // Validate input to prevent unreasonable values
        if (frequencyHz <= 0)
            return 0;
        
        // Use the standard formula to convert frequency to MIDI note number
        // A4 (MIDI note 69) is defined as 440 Hz
        float midiNoteFloat = 12.0 * (std::log2(frequencyHz / 440.0)) + 69.0;
        
        // Round to the nearest integer
        return std::round(midiNoteFloat);
    };
    
    float getRMSAmplitude(const juce::AudioBuffer<float>& buffer)
    {
        float sumOfSquares = 0.0f;
        int totalSamples = buffer.getNumChannels() * buffer.getNumSamples();
        
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            const float* channelData = buffer.getReadPointer(channel);
            
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                sumOfSquares += channelData[sample] * channelData[sample];
            }
        }
        
        return std::sqrt(sumOfSquares / totalSamples);
    }
    
    float getPeakAmplitude(const juce::AudioBuffer<float>& buffer)
    {
        float maxAmplitude = 0.0f;
        
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            const float* channelData = buffer.getReadPointer(channel);
            
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                maxAmplitude = std::max(maxAmplitude, std::abs(channelData[sample]));
            }
        }
        
        return std::min(maxAmplitude, 1.0f);  // Ensure it never exceeds 1.0
    }
    
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
    
    static constexpr int fftOrder = 11; // FFT size = 2^11 = 2048
    static constexpr int fftSize = 1 << fftOrder;

    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> window;
    std::array<float, fftSize * 2> fftBuffer { 0.0f };
    std::array<float, fftSize> fifoBuffer { 0.0f };
    int fifoIndex = 0;
    bool readyToProcessFFT = false;
    double sampleRate = 44100.0;

    void pushNextSampleIntoFifo (float sample)
    {
        if (fifoIndex < fftSize)
        {
            fifoBuffer[fifoIndex++] = sample;

            if (fifoIndex == fftSize)
            {
                if (!readyToProcessFFT)
                {
                    std::copy(fifoBuffer.begin(), fifoBuffer.end(), fftBuffer.begin());
                    readyToProcessFFT = true;
                }
                fifoIndex = 0;
            }
        }
    }

    float findFundamentalFrequency()
    {
        auto maxIndex = 0;
        auto maxValue = 0.0f;

        // Find the bin with the maximum magnitude
        for (int i = 1; i < fftSize / 2; ++i)
        {
            if (fftBuffer[i] > maxValue)
            {
                maxValue = fftBuffer[i];
                maxIndex = i;
            }
        }

        // Calculate the fundamental frequency
        auto fundamentalFreq = (maxIndex * sampleRate) / fftSize;
        return fundamentalFreq;
    }
//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResynthesiserAudioProcessor)
};
