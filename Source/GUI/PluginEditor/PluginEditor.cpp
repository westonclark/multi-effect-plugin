#include "PluginEditor.h"
#include "../../Processor/PluginProcessor/PluginProcessor.h"

// EDITOR
//==============================================================================
MultieffectpluginAudioProcessorEditor::MultieffectpluginAudioProcessorEditor(
    MultieffectpluginAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p), tabBar(p.parameters.apvts),
      phaserPanel(p.parameters.apvts), chorusPanel(p.parameters.apvts),
      drivePanel(p.parameters.apvts), ladderFilterPanel(p.parameters.apvts),
      filterPanel(p.parameters.apvts),
      input(p.parameters.apvts, p.inputLevelFifo),
      output(p.parameters.apvts, p.outputLevelFifo) {

  setLookAndFeel(&lookAndFeel);

  addAndMakeVisible(spectrumAnalyzer);
  addAndMakeVisible(tabBar);
  addAndMakeVisible(input);
  addAndMakeVisible(output);

  // Load DSP order and populate tabs
  auto dspOrder = audioProcessor.getDspOrderFromState();
  for (const auto &dspOption : dspOrder) {
    tabBar.addTab(dspOption);
  }

  // Register listeners
  tabBar.addTabOrderListener(this);
  tabBar.addTabSelectionListener(this);

  // Set current tab
  auto savedTab = audioProcessor.getSelectedTabFromState();
  int savedTabIndex = 0;
  for (int i = 0; i < dspOrder.size(); ++i) {
    if (dspOrder[i] == savedTab) {
      savedTabIndex = i;
      break;
    }
  }
  tabBar.setCurrentTabIndex(savedTabIndex, true);

  addChildComponent(phaserPanel);
  addChildComponent(chorusPanel);
  addChildComponent(drivePanel);
  addChildComponent(ladderFilterPanel);
  addChildComponent(filterPanel);
  showDspPanel(savedTab);

  setSize(800, 450);
}

MultieffectpluginAudioProcessorEditor::
    ~MultieffectpluginAudioProcessorEditor() {
  tabBar.removeTabOrderListener(this);
  tabBar.removeTabSelectionListener(this);
  setLookAndFeel(nullptr);
}

void MultieffectpluginAudioProcessorEditor::tabOrderChanged(DspOrder newOrder) {
  audioProcessor.saveDspOrderToState(newOrder);
  audioProcessor.dspOrderFifo.push(newOrder);
}

void MultieffectpluginAudioProcessorEditor::tabSelectionChanged(
    int newSelectionIndex, DspOption dspOption) {
  showDspPanel(dspOption);
  audioProcessor.saveSelectedTabToState(dspOption);
}

void MultieffectpluginAudioProcessorEditor::showDspPanel(DspOption dspOption) {
  phaserPanel.setVisible(dspOption == DspOption::Phase);
  chorusPanel.setVisible(dspOption == DspOption::Chorus);
  drivePanel.setVisible(dspOption == DspOption::OverDrive);
  ladderFilterPanel.setVisible(dspOption == DspOption::LadderFilter);
  filterPanel.setVisible(dspOption == DspOption::Filter);
}

void MultieffectpluginAudioProcessorEditor::paint(juce::Graphics &g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MultieffectpluginAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds().reduced(10);
  input.setBounds(bounds.removeFromLeft(80));
  output.setBounds(bounds.removeFromRight(80));

  spectrumAnalyzer.setBounds(bounds.removeFromTop(275));
  tabBar.setBounds(bounds.removeFromTop(25));

  phaserPanel.setBounds(bounds);
  chorusPanel.setBounds(bounds);
  drivePanel.setBounds(bounds);
  ladderFilterPanel.setBounds(bounds);
  filterPanel.setBounds(bounds);
}
