#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "juce_audio_processors/juce_audio_processors.h"

// BUTTON BAR HORIZONTAL CONSTRATINER
//==============================================================================
HorizontalConstrainer::HorizontalConstrainer(
    std::function<juce::Rectangle<int>()> confinerBoundsGetter,
    std::function<juce::Rectangle<int>()> confineeBoundsGetter)
    : boundsToConfineToGetter(std::move(confinerBoundsGetter)),
      boundsOfConfineeGetter(std::move(confineeBoundsGetter)) {

      };

void HorizontalConstrainer::checkBounds(
    juce::Rectangle<int> &bounds, const juce::Rectangle<int> &previousBounds,
    const juce::Rectangle<int> &limits, bool isStretchingTop,
    bool isStretchingLeft, bool isStretchingBottom, bool isStretchingRight) {

  bounds.setY(previousBounds.getY());

  if (boundsToConfineToGetter != nullptr && boundsOfConfineeGetter != nullptr) {
    auto boundsToConfineTo = boundsToConfineToGetter();
    auto boundsOfConfinee = boundsOfConfineeGetter();
    bounds.setX(
        juce::jlimit(boundsToConfineTo.getX(),
                     boundsToConfineTo.getRight() - boundsOfConfinee.getWidth(),
                     bounds.getX()));
  } else {
    bounds.setX(juce::jlimit(limits.getX(), limits.getY(), bounds.getX()));
  }
};

// BUTTON BAR
//==============================================================================
ExtendedTabbedButtonBar::ExtendedTabbedButtonBar()
    : juce::TabbedButtonBar(juce::TabbedButtonBar::Orientation::TabsAtTop) {};

void ExtendedTabbedButtonBar::tabDragStarted(ExtendedTabBarButton *button) {}

void ExtendedTabbedButtonBar::tabDragMoved(ExtendedTabBarButton *button) {
  int draggedTabIndex = -1;
  for (int i = 0; i < getNumTabs(); ++i) {
    if (getTabButton(i) == button) {
      draggedTabIndex = i;
      break;
    }
  }

  if (draggedTabIndex == -1)
    return;
  int draggedCenterX = button->getBounds().getCentreX();

  for (int i = 0; i < getNumTabs(); ++i) {
    if (i == draggedTabIndex)
      continue;

    auto *targetTab = getTabButton(i);
    int targetCenterX = targetTab->getBounds().getCentreX();

    bool shouldSwap = false;
    if (i < draggedTabIndex) {
      shouldSwap = draggedCenterX < targetCenterX;
    } else {
      shouldSwap = draggedCenterX > targetCenterX;
    }

    if (shouldSwap) {
      moveTab(draggedTabIndex, i);
      break;
    }
  }
}

void ExtendedTabbedButtonBar::tabDragEnded(ExtendedTabBarButton *button) {
  finalizeTabOrder();
}

void ExtendedTabbedButtonBar::finalizeTabOrder() {
  resized();

  MultieffectpluginAudioProcessor::DSP_Order newDspOrder;
  for (int i = 0; i < getNumTabs(); i++) {
    if (auto *tab = getTabButton(i)) {
      newDspOrder[i] = MultieffectpluginAudioProcessor::getDSPOptionFromName(
          tab->getButtonText());
    }
  }
  tabOrderListener.call(&TabOrderListener::tabOrderChanged, newDspOrder);
}

void ExtendedTabbedButtonBar::currentTabChanged(int newSelectionIndex,
                                                const juce::String &dspName) {
  auto dspOption =
      MultieffectpluginAudioProcessor::getDSPOptionFromName(dspName);
  tabSelectionListener.call(&TabSelectionListener::TabSelectionChanged,
                            newSelectionIndex, dspOption);
};

// BUTTON
//==============================================================================
ExtendedTabBarButton::ExtendedTabBarButton(const juce::String &name,
                                           juce::TabbedButtonBar &owner)
    : juce::TabBarButton(name, owner) {
  constrainer = std::make_unique<HorizontalConstrainer>(
      [&owner]() { return owner.getLocalBounds(); },
      [this]() { return getBounds(); });

  constrainer->setMinimumOnscreenAmounts(0xffffffff, 0xffffffff, 0xffffffff,
                                         0xffffffff);
};

