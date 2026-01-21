#pragma once

#include <JuceHeader.h>

struct Parameter {
  const char *id;
  const char *displayName;
  const char *suffix;
  bool isPercent;
};

struct Parameters {
  struct Phaser {
    static constexpr Parameter rate = {"Phaser Rate", "Rate", " Hz", false};
    static constexpr Parameter depth = {"Phaser Depth", "Depth", "%", true};
    static constexpr Parameter centerFreq = {"Phaser Center Freq", "Center",
                                             " Hz", false};
    static constexpr Parameter feedback = {"Phaser Feedback", "Feedback", "%",
                                           true};
    static constexpr Parameter mix = {"Phaser Mix", "Mix", "%", true};
    static constexpr const char *bypass = "Phaser Bypass";

    static inline const std::vector<Parameter> sliderParams = {
        rate, depth, centerFreq, feedback, mix};
  };

  struct Chorus {
    static constexpr Parameter rate = {"Chorus Rate", "Rate", " Hz", false};
    static constexpr Parameter depth = {"Chorus Depth", "Depth", "%", true};
    static constexpr Parameter centerDelay = {"Chorus Center Delay", "Delay",
                                              " ms", false};
    static constexpr Parameter feedback = {"Chorus Feedback", "Feedback", "%",
                                           true};
    static constexpr Parameter mix = {"Chorus Mix", "Mix", "%", true};
    static constexpr const char *bypass = "Chorus Bypass";

    static inline const std::vector<Parameter> sliderParams = {
        rate, depth, centerDelay, feedback, mix};
  };

  struct Overdrive {
    static constexpr Parameter saturation = {"OverDrive", "Drive", "", false};
    static constexpr const char *bypass = "Overdrive Bypass";

    static inline const std::vector<Parameter> sliderParams = {saturation};
  };

  struct LadderFilter {
    static constexpr Parameter cutoff = {"Ladder Filter Cutoff", "Cutoff",
                                         " Hz", false};
    static constexpr Parameter resonance = {"Ladder Filter Resonance",
                                            "Resonance", "", false};
    static constexpr Parameter drive = {"Ladder Filter Drive", "Drive", "",
                                        false};
    static constexpr const char *mode = "Ladder Filter Mode";
    static constexpr const char *bypass = "Ladder Filter Bypass";

    static inline const juce::StringArray modeChoices{
        "LPF12", "HPF12", "BPF12", "LPF24", "HPF24", "BPF24"};

    static inline const std::vector<Parameter> sliderParams = {
        cutoff, resonance, drive};
  };

  struct Filter {
    static constexpr Parameter freq = {"Filter Freq", "Freq", " Hz", false};
    static constexpr Parameter quality = {"Filter Quality", "Q", "", false};
    static constexpr Parameter gain = {"Filter Gain", "Gain", " dB", false};
    static constexpr const char *mode = "Filter Mode";
    static constexpr const char *bypass = "Filter Bypass";

    static inline const juce::StringArray modeChoices{"Peak", "Bandpass",
                                                      "Notch", "Allpass"};

    static inline const std::vector<Parameter> sliderParams = {freq, quality,
                                                               gain};
  };
};
