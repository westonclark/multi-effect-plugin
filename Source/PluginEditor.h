/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

struct ExtendedTabbedButtonBar : juce::TabbedButtonBar,
                                 juce::DragAndDropTarget {
  ExtendedTabbedButtonBar()
      : juce::TabbedButtonBar(juce::TabbedButtonBar::Orientation::TabsAtTop) {}

  bool
  isInterestedInDragSource(const SourceDetails &dragSourceDetails) override {
    return false;
  }

  void itemDropped(const SourceDetails &dragSourceDetails) override {}
};

struct ExtendedTabbedButton : juce::TabBarButton {};
//==============================================================================
/**
 */
class MultieffectpluginAudioProcessorEditor
    : public juce::AudioProcessorEditor {
public:
  MultieffectpluginAudioProcessorEditor(MultieffectpluginAudioProcessor &);
  ~MultieffectpluginAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  MultieffectpluginAudioProcessor &audioProcessor;

  juce::TextButton dspOrderButton{"dspOrder"};
  ExtendedTabbedButtonBar tabbedComponent;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
      MultieffectpluginAudioProcessorEditor)
};
