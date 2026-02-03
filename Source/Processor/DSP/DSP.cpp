#include "DSP.h"

DSP::DSP(Parameters &params, juce::AudioProcessor &processor)
    : leftChannel(params, processor), rightChannel(params, processor),
      parameters(params) {}

void DSP::prepareToPlay(const juce::dsp::ProcessSpec &spec) {
  leftChannel.prepare(spec);
  rightChannel.prepare(spec);
}

void DSP::processBlock(juce::dsp::AudioBlock<float> leftBlock,
                       juce::dsp::AudioBlock<float> rightBlock,
                       const DspOrder &dspOrder) {
  leftChannel.inputGain.setGainDecibels(parameters.inputGain->get());
  leftChannel.inputGain.process(
      juce::dsp::ProcessContextReplacing<float>(leftBlock));

  rightChannel.inputGain.setGainDecibels(parameters.inputGain->get());
  rightChannel.inputGain.process(
      juce::dsp::ProcessContextReplacing<float>(rightBlock));

  leftChannel.update();
  rightChannel.update();

  leftChannel.process(leftBlock, dspOrder);
  rightChannel.process(rightBlock, dspOrder);

  leftChannel.outputGain.setGainDecibels(parameters.outputGain->get());
  leftChannel.outputGain.process(
      juce::dsp::ProcessContextReplacing<float>(leftBlock));

  rightChannel.outputGain.setGainDecibels(parameters.outputGain->get());
  rightChannel.outputGain.process(
      juce::dsp::ProcessContextReplacing<float>(rightBlock));
}

// DSP CHANNEL
//==============================================================================
DSP::DspChannel::DspChannel(Parameters &params, juce::AudioProcessor &proc)
    : parameters(params), processor(proc) {}

void DSP::DspChannel::prepare(const juce::dsp::ProcessSpec &spec) {
  jassert(spec.numChannels == 1);

  std::vector<juce::dsp::ProcessorBase *> dsp{&phaser, &chorus, &overdrive,
                                              &ladderFilter, &filter};

  for (auto processor : dsp) {
    processor->prepare(spec);
    processor->reset();
  }

  // Set default filter settings for overdrive
  overdrive.dsp.setMode(juce::dsp::LadderFilterMode::LPF12);
  overdrive.dsp.setCutoffFrequencyHz(20000.0f);
  overdrive.dsp.setResonance(0);

  inputGain.prepare(spec);
  outputGain.prepare(spec);
  inputGain.setRampDurationSeconds(0.05);
  outputGain.setRampDurationSeconds(0.05);
}

