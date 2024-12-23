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

    fundamentalSlider    .setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    rangeSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    dragSlider     .setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    grainDensitySlider  .setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    grainWindowSlider  .setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    grainSizeSlider  .setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    
    addAndMakeVisible(fundamentalSlider);
    addAndMakeVisible(rangeSlider);
    addAndMakeVisible(dragSlider);
    addAndMakeVisible(grainDensitySlider);
    addAndMakeVisible(grainWindowSlider);
    addAndMakeVisible(grainSizeSlider);
    
    fundamentalLabel, rangeLabel, dragLabel, grainDensityLabel, grainWindowLabel, grainSizeLabel;

    
    fundamentalLabel.setText("Fundamental", juce::dontSendNotification);
    fundamentalLabel.attachToComponent(&fundamentalSlider, true);
    addAndMakeVisible(fundamentalLabel);
    
    rangeLabel.setText("Range", juce::dontSendNotification);
    rangeLabel.attachToComponent(&rangeSlider, true);
    addAndMakeVisible(rangeLabel);

    dragLabel.setText("Drag", juce::dontSendNotification);
    dragLabel.attachToComponent(&dragSlider, true);
    addAndMakeVisible(dragLabel);

    grainDensityLabel.setText("Grain Density", juce::dontSendNotification);
    grainDensityLabel.attachToComponent(&grainDensitySlider, true);
    addAndMakeVisible(grainDensityLabel);

    grainWindowLabel.setText("Grain Window", juce::dontSendNotification);
    grainWindowLabel.attachToComponent(&grainWindowSlider, true);
    addAndMakeVisible(grainWindowLabel);

    grainSizeLabel.setText("Grain Size", juce::dontSendNotification);
    grainSizeLabel.attachToComponent(&grainSizeSlider, true);
    addAndMakeVisible(grainSizeLabel);



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
    
//    juce::Rectangle<int> bounds = getLocalBounds();
//    const int numParams = 6;
    const int margin = 20;
    const int labelWidth = 100;
    const int labelHeight = 100;


    juce::Rectangle<int> param1Bounds(labelWidth,200,getWidth()-labelWidth, labelHeight);
    juce::Rectangle<int> param2Bounds(labelWidth,250,getWidth()-labelWidth, labelHeight);
    juce::Rectangle<int> param3Bounds(labelWidth,300,getWidth()-labelWidth, labelHeight);
    juce::Rectangle<int> param4Bounds(labelWidth,350,getWidth()-labelWidth, labelHeight);
    juce::Rectangle<int> param5Bounds(labelWidth,400,getWidth()-labelWidth, labelHeight);
    juce::Rectangle<int> param6Bounds(labelWidth,450,getWidth()-labelWidth, labelHeight);

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
