/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

// HORIZONTAL CONSTRATINER
//==============================================================================
struct HorizontalConstrainer : juce::ComponentBoundsConstrainer {
  HorizontalConstrainer(
      std::function<juce::Rectangle<int>()> confinerBoundsGetter,
      std::function<juce::Rectangle<int>()> confineeBoundsGetter);

  void checkBounds(juce::Rectangle<int> &bounds,
                   const juce::Rectangle<int> &previousBounds,
                   const juce::Rectangle<int> &limits, bool isStretchingTop,
                   bool isStretchingLeft, bool isStretchingBottom,
                   bool isStretchingRight) override;

private:
  std::function<juce::Rectangle<int>()> boundsToConfineToGetter;
  std::function<juce::Rectangle<int>()> boundsOfConfineeGetter;
};

// BUTTON BAR
//==============================================================================
struct ExtendedTabbedButtonBar : juce::TabbedButtonBar,
                                 juce::DragAndDropTarget,
                                 juce::DragAndDropContainer {
  ExtendedTabbedButtonBar();

  bool
  isInterestedInDragSource(const SourceDetails &dragSourceDetails) override;

  void itemDragEnter(const SourceDetails &dragSourceDetails) override;
  void itemDragMove(const SourceDetails &dragSourceDetails) override;
  void itemDragExit(const SourceDetails &dragSourceDetails) override;
  void itemDropped(const SourceDetails &dragSourceDetails) override;

  void mouseDown(const juce::MouseEvent &e) override;

  juce::TabBarButton *createTabButton(const juce::String &tabName,
                                      int tabIndex) override;

private:
};

// BUTTON
//==============================================================================
struct ExtendedTabBarButton : juce::TabBarButton {
  ExtendedTabBarButton(const juce::String &name, juce::TabbedButtonBar &owner);
  juce::ComponentDragger dragger;
  std::unique_ptr<HorizontalConstrainer> constrainer;

  void mouseDown(const juce::MouseEvent &event) override;
  void mouseDrag(const juce::MouseEvent &event) override;
};

// EDITOR
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
