#pragma once

#include "../../Processor/PluginProcessor.h"

struct ExtendedTabBarButton; // Forward declaration

// TAB ORDER CHANGED LISTENER
//==============================================================================
struct TabOrderListener {
  virtual ~TabOrderListener() = default;
  virtual void
  tabOrderChanged(MultieffectpluginAudioProcessor::DspOrder newDspOrder) = 0;
};

// TAB SELECTION LISTENER
//==============================================================================
struct TabSelectionListener {
  virtual ~TabSelectionListener() = default;
  virtual void
  tabSelectionChanged(int newSelectionIndex,
                      MultieffectpluginAudioProcessor::DspOption dspOption) = 0;
};

// TAB BUTTON EVENT LISTENER
//==============================================================================
struct TabButtonEventListener {
  virtual ~TabButtonEventListener() = default;
  virtual void tabDragStarted(ExtendedTabBarButton *button) = 0;
  virtual void tabDragMoved(ExtendedTabBarButton *button) = 0;
  virtual void tabDragEnded(ExtendedTabBarButton *button) = 0;
};
