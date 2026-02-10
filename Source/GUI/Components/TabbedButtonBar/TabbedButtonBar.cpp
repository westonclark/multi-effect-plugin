#include "TabbedButtonBar.h"
#include "../../../Processor/PluginProcessor/PluginProcessor.h"
#include "PowerButton.h"

// HORIZONTAL CONSTRAINER
//==============================================================================
HorizontalConstrainer::HorizontalConstrainer(
    std::function<juce::Rectangle<int>()> confinerBoundsGetter,
    std::function<juce::Rectangle<int>()> confineeBoundsGetter)
    : boundsToConfineToGetter(std::move(confinerBoundsGetter)),
      boundsOfConfineeGetter(std::move(confineeBoundsGetter)) {}

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
}

// BUTTON
//==============================================================================
ExtendedTabBarButton::ExtendedTabBarButton(
    const juce::String &name, juce::TabbedButtonBar &owner,
    juce::AudioProcessorValueTreeState *apvts, DspOption dspOption)
    : juce::TabBarButton(name, owner), dspOption(dspOption) {
  constrainer = std::make_unique<HorizontalConstrainer>(
      [&owner]() { return owner.getLocalBounds(); },
      [this]() { return getBounds(); });

  constrainer->setMinimumOnscreenAmounts(0xffffffff, 0xffffffff, 0xffffffff,
                                         0xffffffff);

  auto &bypassParam = getBypassParam(dspOption);
  auto *powerButton = new PowerButton();
  bypassAttachment =
      std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
          *apvts, bypassParam.id, *powerButton);

  setExtraComponent(powerButton, juce::TabBarButton::beforeText);
}

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
  return juce::jmax(bestWidth, bar.getWidth() / bar.getNumTabs());
}

// TABBED BUTTON BAR
//==============================================================================
ExtendedTabbedButtonBar::ExtendedTabbedButtonBar(
    juce::AudioProcessorValueTreeState &apvts)
    : juce::TabbedButtonBar(juce::TabbedButtonBar::Orientation::TabsAtTop),
      apvts(apvts) {}

void ExtendedTabbedButtonBar::addTab(DspOption option, int insertIndex) {
  auto tabColour =
      findColour(juce::ResizableWindow::backgroundColourId).brighter(0.1f);
  juce::TabbedButtonBar::addTab(
      PluginProcessor::getDspNameFromOption(option), tabColour,
      insertIndex);
}

juce::TabBarButton *
ExtendedTabbedButtonBar::createTabButton(const juce::String &tabName,
                                         int tabIndex) {
  auto dspOption =
      PluginProcessor::getDspOptionFromName(tabName);
  auto *button = new ExtendedTabBarButton(tabName, *this, &apvts, dspOption);
  button->setButtonEventListener(this);
  return button;
}

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
  resized();

  DspOrder newDspOrder;
  for (int i = 0; i < getNumTabs(); i++) {
    auto *tab = static_cast<ExtendedTabBarButton *>(getTabButton(i));
    newDspOrder[i] = tab->dspOption;
  }
  tabOrderListener.call(&TabOrderListener::tabOrderChanged, newDspOrder);
}

void ExtendedTabbedButtonBar::currentTabChanged(int newSelectionIndex,
                                                const juce::String &dspName) {
  // Update background colors for all tabs based on selection state
  auto baseColour = findColour(juce::ResizableWindow::backgroundColourId);
  auto unselectedColour = baseColour.brighter(0.05f);
  for (int i = 0; i < getNumTabs(); ++i) {
    setTabBackgroundColour(i, i == newSelectionIndex ? baseColour
                                                     : unselectedColour);
  }

  auto *button =
      static_cast<ExtendedTabBarButton *>(getTabButton(newSelectionIndex));
  auto dspOption = button->dspOption;
  tabSelectionListener.call(&TabSelectionListener::tabSelectionChanged,
                            newSelectionIndex, dspOption);
}
