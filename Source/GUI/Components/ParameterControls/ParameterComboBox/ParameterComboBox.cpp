#include "ParameterComboBox.h"

ParameterComboBox::ParameterComboBox(const Parameter &parameter,
                                     juce::AudioProcessorValueTreeState &apvts,
                                     juce::Component *component)
    : parameter(parameter), component(component) {

  comboBox = std::make_unique<juce::ComboBox>();
  comboBox->addItemList(*parameter.choices, 1);

  comboBoxAttachment =
      std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
          apvts, parameter.id, *comboBox);

  component->addAndMakeVisible(*comboBox);

  label = std::make_unique<juce::Label>();
  label->setText(parameter.displayName, juce::dontSendNotification);
  label->setJustificationType(juce::Justification::centred);
  component->addAndMakeVisible(*label);
}

void ParameterComboBox::setBounds(juce::Rectangle<int> bounds) {
  bounds.removeFromTop(10);
  bounds.removeFromBottom(10);
  label->setBounds(bounds.removeFromTop(20));

  int comboWidth = juce::jmin(bounds.getWidth() - 10, 100);
  auto comboBounds = bounds.withSizeKeepingCentre(comboWidth, 24);
  comboBox->setBounds(comboBounds);
}
