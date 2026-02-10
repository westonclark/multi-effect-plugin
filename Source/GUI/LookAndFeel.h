#pragma once

#include <JuceHeader.h>

class LookAndFeel : public juce::LookAndFeel_V4 {
public:
  static constexpr juce::uint32 BACKGROUND = 0xff09090b; // zinc-950
  static constexpr juce::uint32 TEXT = 0xfffef3c7;       // amber-50
  static constexpr juce::uint32 HIGHLIGHT = 0xffd97757;  // orange accent
  static constexpr juce::uint32 METER_GREEN = 0xff22c55e; // green-500
  static constexpr juce::uint32 METER_RED = 0xffef4444;   // red-500 (clip indicator)

  LookAndFeel() {
    auto scheme = LookAndFeel_V4::ColourScheme(
        juce::Colour(BACKGROUND),                 // windowBackground
        juce::Colour(BACKGROUND).brighter(0.1f),  // widgetBackground
        juce::Colour(BACKGROUND).brighter(0.05f), // menuBackground
        juce::Colour(TEXT).withAlpha(0.3f),       // outline
        juce::Colour(TEXT),                       // defaultText
        juce::Colour(HIGHLIGHT),                  // defaultFill
        juce::Colour(BACKGROUND),                 // highlightedText
        juce::Colour(HIGHLIGHT),                  // highlightedFill
        juce::Colour(TEXT)                        // menuText
    );
    setColourScheme(scheme);
  }

  static juce::Colour getBypassedColour(juce::Colour baseColour) {
    return baseColour.withMultipliedSaturation(0.3f).withMultipliedBrightness(
        0.6f);
  }
};
