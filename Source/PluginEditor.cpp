#include "PluginEditor.h"
#include "PluginProcessor.h"

// EDITOR
//==============================================================================
MultieffectpluginAudioProcessorEditor::MultieffectpluginAudioProcessorEditor(
    MultieffectpluginAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p), tabBarComponent(),
      phaserPanel(p.apvts), chorusPanel(p.apvts), drivePanel(p.apvts),
      ladderFilterPanel(p.apvts), filterPanel(p.apvts) {

  // Load DSP order and populate tabs
  auto dspOrder = audioProcessor.getDspOrderFromState();
  for (const auto &dspOption : dspOrder) {
    tabBarComponent.addTab(
        MultieffectpluginAudioProcessor::getDspNameFromOption(dspOption),
        juce::Colours::white, -1);
  }

  // Register listeners
  tabBarComponent.addTabOrderListener(this);
  tabBarComponent.addTabSelectionListener(this);

  // Find index of the currently selected tab
  auto savedTab = audioProcessor.getSelectedTabFromState();
  int savedTabIndex = 0;
  for (int i = 0; i < dspOrder.size(); ++i) {
    if (dspOrder[i] == savedTab) {
      savedTabIndex = i;
      break;
    }
  }
  tabBarComponent.setCurrentTabIndex(savedTabIndex, true);

  addAndMakeVisible(tabBarComponent);

  addChildComponent(phaserPanel);
  addChildComponent(chorusPanel);
  addChildComponent(drivePanel);
  addChildComponent(ladderFilterPanel);
  addChildComponent(filterPanel);
  showDspPanel(savedTab);

  setSize(400, 300);
}

MultieffectpluginAudioProcessorEditor::
    ~MultieffectpluginAudioProcessorEditor() {
  tabBarComponent.removeTabOrderListener(this);
  tabBarComponent.removeTabSelectionListener(this);
}

void MultieffectpluginAudioProcessorEditor::tabOrderChanged(
    MultieffectpluginAudioProcessor::DspOrder newOrder) {
  audioProcessor.saveDspOrderToState(newOrder);
  audioProcessor.dspOrderFifo.push(newOrder);
}

void MultieffectpluginAudioProcessorEditor::tabSelectionChanged(
    int newSelectionIndex,
    MultieffectpluginAudioProcessor::DspOption dspOption) {
  showDspPanel(dspOption);
  audioProcessor.saveSelectedTabToState(dspOption);
}

void MultieffectpluginAudioProcessorEditor::showDspPanel(
    MultieffectpluginAudioProcessor::DspOption dspOption) {
  phaserPanel.setVisible(dspOption ==
                         MultieffectpluginAudioProcessor::DspOption::Phase);
  chorusPanel.setVisible(dspOption ==
                         MultieffectpluginAudioProcessor::DspOption::Chorus);
  drivePanel.setVisible(dspOption ==
                        MultieffectpluginAudioProcessor::DspOption::OverDrive);
  ladderFilterPanel.setVisible(
      dspOption == MultieffectpluginAudioProcessor::DspOption::LadderFilter);
  filterPanel.setVisible(dspOption ==
                         MultieffectpluginAudioProcessor::DspOption::Filter);
}

void MultieffectpluginAudioProcessorEditor::paint(juce::Graphics &g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MultieffectpluginAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds();
  tabBarComponent.setBounds(bounds.removeFromTop(30));

  phaserPanel.setBounds(bounds);
  chorusPanel.setBounds(bounds);
  drivePanel.setBounds(bounds);
  ladderFilterPanel.setBounds(bounds);
  filterPanel.setBounds(bounds);
}