void ExtendedTabBarButton::mouseDown(const juce::MouseEvent &e) {
  toFront(true);
  dragger.startDraggingComponent(this, e);
  if (listener) {
    listener->tabDragStarted(this);
  }
  juce::TabBarButton::mouseDown(e);
}

void ExtendedTabBarButton::mouseDrag(const juce::MouseEvent &e) {
  toFront(true);
  dragger.dragComponent(this, e, constrainer.get());
  if (listener) {
    listener->tabDragMoved(this);
  }
}

void ExtendedTabBarButton::mouseUp(const juce::MouseEvent &e) {
  if (listener) {
    listener->tabDragEnded(this);
  }
  juce::TabBarButton::mouseUp(e);
}

int ExtendedTabBarButton::getBestTabLength(int depth) {
  auto bestWidth = getLookAndFeel().getTabButtonBestWidth(*this, depth);
  auto &bar = getTabbedButtonBar();
  // Choose the larger of: the best width for text, or equal division of bar
  // width
  return juce::jmax(bestWidth, bar.getWidth() / bar.getNumTabs());
}

juce::TabBarButton *
ExtendedTabbedButtonBar::createTabButton(const juce::String &tabName,
                                         int tabIndex) {
  auto *button = new ExtendedTabBarButton(tabName, *this);
  button->setButtonEventListener(this);
  return button;
};

// PARAMETER VIEW CONTAINTER
//==============================================================================
ParameterViewContainer::ParameterViewContainer(
    juce::AudioProcessorValueTreeState &apvts)
    : apvts(apvts), currentlyDisplayed() {};

void ParameterViewContainer::paint(juce::Graphics &g) {
  g.fillAll(juce::Colours::darkgrey);

  g.setColour(juce::Colours::white);
  g.setFont(20.0f);

  auto dspName =
      MultieffectpluginAudioProcessor::getDSPNameFromOption(currentlyDisplayed);
  g.drawText(dspName, getLocalBounds(), juce::Justification::centred);
}

void ParameterViewContainer::showPanelFor(
    MultieffectpluginAudioProcessor::DSP_Option tab) {
  currentlyDisplayed = tab;
  repaint();
}

// EDITOR
//==============================================================================
MultieffectpluginAudioProcessorEditor::MultieffectpluginAudioProcessorEditor(
    MultieffectpluginAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      parametersComponent(p.apvts) {

  // Load DSP order and populate tabs
  auto dspOrder = audioProcessor.getDspOrderFromState();
  for (const auto &dspOption : dspOrder) {
    tabBarComponent.addTab(
        MultieffectpluginAudioProcessor::getDSPNameFromOption(dspOption),
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
  addAndMakeVisible(parametersComponent);
  setSize(400, 300);
}

MultieffectpluginAudioProcessorEditor::
    ~MultieffectpluginAudioProcessorEditor() {
  tabBarComponent.removeTabOrderListener(this);
  tabBarComponent.removeTabSelectionListener(this);
}

void MultieffectpluginAudioProcessorEditor::tabOrderChanged(
    MultieffectpluginAudioProcessor::DSP_Order newOrder) {
  audioProcessor.saveDspOrderToState(newOrder);
  audioProcessor.dspOrderFifo.push(newOrder);
}

void MultieffectpluginAudioProcessorEditor::TabSelectionChanged(
    int newSelectionIndex,
    MultieffectpluginAudioProcessor::DSP_Option dspOption) {
  parametersComponent.showPanelFor(dspOption);
  audioProcessor.saveSelectedTabToState(dspOption);
}

void MultieffectpluginAudioProcessorEditor::paint(juce::Graphics &g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MultieffectpluginAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds();
  tabBarComponent.setBounds(bounds.removeFromTop(30));
  parametersComponent.setBounds(bounds);
}
