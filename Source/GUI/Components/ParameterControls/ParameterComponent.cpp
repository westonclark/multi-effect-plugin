#include "ParameterComponent.h"
#include "ParameterComboBox/ParameterComboBox.h"
#include "ParameterSlider/ParameterSlider.h"
#include "ParameterToggle/ParameterToggle.h"

std::unique_ptr<ParameterComponent>
ParameterComponent::create(const Parameter &parameter,
                           juce::AudioProcessorValueTreeState &apvts,
                           juce::Component *component) {
  switch (parameter.type) {
  case ParameterType::Float:
    return std::make_unique<ParameterSlider>(parameter, apvts, component);
  case ParameterType::Choice:
    return std::make_unique<ParameterComboBox>(parameter, apvts, component);
  case ParameterType::Bool:
    return std::make_unique<ParameterToggle>(parameter, apvts, component);
  default:
    return nullptr;
  }
}

void ParameterComponent::layoutHorizontally(
    juce::Rectangle<int> bounds,
    std::vector<std::unique_ptr<ParameterComponent>> &components) {
  if (components.empty())
    return;

  int componentWidth = bounds.getWidth() / static_cast<int>(components.size());

  for (auto &component : components) {
    auto componentBounds = bounds.removeFromLeft(componentWidth);
    component->setBounds(componentBounds);
  }
}
