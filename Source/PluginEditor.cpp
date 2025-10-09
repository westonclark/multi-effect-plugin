/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
MultieffectpluginAudioProcessorEditor::MultieffectpluginAudioProcessorEditor(
    MultieffectpluginAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  dspOrderButton.onClick = [this]() {
    juce::Random random;
    MultieffectpluginAudioProcessor::DSP_Order dspOrder;

    auto range = juce::Range<int>(
        static_cast<int>(MultieffectpluginAudioProcessor::DSP_Option::Phase),
        static_cast<int>(
            MultieffectpluginAudioProcessor::DSP_Option::END_OF_LIST));
    for (auto &value : dspOrder) {
      auto entry = random.nextInt(range);
      value = static_cast<MultieffectpluginAudioProcessor::DSP_Option>(entry);
    }

    DBG(juce::Base64::toBase64(dspOrder.data(), dspOrder.size()));

    audioProcessor.dspOrderFifo.push(dspOrder);
  };

  addAndMakeVisible(dspOrderButton);
  setSize(400, 300);
}

MultieffectpluginAudioProcessorEditor::
    ~MultieffectpluginAudioProcessorEditor() {}

//==============================================================================
void MultieffectpluginAudioProcessorEditor::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  g.setColour(juce::Colours::white);
  g.setFont(15.0f);
  g.drawFittedText("Hello World!", getLocalBounds(),
                   juce::Justification::centred, 1);
}

void MultieffectpluginAudioProcessorEditor::resized() {
  dspOrderButton.setBounds(getLocalBounds().reduced(100));
}