void DSP::DspChannel::update() {
  // Phaser
  phaser.dsp.setRate(parameters.phaserRateSmoother.getCurrentValue());
  phaser.dsp.setCentreFrequency(
      parameters.phaserCenterFreqSmoother.getCurrentValue());
  phaser.dsp.setDepth(parameters.phaserDepthSmoother.getCurrentValue());
  phaser.dsp.setFeedback(parameters.phaserFeedbackSmoother.getCurrentValue());
  phaser.dsp.setMix(parameters.phaserMixSmoother.getCurrentValue());
  // Chorus
  chorus.dsp.setRate(parameters.chorusRateSmoother.getCurrentValue());
  chorus.dsp.setDepth(parameters.chorusDepthSmoother.getCurrentValue());
  chorus.dsp.setCentreDelay(
      parameters.chorusCenterDelaySmoother.getCurrentValue());
  chorus.dsp.setFeedback(parameters.chorusFeedbackSmoother.getCurrentValue());
  chorus.dsp.setMix(parameters.chorusMixSmoother.getCurrentValue());
  // Drive
  overdrive.dsp.setDrive(
      parameters.overdriveSaturationSmoother.getCurrentValue());
  // Ladder Filter
  ladderFilter.dsp.setMode(static_cast<juce::dsp::LadderFilterMode>(
      parameters.ladderFilterMode->getIndex()));
  ladderFilter.dsp.setCutoffFrequencyHz(
      parameters.ladderFilterCutoffSmoother.getCurrentValue());
  ladderFilter.dsp.setResonance(
      parameters.ladderFilterResonanceSmoother.getCurrentValue());
  ladderFilter.dsp.setDrive(
      parameters.ladderFilterDriveSmoother.getCurrentValue());
  // Filter
  auto currentFilterFreq = parameters.filterFreqSmoother.getCurrentValue();
  auto currentFilterQuality =
      parameters.filterQualitySmoother.getCurrentValue();
  auto currentFilterGain = parameters.filterGainSmoother.getCurrentValue();
  auto currentFilterMode = parameters.filterMode->getIndex();

  // Only update filter coefficients if mode changes or if values are changing
  bool modeChanged = (currentFilterMode != cachedFilterMode);
  bool paramsChanging = (currentFilterFreq != cachedFilterFreq) ||
                        (currentFilterQuality != cachedFilterQuality) ||
                        (currentFilterGain != cachedFilterGain);

  if (modeChanged || paramsChanging) {
    cachedFilterMode = static_cast<FilterMode>(currentFilterMode);
    cachedFilterFreq = currentFilterFreq;
    cachedFilterQuality = currentFilterQuality;
    cachedFilterGain = currentFilterGain;

    auto sampleRate = processor.getSampleRate();
    juce::dsp::IIR::Coefficients<float>::Ptr coefficients;

    switch (cachedFilterMode) {
    case FilterMode::Peak: {
      coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
          sampleRate, cachedFilterFreq, cachedFilterQuality,
          juce::Decibels::decibelsToGain(cachedFilterGain));
      break;
    };
    case FilterMode::Bandpass: {
      coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(
          sampleRate, cachedFilterFreq, cachedFilterQuality);
      break;
    }
    case FilterMode::Notch: {
      coefficients = juce::dsp::IIR::Coefficients<float>::makeNotch(
          sampleRate, cachedFilterFreq, cachedFilterQuality);
      break;
    };
    case FilterMode::Allpass: {
      coefficients = juce::dsp::IIR::Coefficients<float>::makeAllPass(
          sampleRate, cachedFilterFreq, cachedFilterQuality);
      break;
    }
    case FilterMode::END_OF_LIST: {
      jassertfalse;
      break;
    }
    }

    if (coefficients != nullptr) {
      *filter.dsp.coefficients = *coefficients;
    }
  }
}

void DSP::DspChannel::process(juce::dsp::AudioBlock<float> block,
                              const DspOrder &dspOrder) {
  // Convert dspOrder into pointers
  DspPointers dspPointers;
  dspPointers.fill({});

  for (size_t i = 0; i < dspPointers.size(); ++i) {
    switch (dspOrder[i]) {
    case DspOption::Phase:
      dspPointers[i].processor = &phaser;
      dspPointers[i].bypassed = parameters.phaserBypass->get();
      break;
    case DspOption::Chorus:
      dspPointers[i].processor = &chorus;
      dspPointers[i].bypassed = parameters.chorusBypass->get();
      break;
    case DspOption::OverDrive:
      dspPointers[i].processor = &overdrive;
      dspPointers[i].bypassed = parameters.overdriveBypass->get();
      break;
    case DspOption::LadderFilter:
      dspPointers[i].processor = &ladderFilter;
      dspPointers[i].bypassed = parameters.ladderFilterBypass->get();
      break;
    case DspOption::Filter:
      dspPointers[i].processor = &filter;
      dspPointers[i].bypassed = parameters.filterBypass->get();
      break;
    case DspOption::END_OF_LIST:
      jassertfalse;
      break;
    }
  }

  // Process
  auto context = juce::dsp::ProcessContextReplacing<float>(block);
  for (size_t i = 0; i < dspPointers.size(); ++i) {
    if (dspPointers[i].processor != nullptr && !dspPointers[i].bypassed) {
      juce::ScopedValueSetter<bool> svs(context.isBypassed,
                                        dspPointers[i].bypassed);
      dspPointers[i].processor->process(context);
    }
  }
}
