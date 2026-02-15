#include "AudioMeter.h"
#include "../../LookAndFeel.h"

AudioMeter::AudioMeter(
    InputOutputLevelFifo<std::vector<float>> &inputOutputLevelFifo)
    : inputOutputLevelFifo(inputOutputLevelFifo) {
  startTimerHz(60);
}

void AudioMeter::paint(juce::Graphics &g) {
  for (int channel = 0; channel < 2; ++channel) {
    const float levelDb =
        juce::Decibels::gainToDecibels(smoothedLevel[channel], MIN_DB);
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
    if (smoothedLevel[channel] > 0.99f) {
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
  std::vector<float> newLevel;
  while (inputOutputLevelFifo.pull(newLevel)) {
    rawLevel = newLevel;
  }

  // Attack/release smoothing
  for (int channel = 0; channel < 2; ++channel) {
    if (rawLevel[channel] > smoothedLevel[channel]) {
      smoothedLevel[channel] =
          rawLevel[channel] * ATTACK + smoothedLevel[channel] * (1.0f - ATTACK);
    } else {
      smoothedLevel[channel] = rawLevel[channel] * (1.0f - RELEASE) +
                               smoothedLevel[channel] * RELEASE;
    }
  }

  repaint();
}
