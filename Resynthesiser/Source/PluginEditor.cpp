/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ResynthesiserAudioProcessorEditor::ResynthesiserAudioProcessorEditor (ResynthesiserAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
        fundamentalAttachment     (p.state, "fundamental", fundamentalSlider),
        dragAttachment            (p.state, "drag",   dragSlider),
        rangeAttachment           (p.state, "range", rangeSlider),
        grainDensityAttachment    (p.state, "grainDensity",  grainDensitySlider),
        grainWindowAttachment     (p.state, "grainWindow",  grainWindowSlider),
        grainSizeAttachment       (p.state, "grainSize",  grainSizeSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    addAndMakeVisible(noteDisplayLabel);
    addAndMakeVisible(FFTDisplayLabel);

    noteDisplayLabel.setFont(juce::Font(20.0f));
    noteDisplayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    FFTDisplayLabel.setFont(juce::Font(20.0f));
    FFTDisplayLabel.setColour(juce::Label::textColourId, juce::Colours::white);

    fundamentalSlider    .setSliderStyle (juce::Slider::SliderStyle::LinearVertical);
    rangeSlider.setSliderStyle (juce::Slider::SliderStyle::LinearVertical);
    dragSlider     .setSliderStyle (juce::Slider::SliderStyle::LinearVertical);
    grainDensitySlider  .setSliderStyle (juce::Slider::SliderStyle::LinearVertical);
    grainWindowSlider  .setSliderStyle (juce::Slider::SliderStyle::LinearVertical);
    grainSizeSlider  .setSliderStyle (juce::Slider::SliderStyle::LinearVertical);
    
    addAndMakeVisible(fundamentalSlider);
    addAndMakeVisible(rangeSlider);
    addAndMakeVisible(dragSlider);
    addAndMakeVisible(grainDensitySlider);
    addAndMakeVisible(grainWindowSlider);
    addAndMakeVisible(grainSizeSlider);

    setSize (800, 600);
    startTimer(60);
}

ResynthesiserAudioProcessorEditor::~ResynthesiserAudioProcessorEditor()
{
}

//==============================================================================
void ResynthesiserAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);

}

void ResynthesiserAudioProcessorEditor::resized()
{
    noteDisplayLabel.setBounds(10, 10, getWidth() - 20, 50);
    FFTDisplayLabel.setBounds(10, 30, getWidth() - 20, 50);
    
    juce::Rectangle<int> bounds = getLocalBounds();
    const int numParams = 6;
    int margin = 20;

    juce::Rectangle<int> param1Bounds = bounds.removeFromLeft (getWidth() / numParams);
    juce::Rectangle<int> param2Bounds = bounds.removeFromLeft (getWidth() / numParams-1);
    juce::Rectangle<int> param3Bounds = bounds.removeFromLeft (getWidth() / numParams-2);
    juce::Rectangle<int> param4Bounds = bounds.removeFromLeft (getWidth() / numParams-3);
    juce::Rectangle<int> param5Bounds = bounds.removeFromLeft (getWidth() / numParams-4);
    juce::Rectangle<int> param6Bounds = bounds.removeFromLeft (getWidth() / numParams-5);

    fundamentalSlider.setBounds (param1Bounds.reduced (margin));
    rangeSlider.setBounds (param2Bounds.reduced (margin));
    dragSlider.setBounds (param3Bounds.reduced (margin));
    grainDensitySlider.setBounds (param4Bounds.reduced (margin));
    grainWindowSlider.setBounds (param5Bounds.reduced (margin));
    grainSizeSlider.setBounds (param6Bounds.reduced (margin));

}

void ResynthesiserAudioProcessorEditor::timerCallback()
{
    noteDisplayLabel.setText(audioProcessor.lastNoteText, juce::dontSendNotification);
    FFTDisplayLabel.setText(juce::String(audioProcessor.getFundamentalFrequency(),2), juce::dontSendNotification);
    repaint();
}
