#pragma once

#include "GUI/DspPanels/Chorus/ChorusPanel.h"
#include "GUI/DspPanels/Drive/DrivePanel.h"
#include "GUI/DspPanels/Filter/FilterPanel.h"
#include "GUI/DspPanels/LadderFilter/LadderFilterPanel.h"
#include "GUI/DspPanels/Phaser/PhaserPanel.h"
#include "GUI/Listeners/Listeners.h"
#include "GUI/LookAndFeel.h"
#include "GUI/SpectrumAnalyzer/SpectrumAnalyzer.h"
#include "GUI/TabbedButtonBar/TabbedButtonBar.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>

// EDITOR
//==============================================================================
class MultieffectpluginAudioProcessorEditor : public juce::AudioProcessorEditor,
                                              public TabOrderListener,
                                              public TabSelectionListener {
public:
  MultieffectpluginAudioProcessorEditor(MultieffectpluginAudioProcessor &);
  ~MultieffectpluginAudioProcessorEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

  void
  tabOrderChanged(MultieffectpluginAudioProcessor::DspOrder newOrder) override;
  void tabSelectionChanged(
      int newSelectionIndex,
      MultieffectpluginAudioProcessor::DspOption dspOption) override;

private:
  LookAndFeel lookAndFeel;
  MultieffectpluginAudioProcessor &audioProcessor;

  void showDspPanel(MultieffectpluginAudioProcessor::DspOption dspOption);

  ExtendedTabbedButtonBar tabBar;
  SpectrumAnalyzer spectrumAnalyzer;

  PhaserPanel phaserPanel;
  ChorusPanel chorusPanel;
  DrivePanel drivePanel;
  LadderFilterPanel ladderFilterPanel;
  FilterPanel filterPanel;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
      MultieffectpluginAudioProcessorEditor)
};
