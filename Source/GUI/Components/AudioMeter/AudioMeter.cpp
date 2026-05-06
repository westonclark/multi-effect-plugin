#include "AudioMeter.h"
#include "../../LookAndFeel.h"
#include "../../../Processor/PluginProcessor/PluginProcessor.h"

AudioMeter::AudioMeter(
    AudioMeterFifo<MeterLevel> &inputOutputLevelFifo)
    : inputOutputLevelFifo(inputOutputLevelFifo) {
  startTimerHz(60);
}

void AudioMeter::paint(juce::Graphics &g) {
  float levels[2] = {smoothedLevel.left, smoothedLevel.right};
  for (int channel = 0; channel < 2; ++channel) {
    const float levelDb =
        juce::Decibels::gainToDecibels(levels[channel], MIN_DB);
    const float normalizedLevel =
        juce::jmap(levelDb, MIN_DB, MAX_DB, 0.0f, 1.0f);

    // Green segments
    for (int i = 0; i < NUM_SEGMENTS - 1; ++i) {
      if ((float)i / NUM_SEGMENTS <= normalizedLevel) {
        g.setColour(juce::Colour(LookAndFeel::METER_GREEN));
      } else {
        g.setColour(juce::Colour(LookAndFeel::METER_OFF));
      }
      g.fillRect(segments[channel][i]);
    }

    // Clip indicator
    if (levels[channel] > 0.99f) {
      g.setColour(juce::Colour(LookAndFeel::METER_RED));
    } else {
      g.setColour(juce::Colour(LookAndFeel::METER_OFF));
    }
    g.fillRect(segments[channel][NUM_SEGMENTS - 1]);
  }
}

void AudioMeter::resized() {
  auto bounds = getLocalBounds();

  // Meters
  const float meterWidth = juce::jmin(12, bounds.getWidth() / 3);
  const int meterGap = 6;
  const int totalMeterWidth = (meterWidth * 2) + meterGap;
  const float segmentGap = 1.0f;

  const int horizontalPadding = (bounds.getWidth() - totalMeterWidth) / 2;
  bounds.removeFromLeft(horizontalPadding);
  bounds.removeFromRight(horizontalPadding);
  bounds = bounds.reduced(0, 4);

  auto leftBounds = bounds.removeFromLeft(meterWidth);
  auto rightBounds = bounds.removeFromRight(meterWidth);
  juce::Rectangle<int> meterBounds[2] = {leftBounds, rightBounds};

  // Meter segments
  for (int channel = 0; channel < 2; ++channel) {
    const auto &meter = meterBounds[channel];
    const float meterX = (float)meter.getX();
    const float meterBottom = (float)meter.getBottom();
    const float segmentHeight = (float)meter.getHeight() / NUM_SEGMENTS;

    for (int i = 0; i < NUM_SEGMENTS; ++i) {
      const float segmentY = meterBottom - (i + 1) * segmentHeight;
      segments[channel][i] = juce::Rectangle<float>(
          meterX, segmentY, meterWidth, segmentHeight - segmentGap);
    }
  }
  repaint();
}

void AudioMeter::timerCallback() {
  MeterLevel newLevel;
  while (inputOutputLevelFifo.pull(newLevel)) {
    rawLevel = newLevel;
  }

  // Attack/release smoothing
  if (rawLevel.left > smoothedLevel.left) {
    smoothedLevel.left =
        rawLevel.left * ATTACK + smoothedLevel.left * (1.0f - ATTACK);
  } else {
    smoothedLevel.left = rawLevel.left * (1.0f - RELEASE) +
                         smoothedLevel.left * RELEASE;
  }

  if (rawLevel.right > smoothedLevel.right) {
    smoothedLevel.right =
        rawLevel.right * ATTACK + smoothedLevel.right * (1.0f - ATTACK);
  } else {
    smoothedLevel.right = rawLevel.right * (1.0f - RELEASE) +
                          smoothedLevel.right * RELEASE;
  }

  repaint();
}
