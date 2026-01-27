#pragma once

#include <JuceHeader.h>

enum class ParameterType { Float, Choice, Bool };

struct Parameter {
  const char *id;
  const char *displayName;
  const char *suffix = "";
  ParameterType type;
  float minValue = 0.f;
  float maxValue = 1.f;
  float defaultValue = 0.f;
  float step = 0.01f;
  float skew = 1.f;
  const juce::StringArray *choices = nullptr;
};

struct Parameters {
  struct Phaser {
    static constexpr Parameter rate = {.id = "Phaser Rate",
                                       .displayName = "Rate",
                                       .suffix = " Hz",
                                       .type = ParameterType ::Float,
                                       .minValue = 0.01f,
                                       .maxValue = 2.f,
                                       .defaultValue = 1.f};

    static constexpr Parameter depth = {.id = "Phaser Depth",
                                        .displayName = "Depth",
                                        .suffix = "%",
                                        .type = ParameterType ::Float,
                                        .defaultValue = 0.05f};

    static constexpr Parameter centerFreq = {.id = "Phaser Center Freq",
                                             .displayName = "Center",
                                             .suffix = " Hz",
                                             .type = ParameterType ::Float,
                                             .minValue = 20.f,
                                             .maxValue = 20000.f,
                                             .defaultValue = 1000.f,
                                             .step = 1.f,
                                             .skew = 0.23f};

    static constexpr Parameter feedback = {.id = "Phaser Feedback",
                                           .displayName = "Feedback",
                                           .suffix = "%",
                                           .type = ParameterType ::Float,
                                           .minValue = -1.f,
                                           .defaultValue = 0.f};

    static constexpr Parameter mix = {.id = "Phaser Mix",
                                      .displayName = "Mix",
                                      .suffix = "%",
                                      .type = ParameterType ::Float,
                                      .defaultValue = 0.05f};

    static constexpr Parameter bypass = {.id = "Phaser Bypass",
                                         .displayName = "Bypass",
                                         .suffix = "",
                                         .type = ParameterType ::Bool};

    static inline const std::vector<Parameter> params = {
        rate, depth, centerFreq, feedback, mix, bypass};
  };

  struct Chorus {
    static constexpr Parameter rate = {.id = "Chorus Rate",
                                       .displayName = "Rate",
                                       .suffix = " Hz",
                                       .type = ParameterType ::Float,
                                       .minValue = 0.01f,
                                       .maxValue = 2.f,
                                       .defaultValue = 0.2f};

    static constexpr Parameter depth = {.id = "Chorus Depth",
                                        .displayName = "Depth",
                                        .suffix = "%",
                                        .type = ParameterType ::Float,
                                        .defaultValue = 0.05f};
    static constexpr Parameter centerDelay = {.id = "Chorus Center Delay",
                                              .displayName = "Delay",
                                              .suffix = " ms",
                                              .type = ParameterType ::Float,
                                              .maxValue = 100.f,
                                              .defaultValue = 7.f,
                                              .step = 1.f};

    static constexpr Parameter feedback = {.id = "Chorus Feedback",
                                           .displayName = "Feedback",
                                           .suffix = "%",
                                           .type = ParameterType ::Float,
                                           .minValue = -1.f,
                                           .defaultValue = 0.f};

    static constexpr Parameter mix = {.id = "Chorus Mix",
                                      .displayName = "Mix",
                                      .suffix = "%",
                                      .type = ParameterType ::Float,
                                      .defaultValue = 0.05f};

    static constexpr Parameter bypass = {.id = "Chorus Bypass",
                                         .displayName = "Bypass",
                                         .suffix = "",
                                         .type = ParameterType ::Bool};

    static inline const std::vector<Parameter> params = {
        rate, depth, centerDelay, feedback, mix, bypass};
  };

  struct Overdrive {
    static constexpr Parameter saturation = {.id = "OverDrive",
                                             .displayName = "Drive",
                                             .suffix = "",
                                             .type = ParameterType ::Float,
                                             .minValue = 1.f,
                                             .maxValue = 100.f,
                                             .defaultValue = 1.f,
                                             .step = 0.1f};

