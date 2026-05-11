#include "Parameters.h"

Parameters::Parameters(juce::AudioProcessor &processor)
    : apvts(processor, nullptr, "Parameters", createParameterLayout()) {

  // Float parameters
  struct FloatParameter {
    const char *id;
    juce::AudioParameterFloat **ptr;
    juce::SmoothedValue<float> *smoother;
  };

  const FloatParameter floatParameters[] = {
      {Phaser::rate.id, &phaserRate, &phaserRateSmoother},
      {Phaser::depth.id, &phaserDepth, &phaserDepthSmoother},
      {Phaser::centerFreq.id, &phaserCenterFreq, &phaserCenterFreqSmoother},
      {Phaser::feedback.id, &phaserFeedback, &phaserFeedbackSmoother},
      {Phaser::mix.id, &phaserMix, &phaserMixSmoother},
      {Chorus::rate.id, &chorusRate, &chorusRateSmoother},
      {Chorus::depth.id, &chorusDepth, &chorusDepthSmoother},
      {Chorus::centerDelay.id, &chorusCenterDelay, &chorusCenterDelaySmoother},
      {Chorus::feedback.id, &chorusFeedback, &chorusFeedbackSmoother},
      {Chorus::mix.id, &chorusMix, &chorusMixSmoother},
      {Overdrive::saturation.id, &overdriveSaturation,
       &overdriveSaturationSmoother},
      {LadderFilter::cutoff.id, &ladderFilterCutoff,
       &ladderFilterCutoffSmoother},
      {LadderFilter::resonance.id, &ladderFilterResonance,
       &ladderFilterResonanceSmoother},
      {LadderFilter::drive.id, &ladderFilterDrive, &ladderFilterDriveSmoother},
      {Filter::freq.id, &filterFreq, &filterFreqSmoother},
      {Filter::quality.id, &filterQuality, &filterQualitySmoother},
      {Filter::gain.id, &filterGain, &filterGainSmoother},
      {Input::gain.id, &inputGain, nullptr},
      {Output::gain.id, &outputGain, nullptr},
  };
  for (auto &p : floatParameters) {
    *p.ptr =
        dynamic_cast<juce::AudioParameterFloat *>(apvts.getParameter(p.id));
    jassert(*p.ptr != nullptr);
    if (p.smoother)
      paramSmootherPairs.push_back({*p.ptr, p.smoother});
  }

  // Choice parameters
  struct ChoiceParameter {
    const Parameter &def;
    juce::AudioParameterChoice **ptr;
  };

  const ChoiceParameter choiceParameters[] = {
      {LadderFilter::mode, &ladderFilterMode},
      {Filter::mode, &filterMode},
  };
  for (auto &p : choiceParameters) {
    *p.ptr = dynamic_cast<juce::AudioParameterChoice *>(
        apvts.getParameter(p.def.id));
    jassert(*p.ptr != nullptr);
  }

  // Bool parameters
  struct BoolParameter {
    const Parameter &def;
    juce::AudioParameterBool **ptr;
  };

  const BoolParameter boolParameters[] = {
      {Phaser::bypass, &phaserBypass},
      {Chorus::bypass, &chorusBypass},
      {Overdrive::bypass, &overdriveBypass},
      {LadderFilter::bypass, &ladderFilterBypass},
      {Filter::bypass, &filterBypass},
  };
  for (auto &p : boolParameters) {
    *p.ptr =
        dynamic_cast<juce::AudioParameterBool *>(apvts.getParameter(p.def.id));
    jassert(*p.ptr != nullptr);
  }
}

void Parameters::prepareToPlay(double sampleRate) {
  for (const auto &pair : paramSmootherPairs) {
    pair.smoother->reset(sampleRate, 0.05);
  }
  updateSmoothers(1, SmootherUpdateMode::initialize);
}

void Parameters::updateSmoothers(int samplesToSkip,
                                 SmootherUpdateMode smootherMode) {
  for (const auto &pair : paramSmootherPairs) {
    if (smootherMode == SmootherUpdateMode::initialize) {
      pair.smoother->setCurrentAndTargetValue(pair.param->get());
    } else {
      pair.smoother->setTargetValue(pair.param->get());
    }
    pair.smoother->skip(samplesToSkip);
  }
}

juce::AudioProcessorValueTreeState::ParameterLayout
Parameters::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;
  const int versionHint = 1;

  for (const auto &param : getAllParameters()) {
    if (param.type == ParameterType::Float) {
      layout.add(std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{param.id, versionHint}, param.id,
          juce::NormalisableRange<float>(param.minValue, param.maxValue,
                                         param.step, param.skew),
          param.defaultValue, param.suffix));
    } else if (param.type == ParameterType::Choice) {
      layout.add(std::make_unique<juce::AudioParameterChoice>(
          juce::ParameterID{param.id, versionHint}, param.displayName,
          *param.choices, 0, ""));
    } else if (param.type == ParameterType::Bool) {
      layout.add(std::make_unique<juce::AudioParameterBool>(
          juce::ParameterID{param.id, versionHint}, param.id, false));
    }
  }

  return layout;
}
