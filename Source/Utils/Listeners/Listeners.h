#pragma once

#include "../../Processor/DSP/DSP.h"

struct ExtendedTabBarButton; // Forward declaration

// TAB ORDER CHANGED LISTENER
//==============================================================================
struct TabOrderListener {
  virtual ~TabOrderListener() = default;
  virtual void tabOrderChanged(DspOrder newDspOrder) = 0;
};

// TAB SELECTION LISTENER
//==============================================================================
struct TabSelectionListener {
  virtual ~TabSelectionListener() = default;
  virtual void tabSelectionChanged(int newSelectionIndex,
                                   DspOption dspOption) = 0;
};

// TAB BUTTON EVENT LISTENER
//==============================================================================
struct TabButtonEventListener {
  virtual ~TabButtonEventListener() = default;
  virtual void tabDragStarted(ExtendedTabBarButton *button) = 0;
  virtual void tabDragMoved(ExtendedTabBarButton *button) = 0;
  virtual void tabDragEnded(ExtendedTabBarButton *button) = 0;
};
