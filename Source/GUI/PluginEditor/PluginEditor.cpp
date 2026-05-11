#include "PluginEditor.h"
#include "../../Processor/PluginProcessor/PluginProcessor.h"

// EDITOR
//==============================================================================
PluginEditor::PluginEditor(PluginProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p), spectrumAnalyzer(p),
      tabBar(p.parameters.apvts), phaserPanel(p.parameters.apvts),
      chorusPanel(p.parameters.apvts), drivePanel(p.parameters.apvts),
      ladderFilterPanel(p.parameters.apvts), filterPanel(p.parameters.apvts),
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

  tabBar.addTabOrderListener(this);
  tabBar.addTabSelectionListener(this);

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

PluginEditor::~PluginEditor() {
  tabBar.removeTabOrderListener(this);
  tabBar.removeTabSelectionListener(this);
  setLookAndFeel(nullptr);
}

void PluginEditor::tabOrderChanged(DspOrder newOrder) {
  audioProcessor.saveDspOrderToState(newOrder);
  audioProcessor.dspOrderFifo.push(newOrder);
}

void PluginEditor::tabSelectionChanged(int newSelectionIndex,
                                       DspOption dspOption) {
  showDspPanel(dspOption);
  audioProcessor.saveSelectedTabToState(dspOption);
}

void PluginEditor::showDspPanel(DspOption dspOption) {
  phaserPanel.setVisible(dspOption == DspOption::Phase);
  chorusPanel.setVisible(dspOption == DspOption::Chorus);
  drivePanel.setVisible(dspOption == DspOption::OverDrive);
  ladderFilterPanel.setVisible(dspOption == DspOption::LadderFilter);
  filterPanel.setVisible(dspOption == DspOption::Filter);
}

void PluginEditor::paint(juce::Graphics &g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  // Draw border around DSP panel area (tabs + effect controls)
  if (!dspPanelBounds.isEmpty()) {
    LookAndFeel::drawBorder(g, getLookAndFeel(), dspPanelBounds);
  }
}

void PluginEditor::resized() {
  auto bounds = getLocalBounds().reduced(10);

  input.setBounds(bounds.removeFromLeft(85));
  bounds.removeFromLeft(5);  // Gap
  output.setBounds(bounds.removeFromRight(85));
  bounds.removeFromRight(5);  // Gap

  spectrumAnalyzer.setBounds(bounds.removeFromTop(260));

  bounds.removeFromTop(3);

  dspPanelBounds = bounds;

  tabBar.setBounds(bounds.removeFromTop(28));

  phaserPanel.setBounds(bounds);
  chorusPanel.setBounds(bounds);
  drivePanel.setBounds(bounds);
  ladderFilterPanel.setBounds(bounds);
  filterPanel.setBounds(bounds);
}
