#include "PluginEditor.h"
#include "PluginProcessor.h"

// HORIZONTAL CONSTRATINER
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

  // Notify listener of DSP order change
  MultieffectpluginAudioProcessor::DSP_Order newDspOrder;
  for (int i = 0; i < getNumTabs(); i++) {
    if (auto *tab = getTabButton(i)) {
      newDspOrder[i] = MultieffectpluginAudioProcessor::getDSPOptionFromName(
          tab->getButtonText());
    }
  }
  listeners.call(&TabOrderListener::tabOrderChanged, newDspOrder);
}

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

// EDITOR
//==============================================================================
MultieffectpluginAudioProcessorEditor::MultieffectpluginAudioProcessorEditor(
    MultieffectpluginAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {

  tabbedComponent.addListener(this);

  dspOrderButton.onClick = [this]() {
    juce::Random random;
    MultieffectpluginAudioProcessor::DSP_Order dspOrder;

    auto range = juce::Range<int>(
        static_cast<int>(MultieffectpluginAudioProcessor::DSP_Option::Phase),
        static_cast<int>(
            MultieffectpluginAudioProcessor::DSP_Option::END_OF_LIST));

    tabbedComponent.clearTabs();

    for (auto &dspOption : dspOrder) {
      auto entry = random.nextInt(range);
      dspOption =
          static_cast<MultieffectpluginAudioProcessor::DSP_Option>(entry);
      tabbedComponent.addTab(
          MultieffectpluginAudioProcessor::getDSPOptionName(dspOption),
          juce::Colours::white, -1);
    }

    audioProcessor.dspOrderFifo.push(dspOrder);
  };

  addAndMakeVisible(dspOrderButton);
  addAndMakeVisible(tabbedComponent);
  setSize(400, 300);
}

MultieffectpluginAudioProcessorEditor::
    ~MultieffectpluginAudioProcessorEditor() {
  tabbedComponent.removeListener(this);
}

void MultieffectpluginAudioProcessorEditor::tabOrderChanged(
    MultieffectpluginAudioProcessor::DSP_Order newOrder) {
  audioProcessor.dspOrderFifo.push(newOrder);
}

void MultieffectpluginAudioProcessorEditor::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background
  // with a solid colour)
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  g.setColour(juce::Colours::white);
  g.setFont(15.0f);
  g.drawFittedText("Hello World!", getLocalBounds(),
                   juce::Justification::centred, 1);
}

void MultieffectpluginAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds();
  dspOrderButton.setBounds(
      bounds.removeFromTop(30).withSizeKeepingCentre(150, 30));
  bounds.removeFromTop(10);
  tabbedComponent.setBounds(bounds.removeFromTop(30));
}
