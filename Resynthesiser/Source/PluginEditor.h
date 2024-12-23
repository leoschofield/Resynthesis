/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ResynthesiserAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                            public juce::Timer
{
public:
    ResynthesiserAudioProcessorEditor (ResynthesiserAudioProcessor&);
    ~ResynthesiserAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ResynthesiserAudioProcessor& audioProcessor;
    juce::Label noteDisplayLabel;
    juce::Label FFTDisplayLabel;

    juce::Slider fundamentalSlider, rangeSlider, dragSlider, grainDensitySlider, grainWindowSlider, grainSizeSlider;
    juce::Label fundamentalLabel, rangeLabel, dragLabel, grainDensityLabel, grainWindowLabel, grainSizeLabel;
    juce::AudioProcessorValueTreeState::SliderAttachment fundamentalAttachment, dragAttachment, rangeAttachment, grainDensityAttachment, grainWindowAttachment, grainSizeAttachment;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResynthesiserAudioProcessorEditor)
};
