#pragma once

#include "../../Parameters.h"
#include "../Listeners/Listeners.h"
#include "../PluginProcessor.h"
#include <JuceHeader.h>

// BYPASS PARAMETER LOOKUP
//==============================================================================
inline const Parameter &
getBypassParam(MultieffectpluginAudioProcessor::DspOption option) {
  switch (option) {
  case MultieffectpluginAudioProcessor::DspOption::Phase:
    return Parameters::Phaser::bypass;
  case MultieffectpluginAudioProcessor::DspOption::Chorus:
    return Parameters::Chorus::bypass;
  case MultieffectpluginAudioProcessor::DspOption::OverDrive:
    return Parameters::Overdrive::bypass;
  case MultieffectpluginAudioProcessor::DspOption::LadderFilter:
    return Parameters::LadderFilter::bypass;
  case MultieffectpluginAudioProcessor::DspOption::Filter:
    return Parameters::Filter::bypass;
  case MultieffectpluginAudioProcessor::DspOption::END_OF_LIST:
    break;
  }
  return Parameters::Phaser::bypass;
}

// HORIZONTAL CONSTRAINER
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

// BUTTON
//==============================================================================
struct ExtendedTabBarButton : juce::TabBarButton {
  ExtendedTabBarButton(const juce::String &name, juce::TabbedButtonBar &owner,
                       juce::AudioProcessorValueTreeState *apvts,
                       MultieffectpluginAudioProcessor::DspOption dspOption);

  void mouseDown(const juce::MouseEvent &event) override;
  void mouseDrag(const juce::MouseEvent &event) override;
  void mouseUp(const juce::MouseEvent &event) override;
  int getBestTabLength(int depth) override;

  void setButtonEventListener(TabButtonEventListener *l) { listener = l; }

  MultieffectpluginAudioProcessor::DspOption dspOption;

private:
  juce::ComponentDragger dragger;
  std::unique_ptr<HorizontalConstrainer> constrainer;
  TabButtonEventListener *listener = nullptr;

  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
      bypassAttachment;
};

// TABBED BUTTON BAR
//==============================================================================
struct ExtendedTabbedButtonBar : juce::TabbedButtonBar, TabButtonEventListener {
  ExtendedTabbedButtonBar(juce::AudioProcessorValueTreeState &apvts);

  void addTab(MultieffectpluginAudioProcessor::DspOption option,
              int insertIndex = -1);

  juce::TabBarButton *createTabButton(const juce::String &tabName,
                                      int tabIndex) override;

  void addTabOrderListener(TabOrderListener *l) { tabOrderListener.add(l); }
  void removeTabOrderListener(TabOrderListener *l) {
    tabOrderListener.remove(l);
  }

  void addTabSelectionListener(TabSelectionListener *l) {
    tabSelectionListener.add(l);
  }
  void removeTabSelectionListener(TabSelectionListener *l) {
    tabSelectionListener.remove(l);
  }

  void tabDragStarted(ExtendedTabBarButton *button) override;
  void tabDragMoved(ExtendedTabBarButton *button) override;
  void tabDragEnded(ExtendedTabBarButton *button) override;

  void currentTabChanged(int newSelectionIndex,
                         const juce::String &dspName) override;

private:
  juce::ListenerList<TabOrderListener> tabOrderListener;
  juce::ListenerList<TabSelectionListener> tabSelectionListener;
  juce::AudioProcessorValueTreeState &apvts;
};
