#pragma once

#include "../Components/Chorus/ChorusPanel.h"
#include "../Components/Drive/DrivePanel.h"
#include "../Components/Filter/FilterPanel.h"
#include "../Components/Input/Input.h"
#include "../Components/LadderFilter/LadderFilterPanel.h"
#include "../Components/Output/Output.h"
#include "../Components/Phaser/PhaserPanel.h"
#include "../Components/SpectrumAnalyzer/SpectrumAnalyzer.h"
#include "../Components/TabbedButtonBar/TabbedButtonBar.h"
#include "../LookAndFeel.h"
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

  void tabOrderChanged(DspOrder newOrder) override;
  void tabSelectionChanged(int newSelectionIndex, DspOption dspOption) override;

private:
  LookAndFeel lookAndFeel;
  MultieffectpluginAudioProcessor &audioProcessor;

  void showDspPanel(DspOption dspOption);

  ExtendedTabbedButtonBar tabBar;
  SpectrumAnalyzer spectrumAnalyzer;

  PhaserPanel phaserPanel;
  ChorusPanel chorusPanel;
  DrivePanel drivePanel;
  LadderFilterPanel ladderFilterPanel;
  FilterPanel filterPanel;
  Input input;
  Output output;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
      MultieffectpluginAudioProcessorEditor)
};
