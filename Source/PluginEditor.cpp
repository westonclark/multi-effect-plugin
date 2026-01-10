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

bool ExtendedTabbedButtonBar::isInterestedInDragSource(
    const SourceDetails &dragSourceDetails) {

  auto *isATabBarButton = dynamic_cast<ExtendedTabBarButton *>(
      dragSourceDetails.sourceComponent.get());
  if (isATabBarButton) {
    return true;
  }

  return false;
}

void ExtendedTabbedButtonBar::itemDragEnter(
    const SourceDetails &dragSourceDetails) {
  juce::DragAndDropTarget::itemDragEnter(dragSourceDetails);
};

void ExtendedTabbedButtonBar::itemDragMove(
    const SourceDetails &dragSourceDetails) {
  // Get the dragged tab component
  auto *tabButtonBeingDragged = dynamic_cast<ExtendedTabBarButton *>(
      dragSourceDetails.sourceComponent.get());
  if (!tabButtonBeingDragged) {
    return;
  }

  // Find the index of the dragged tab
  int draggedTabIndex = -1;
  for (int i = 0; i < getNumTabs(); ++i) {
    if (getTabButton(i) == tabButtonBeingDragged) {
      draggedTabIndex = i;
      break;
    }
  }

  // Get the dragged tab position and decide when to swap
  auto dragPosition = dragSourceDetails.localPosition;
  for (int i = 0; i < getNumTabs(); ++i) {
    if (i == draggedTabIndex)
      continue;

    auto *targetTab = getTabButton(i);
    auto targetTabBounds = targetTab->getBounds();

    // If we are hovering over another tab
    if (targetTabBounds.contains(dragPosition)) {
      int targetMidpoint = targetTabBounds.getCentreX();

      // Swap once we pass the midpoint
      bool shouldSwap = false;
      if (i < draggedTabIndex) {
        shouldSwap = dragPosition.x < targetMidpoint;
      } else {
        shouldSwap = dragPosition.x > targetMidpoint;
      }

      if (shouldSwap) {
        moveTab(draggedTabIndex, i);
        break;
      }
    }
  }
};

void ExtendedTabbedButtonBar::itemDragExit(
    const SourceDetails &dragSourceDetails) {
  juce::DragAndDropTarget::itemDragExit(dragSourceDetails);
  resized();
};

void ExtendedTabbedButtonBar::itemDropped(
    const SourceDetails &dragSourceDetails) {
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
};

void ExtendedTabbedButtonBar::mouseDown(const juce::MouseEvent &e) {
  if (auto *tabButtonBeingDragged =
          dynamic_cast<ExtendedTabBarButton *>(e.originalComponent)) {

    // A 1x1 transparent image to hide the drag image
    juce::Image transparentImage(juce::Image::ARGB, 1, 1, true);
    transparentImage.clear(transparentImage.getBounds(),
                           juce::Colours::transparentBlack);
    DragAndDropContainer::startDragging(tabButtonBeingDragged->getButtonText(),
                                        tabButtonBeingDragged,
                                        juce::ScaledImage(transparentImage));
  }
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
  juce::TabBarButton::mouseDown(e);
}

void ExtendedTabBarButton::mouseDrag(const juce::MouseEvent &e) {
  toFront(true);
  dragger.dragComponent(this, e, constrainer.get());
}

juce::TabBarButton *
ExtendedTabbedButtonBar::createTabButton(const juce::String &tabName,
                                         int tabIndex) {
  auto *button = new ExtendedTabBarButton(tabName, *this);
  button->addMouseListener(this, false);
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
