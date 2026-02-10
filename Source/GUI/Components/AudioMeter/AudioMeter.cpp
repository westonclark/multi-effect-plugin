#include "AudioMeter.h"
#include "../../LookAndFeel.h"

AudioMeter::AudioMeter(
    InputOutputLevelFifo<std::vector<float>> &inputOutputLevelFifo)
    : inputOutputLevelFifo(inputOutputLevelFifo) {
  startTimerHz(60);
}

void AudioMeter::paint(juce::Graphics &g) {
  auto bounds = getLocalBounds().reduced(4);
  const int meterWidth = (bounds.getWidth() - 4) / 2;

  auto leftBounds = bounds.removeFromLeft(meterWidth);
  auto rightBounds = bounds.removeFromRight(meterWidth);

  for (int channel = 0; channel < 2; ++channel) {
    auto meterBounds = (channel == 0) ? leftBounds : rightBounds;

    float levelDb =
        juce::Decibels::gainToDecibels(smoothedLevel[channel], MIN_DB);
    float normalizedLevel = juce::jmap(levelDb, MIN_DB, MAX_DB, 0.0f, 1.0f);

    const float segmentHeight =
        (float)meterBounds.getHeight() / (float)NUM_SEGMENTS;
    const float segmentGap = 1.0f;

    for (int i = 0; i < NUM_SEGMENTS; ++i) {
      float segmentBottom = meterBounds.getBottom() - (i + 1) * segmentHeight;
      juce::Rectangle<float> segmentRect(
          (float)meterBounds.getX(), segmentBottom,
          (float)meterBounds.getWidth(), segmentHeight - segmentGap);

      // Top segment (clip indicator) only lights at 0dB or above
      bool isLit = (i == NUM_SEGMENTS - 1)
                       ? (normalizedLevel >= 0.99f)
                       : ((float)i / (float)NUM_SEGMENTS <= normalizedLevel);

      if (isLit) {
        auto colour = (i == NUM_SEGMENTS - 1)
                          ? juce::Colour(LookAndFeel::METER_RED)
                          : juce::Colour(LookAndFeel::METER_GREEN);
        g.setColour(colour);
      } else {
        g.setColour(juce::Colour(LookAndFeel::BACKGROUND).brighter(0.15f));
      }

      g.fillRect(segmentRect);
    }
  }
}

void AudioMeter::resized() { repaint(); }

void AudioMeter::timerCallback() {
  std::vector<float> newLevel;
  while (inputOutputLevelFifo.pull(newLevel)) {
    rawLevel = newLevel;
  }

  for (int channel = 0; channel < 2; ++channel) {
    if (rawLevel[channel] > smoothedLevel[channel]) {
      // ATTACK: Audio is getting louder - respond quickly
      smoothedLevel[channel] =
          rawLevel[channel] * ATTACK + smoothedLevel[channel] * (1.0f - ATTACK);
    } else {
      // RELEASE: Audio is getting quieter - respond slowly
      smoothedLevel[channel] = rawLevel[channel] * (1.0f - RELEASE) +
                               smoothedLevel[channel] * RELEASE;
    }
  }

  repaint();
}
