/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ResynthesiserAudioProcessorEditor::ResynthesiserAudioProcessorEditor (ResynthesiserAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    addAndMakeVisible(noteDisplayLabel);
    addAndMakeVisible(FFTDisplayLabel);

    noteDisplayLabel.setFont(juce::Font(20.0f));
    noteDisplayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    FFTDisplayLabel.setFont(juce::Font(20.0f));
    FFTDisplayLabel.setColour(juce::Label::textColourId, juce::Colours::white);

    setSize (400, 300);
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
}

void ResynthesiserAudioProcessorEditor::timerCallback()
{
    noteDisplayLabel.setText(audioProcessor.lastNoteText, juce::dontSendNotification);
    FFTDisplayLabel.setText(juce::String(audioProcessor.getFundamentalFrequency(),2), juce::dontSendNotification);
    repaint();
}
