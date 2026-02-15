#include "SpectrumAnalyzer.h"
#include "../../../Processor/PluginProcessor/PluginProcessor.h"
#include "../../LookAndFeel.h"

SpectrumAnalyzer::SpectrumAnalyzer(PluginProcessor &audioProcessor)
    : audioProcessor(audioProcessor) {
  startTimerHz(60);
};

void SpectrumAnalyzer::paint(juce::Graphics &g) {
  auto bounds = getLocalBounds();

  // Border
  LookAndFeel::drawBorder(g, getLookAndFeel(), bounds);

  // Filter Curve
  SpectrumAnalyzer::drawFilterCurve(g, bounds);
}

void SpectrumAnalyzer::resized() { repaint(); };

void SpectrumAnalyzer::timerCallback() { repaint(); };

void SpectrumAnalyzer::drawFilterCurve(juce::Graphics &g,
                                       juce::Rectangle<int> bounds) {

  auto sampleRate = audioProcessor.getSampleRate();
  auto coefficients = audioProcessor.dsp.getFilterCoefficients();
  auto isBypassed = audioProcessor.parameters.filterBypass->get();

  juce::Path responseCurve;
  float width = bounds.getWidth();
  auto height = bounds.getHeight();

  for (int x = 0; x < width; ++x) {
    auto normalizedX = x / width;
    auto currentFreq = MIN_FREQ * std::pow(MAX_FREQ / MIN_FREQ, normalizedX);

    auto magnitude =
        coefficients->getMagnitudeForFrequency(currentFreq, sampleRate);
    float magnitudeDb = juce::Decibels::gainToDecibels(magnitude);

    auto normalizedY = juce::jmap(magnitudeDb, MIN_DB, MAX_DB, 1.0f, 0.0f);
    auto y = bounds.getY() + normalizedY * height;

    if (x == 0) {
      responseCurve.startNewSubPath(x, y);
    } else {
      responseCurve.lineTo(x, y);
    }
  }

  // Draw the curve
  auto activeColor = juce::Colour(LookAndFeel::HIGHLIGHT);
  auto color =
      isBypassed ? LookAndFeel::getBypassedColour(activeColor) : activeColor;
  g.setColour(color);
  g.strokePath(responseCurve, juce::PathStrokeType(2.0f));
};
