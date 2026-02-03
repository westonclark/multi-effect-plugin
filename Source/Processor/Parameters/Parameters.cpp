#include "Parameters.h"

Parameters::Parameters(juce::AudioProcessor &processor)
    : apvts(processor, nullptr, "Parameters", createParameterLayout()) {

  // Initialize parameters from Value Tree
  auto floatParamInitializers = std::vector<FloatParamInitializer>{
      {&phaserRate, Phaser::rate.id},
      {&phaserCenterFreq, Phaser::centerFreq.id},
      {&phaserDepth, Phaser::depth.id},
      {&phaserFeedback, Phaser::feedback.id},
      {&phaserMix, Phaser::mix.id},
      {&chorusRate, Chorus::rate.id},
      {&chorusDepth, Chorus::depth.id},
      {&chorusCenterDelay, Chorus::centerDelay.id},
      {&chorusFeedback, Chorus::feedback.id},
      {&chorusMix, Chorus::mix.id},
      {&overdriveSaturation, Overdrive::saturation.id},
      {&ladderFilterCutoff, LadderFilter::cutoff.id},
      {&ladderFilterResonance, LadderFilter::resonance.id},
      {&ladderFilterDrive, LadderFilter::drive.id},
      {&filterFreq, Filter::freq.id},
      {&filterQuality, Filter::quality.id},
      {&filterGain, Filter::gain.id},
      {&inputGain, Input::gain.id},
      {&outputGain, Output::gain.id},
  };

  auto choiceParamInitializers = std::vector<ChoiceParamInitializer>{
      {&ladderFilterMode, LadderFilter::mode},
      {&filterMode, Filter::mode},
  };

  auto boolParamInitializers = std::vector<BoolParamInitializer>{
      {&phaserBypass, Phaser::bypass},
      {&chorusBypass, Chorus::bypass},
      {&overdriveBypass, Overdrive::bypass},
      {&ladderFilterBypass, LadderFilter::bypass},
      {&filterBypass, Filter::bypass},
  };

  initCachedParams<juce::AudioParameterFloat *>(floatParamInitializers);
  initCachedChoiceParams(choiceParamInitializers);
  initCachedBoolParams(boolParamInitializers);

  // Initialize parameter smoothers
  paramSmootherPairs = {
      {phaserRate, &phaserRateSmoother},
      {phaserDepth, &phaserDepthSmoother},
      {phaserCenterFreq, &phaserCenterFreqSmoother},
      {phaserFeedback, &phaserFeedbackSmoother},
      {phaserMix, &phaserMixSmoother},
      {chorusRate, &chorusRateSmoother},
      {chorusDepth, &chorusDepthSmoother},
      {chorusCenterDelay, &chorusCenterDelaySmoother},
      {chorusFeedback, &chorusFeedbackSmoother},
      {chorusMix, &chorusMixSmoother},
      {overdriveSaturation, &overdriveSaturationSmoother},
      {ladderFilterCutoff, &ladderFilterCutoffSmoother},
      {ladderFilterResonance, &ladderFilterResonanceSmoother},
      {ladderFilterDrive, &ladderFilterDriveSmoother},
      {filterFreq, &filterFreqSmoother},
      {filterQuality, &filterQualitySmoother},
      {filterGain, &filterGainSmoother},
  };
}

void Parameters::initCachedChoiceParams(
    const std::vector<ChoiceParamInitializer> &paramInitializers) {
  for (const auto &initializer : paramInitializers) {
    *initializer.paramPtr = dynamic_cast<juce::AudioParameterChoice *>(
        apvts.getParameter(initializer.param.id));
    jassert(*initializer.paramPtr != nullptr);
  }
}

void Parameters::initCachedBoolParams(
    const std::vector<BoolParamInitializer> &paramInitializers) {
  for (const auto &initializer : paramInitializers) {
    *initializer.paramPtr = dynamic_cast<juce::AudioParameterBool *>(
        apvts.getParameter(initializer.param.id));
    jassert(*initializer.paramPtr != nullptr);
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
