#include "Sliders.h"
#include "../../Parameters.h"

SliderWithLabel::SliderWithLabel(const Parameter &param,
                                 juce::AudioProcessorValueTreeState &apvts,
                                 juce::Component *parent) {
  if (param.isPercent) {
    slider = std::make_unique<PercentSlider>();
  } else {
    slider = std::make_unique<juce::Slider>();
  }

  slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
  slider->setColour(juce::Slider::textBoxOutlineColourId,
                    juce::Colours::transparentBlack);
  slider->setTextValueSuffix(param.suffix);

  attachment =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          apvts, param.id, *slider);

  slider->updateText();

  label = std::make_unique<juce::Label>();
  label->setText(param.displayName, juce::dontSendNotification);
  label->setJustificationType(juce::Justification::centred);

  parent->addAndMakeVisible(*slider);
  parent->addAndMakeVisible(*label);
}

void SliderWithLabel::layoutHorizontally(
    juce::Rectangle<int> bounds, std::vector<SliderWithLabel> &sliders) {
  int sliderWidth = bounds.getWidth() / static_cast<int>(sliders.size());
  int labelHeight = 20;
  int topPadding = 10;
  int bottomPadding = 10;

  for (auto &s : sliders) {
    auto sliderBounds = bounds.removeFromLeft(sliderWidth);
    sliderBounds.removeFromTop(topPadding);
    sliderBounds.removeFromBottom(bottomPadding);
    s.label->setBounds(sliderBounds.removeFromTop(labelHeight));
    s.slider->setBounds(sliderBounds);
  }
}
