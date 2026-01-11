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

// TAB ORDER CHANGED LISTENER
//==============================================================================
struct TabOrderListener {
  virtual ~TabOrderListener() = default;
  virtual void
  tabOrderChanged(MultieffectpluginAudioProcessor::DSP_Order newDspOrder) = 0;
};

// TAB BUTTON EVENT LISTENER
//==============================================================================
struct ExtendedTabBarButton;

struct TabButtonEventListener {
  virtual ~TabButtonEventListener() = default;
  virtual void tabDragStarted(ExtendedTabBarButton *button) = 0;
  virtual void tabDragMoved(ExtendedTabBarButton *button) = 0;
  virtual void tabDragEnded(ExtendedTabBarButton *button) = 0;
};

// BUTTON BAR
//==============================================================================
struct ExtendedTabbedButtonBar : juce::TabbedButtonBar, TabButtonEventListener {
  ExtendedTabbedButtonBar();

  juce::TabBarButton *createTabButton(const juce::String &tabName,
                                      int tabIndex) override;

  void addListener(TabOrderListener *l) { listeners.add(l); }
  void removeListener(TabOrderListener *l) { listeners.remove(l); }

  void tabDragStarted(ExtendedTabBarButton *button) override;
  void tabDragMoved(ExtendedTabBarButton *button) override;
  void tabDragEnded(ExtendedTabBarButton *button) override;

  void finalizeTabOrder();

private:
  juce::ListenerList<TabOrderListener> listeners;
};

// BUTTON
//==============================================================================
struct ExtendedTabBarButton : juce::TabBarButton {
  ExtendedTabBarButton(const juce::String &name, juce::TabbedButtonBar &owner);
  juce::ComponentDragger dragger;
  std::unique_ptr<HorizontalConstrainer> constrainer;

  void mouseDown(const juce::MouseEvent &event) override;
  void mouseDrag(const juce::MouseEvent &event) override;
  void mouseUp(const juce::MouseEvent &event) override;
  int getBestTabLength(int depth) override;

  void setButtonEventListener(TabButtonEventListener *l) { listener = l; }

private:
  TabButtonEventListener *listener = nullptr;
};

// EDITOR
//==============================================================================
class MultieffectpluginAudioProcessorEditor : public juce::AudioProcessorEditor,
                                              public TabOrderListener {
public:
  MultieffectpluginAudioProcessorEditor(MultieffectpluginAudioProcessor &);
  ~MultieffectpluginAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

  void
  tabOrderChanged(MultieffectpluginAudioProcessor::DSP_Order newOrder) override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  MultieffectpluginAudioProcessor &audioProcessor;

  ExtendedTabbedButtonBar tabbedComponent;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
      MultieffectpluginAudioProcessorEditor)
};
