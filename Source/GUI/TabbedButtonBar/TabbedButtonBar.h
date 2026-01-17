#pragma once

#include "../Listeners/Listeners.h"
#include "../PluginProcessor.h"
#include <JuceHeader.h>

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
  ExtendedTabBarButton(const juce::String &name, juce::TabbedButtonBar &owner);

  void mouseDown(const juce::MouseEvent &event) override;
  void mouseDrag(const juce::MouseEvent &event) override;
  void mouseUp(const juce::MouseEvent &event) override;
  int getBestTabLength(int depth) override;

  void setButtonEventListener(TabButtonEventListener *l) { listener = l; }

private:
  juce::ComponentDragger dragger;
  std::unique_ptr<HorizontalConstrainer> constrainer;
  TabButtonEventListener *listener = nullptr;
};

// TABBED BUTTON BAR
//==============================================================================
struct ExtendedTabbedButtonBar : juce::TabbedButtonBar, TabButtonEventListener {
  ExtendedTabbedButtonBar();

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

  void finalizeTabOrder();

private:
  juce::ListenerList<TabOrderListener> tabOrderListener;
  juce::ListenerList<TabSelectionListener> tabSelectionListener;
};
