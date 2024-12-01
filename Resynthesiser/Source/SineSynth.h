#pragma once

#include <JuceHeader.h>

class SineSynthSound : public juce::SynthesiserSound {
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

class SineSynthVoice : public juce::SynthesiserVoice {
private:
    juce::ADSR envelope;
    juce::dsp::Oscillator<float> oscillator;
    float currentLevel = 0.0f;

public:
    SineSynthVoice() {
        // Configure very quick envelope
        juce::ADSR::Parameters params;
        params.attack = 0.001f;
        params.decay = 0.02f;
        params.sustain = 0.0f;
        params.release = 0.01f;
        envelope.setParameters(params);
        
        // Default sine oscillator
        oscillator.initialise([](float x) { return std::sin(x); });
    }

//    // Set oscillator function (allows complex waveforms)
//    void setOscillatorFunction(std::function<float(float)> func) {
//        oscillator.initialise(func);
//    }

    bool canPlaySound(juce::SynthesiserSound* sound) override {
        return dynamic_cast<SineSynthSound*>(sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound*, int) override {
        double frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        
        envelope.reset();
        envelope.noteOn();
        
        oscillator.setFrequency(frequency);
        currentLevel = velocity;
    }

    void stopNote(float, bool) override {
        envelope.noteOff();
    }

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                          int startSample, int numSamples) override {
        for (int sample = 0; sample < numSamples; ++sample) {
            float oscillatorSample = oscillator.processSample(0.0f);
            float envelopeSample = envelope.getNextSample();
            float processedSample = oscillatorSample * envelopeSample * currentLevel;
            
            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
                outputBuffer.addSample(channel, startSample + sample, processedSample);
            }
            
            if (!envelope.isActive()) {
                clearCurrentNote();
                break;
            }
        }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
};

class SineSynth : public juce::Synthesiser {
public:
    SineSynth(int numVoices = 64) {
        // Add sounds and voices
        addSound(new SineSynthSound());
        
        for (int i = 0; i < numVoices; ++i) {
            addVoice(new SineSynthVoice());
        }
    }

//    // API methods for external control
//    void setWaveform(std::function<float(float)> waveformFunc) {
//        for (int i = 0; i < getNumVoices(); ++i) {
//            if (auto* voice = dynamic_cast<SineSynthVoice*>(getVoice(i))) {
//                voice->setOscillatorFunction(waveformFunc);
//            }
//        }
//    }

    // Trigger a note directly
    void triggerNote(int midiNoteNumber, float velocity = 0.8f) {
        noteOn(1, midiNoteNumber, velocity);
    }

    // Release a specific note
    void releaseNote(int midiNoteNumber) {
        noteOff(1, midiNoteNumber, 0.0, true);
    }
};
