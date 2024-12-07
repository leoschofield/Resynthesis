/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ResynthesiserAudioProcessor::ResynthesiserAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
                    fft (fftOrder),
                    window (fftSize, juce::dsp::WindowingFunction<float>::hann)

#endif
{
}

ResynthesiserAudioProcessor::~ResynthesiserAudioProcessor()
{
}

//==============================================================================
const juce::String ResynthesiserAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ResynthesiserAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ResynthesiserAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ResynthesiserAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ResynthesiserAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ResynthesiserAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ResynthesiserAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ResynthesiserAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ResynthesiserAudioProcessor::getProgramName (int index)
{
    return {};
}

void ResynthesiserAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ResynthesiserAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Set the sample rate for the synth
    mySineSynth.setCurrentPlaybackSampleRate(sampleRate);
    juce::ignoreUnused(samplesPerBlock);
    this->sampleRate = sampleRate;
}

void ResynthesiserAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ResynthesiserAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ResynthesiserAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    static int counter = 0;
    
    if( ++counter > 10)
    {
        counter = 0;
        auto amplitude = getRMSAmplitude(buffer);

        mySineSynth.triggerNote(
            frequencyToNearestMidiNote(getFundamentalFrequency()),// message.getNoteNumber(),
                                amplitude);
    }

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    

    // Iterate through MIDI messages directly
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();
        
        // Note On
        if (message.isNoteOn())
        {
            // Trigger the synth
            mySineSynth.triggerNote(
                frequencyToNearestMidiNote(getFundamentalFrequency()),// message.getNoteNumber(),
                message.getVelocity() / 127.0f
            );

            // Create readable note name
            int midiNote = message.getNoteNumber();
            juce::String noteName = getNoteNameFromMidiNumber(midiNote);

            lastNoteText = "Note On: " + noteName +
                           " (MIDI: " + juce::String(midiNote) +
                           ", Vel: " + juce::String(message.getVelocity()) + ")";
        }
        // Note Off
        else if (message.isNoteOff())
        {
            // Release the note
            mySineSynth.releaseNote(message.getNoteNumber());

            lastNoteText = "Note Off: " +
                           juce::String(message.getNoteNumber());
        }
    }
    
    //Load samples into FFT
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getReadPointer(0);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
             pushNextSampleIntoFifo(channelData[i]);
        }
    }

    // Render synth audio
    mySineSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
}

//==============================================================================
bool ResynthesiserAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ResynthesiserAudioProcessor::createEditor()
{
    return new ResynthesiserAudioProcessorEditor (*this);
}

//==============================================================================
void ResynthesiserAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ResynthesiserAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ResynthesiserAudioProcessor();
}