    static constexpr Parameter bypass = {.id = "Overdrive Bypass",
                                         .displayName = "Bypass",
                                         .suffix = "",
                                         .type = ParameterType ::Bool};

    static inline const std::vector<Parameter> params = {saturation, bypass};
  };

  struct LadderFilter {
    static inline const juce::StringArray modes{"LPF12", "HPF12", "BPF12",
                                                "LPF24", "HPF24", "BPF24"};

    static inline const Parameter mode = {.id = "Ladder Filter Mode",
                                          .displayName = "Mode",
                                          .suffix = "",
                                          .type = ParameterType ::Choice,
                                          .choices = &modes};

    static constexpr Parameter cutoff = {.id = "Ladder Filter Cutoff",
                                         .displayName = "Cutoff",
                                         .suffix = " Hz",
                                         .type = ParameterType ::Float,
                                         .minValue = 20.f,
                                         .maxValue = 20000.f,
                                         .defaultValue = 20000.f,
                                         .step = 0.1f,
                                         .skew = 0.23f};

    static constexpr Parameter resonance = {.id = "Ladder Filter Resonance",
                                            .displayName = "Resonance",
                                            .suffix = "",
                                            .type = ParameterType ::Float,
                                            .step = 0.1f};

    static constexpr Parameter drive = {.id = "Ladder Filter Drive",
                                        .displayName = "Drive",
                                        .suffix = "",
                                        .type = ParameterType ::Float,
                                        .minValue = 1.f,
                                        .maxValue = 100.f,
                                        .defaultValue = 1.f,
                                        .step = 0.1f};

    static constexpr Parameter bypass = {.id = "Ladder Filter Bypass",
                                         .displayName = "Bypass",
                                         .suffix = "",
                                         .type = ParameterType ::Bool};

    static inline const std::vector<Parameter> params = {
        mode, cutoff, resonance, drive, bypass};
  };

  struct Filter {
    static inline const juce::StringArray modes{"Peak", "Bandpass", "Notch",
                                                "Allpass"};

    static inline const Parameter mode = {.id = "Filter Mode",
                                          .displayName = "Mode",
                                          .suffix = "",
                                          .type = ParameterType ::Choice,
                                          .choices = &modes};

    static constexpr Parameter freq = {.id = "Filter Freq",
                                       .displayName = "Freq",
                                       .suffix = " Hz",
                                       .type = ParameterType ::Float,
                                       .minValue = 20.f,
                                       .maxValue = 20000.f,
                                       .defaultValue = 1000.f,
                                       .step = 1.f,
                                       .skew = 0.23f};

    static constexpr Parameter quality = {.id = "Filter Quality",
                                          .displayName = "Q",
                                          .suffix = "",
                                          .type = ParameterType ::Float,
                                          .minValue = 0.1f,
                                          .maxValue = 10.f,
                                          .defaultValue = 5.f,
                                          .step = 0.05f};

    static constexpr Parameter gain = {.id = "Filter Gain",
                                       .displayName = "Gain",
                                       .suffix = " dB",
                                       .type = ParameterType ::Float,
                                       .minValue = -24.f,
                                       .maxValue = 24.f,
                                       .step = 0.5f};

    static constexpr Parameter bypass = {.id = "Filter Bypass",
                                         .displayName = "Bypass",
                                         .suffix = "",
                                         .type = ParameterType ::Bool};

    static inline const std::vector<Parameter> params = {mode, freq, quality,
                                                         gain, bypass};
  };

  static inline std::vector<Parameter> getAllParameters() {
    std::vector<Parameter> allParameters;
    for (const auto &p : Phaser::params)
      allParameters.push_back(p);
    for (const auto &p : Chorus::params)
      allParameters.push_back(p);
    for (const auto &p : Overdrive::params)
      allParameters.push_back(p);
    for (const auto &p : LadderFilter::params)
      allParameters.push_back(p);
    for (const auto &p : Filter::params)
      allParameters.push_back(p);
    return allParameters;
  }
};
