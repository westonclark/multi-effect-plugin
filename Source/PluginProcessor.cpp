/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

auto getPhaserRateName() { return juce::String("Phaser Rate"); }
auto getPhaserCenterFreqName() { return juce::String("Phaser Center Freq"); }
auto getPhaserDepthName() { return juce::String("Phaser Depth"); }
auto getPhaserFeedbackName() { return juce::String("Phaser Feedback"); }
auto getPhaserMixName() { return juce::String("Phaser Mix"); }
auto getPhaserBypassName() { return juce::String("Phaser Bypass"); }

auto getChorusRateName() { return juce::String("Chorus Rate"); }
auto getChorusDepthName() { return juce::String("Chorus Depth"); }
auto getChorusCenterDelayName() { return juce::String("Chorus Center Delay"); }
auto getChorusFeedbackName() { return juce::String("Chorus Feedback"); }
auto getChorusMixName() { return juce::String("Chorus Mix"); }
auto getChorusBypassName() { return juce::String("Chorus Bypass"); }

auto getOverdriveSaturationName() { return juce::String("OverDrive"); }
auto getOverdriveBypassName() { return juce::String("Overdrive Bypass"); }

auto getLadderFilterModeName() { return juce::String("Ladder Filter Mode"); }
auto getLadderFilterCutoffName() {
  return juce::String("Ladder Filter Cutoff");
}
auto getLadderFilterResonanceName() {
  return juce::String("Ladder Filter Resonance");
}
auto getLadderFilterDriveName() { return juce::String("Ladder Filter Drive"); }
auto getLadderFilterChoices() {
  return juce::StringArray{"LPF12", "HPF12", "BPF12",
                           "LPF24", "HPF24", "BPF24"};
}
auto getLadderFilterBypassName() {
  return juce::String("Ladder Filter Bypass");
}

auto getFilterModeName() { return juce::String("Filter Mode"); }
auto getFilterFreqName() { return juce::String("Filter Freq"); }
auto getFilterQualityName() { return juce::String("Filter Quality"); }
auto getFilterGainName() { return juce::String("Filter Gain"); }
auto getFilterChoices() {
  return juce::StringArray{"Peak", "Bandpass", "Notch", "Allpass"};
}
auto getFilterBypassName() { return juce::String("Filter Bypass"); }

//==============================================================================
MultieffectpluginAudioProcessor::MultieffectpluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      )
#endif
{

  for (size_t i = 0; i < dspOrder.size(); ++i) {
    dspOrder[i] = static_cast<DSP_Option>(i);
  }

  auto floatParams = std::array{&phaserRate,
                                &phaserCenterFreq,
                                &phaserDepth,
                                &phaserFeedback,
                                &phaserMix,
                                &chorusRate,
                                &chorusDepth,
                                &chorusCenterDelay,
                                &chorusFeedback,
                                &chorusMix,
                                &overdriveSaturation,
                                &ladderFilterCutoff,
                                &ladderFilterResonance,
                                &ladderFilterDrive,
                                &filterFreq,
                                &filterQuality,
                                &filterGain};

  auto floatFunctions = std::array{&getPhaserRateName,
                                   &getPhaserCenterFreqName,
                                   &getPhaserDepthName,
                                   &getPhaserFeedbackName,
                                   &getPhaserMixName,
                                   &getChorusRateName,
                                   &getChorusDepthName,
                                   &getChorusCenterDelayName,
                                   &getChorusFeedbackName,
                                   &getChorusMixName,
                                   &getOverdriveSaturationName,
                                   &getLadderFilterCutoffName,
                                   &getLadderFilterResonanceName,
                                   &getLadderFilterDriveName,
                                   &getFilterFreqName,
                                   &getFilterQualityName,
                                   &getFilterGainName};

  auto choiceParams = std::array{&ladderFilterMode, &filterMode};
  auto choiceFunctions =
      std::array{&getLadderFilterModeName, &getFilterModeName};

  auto bypassParams = std::array{&phaserBypass, &chorusBypass, &overdriveBypass,
                                 &ladderFilterBypass, &filterBypass};
  auto bypassFunctions = std::array{
      &getPhaserBypassName, &getChorusBypassName, &getOverdriveBypassName,
      &getLadderFilterBypassName, &getFilterBypassName};

  // Floats
  initCachedParams<juce::AudioParameterFloat *>(floatParams, floatFunctions);
  // Choices
  initCachedParams<juce::AudioParameterChoice *>(choiceParams, choiceFunctions);
  // Booleans (Bypasses)
  initCachedParams<juce::AudioParameterBool *>(bypassParams, bypassFunctions);
}

MultieffectpluginAudioProcessor::~MultieffectpluginAudioProcessor() {}

//==============================================================================
const juce::String MultieffectpluginAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool MultieffectpluginAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool MultieffectpluginAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool MultieffectpluginAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double MultieffectpluginAudioProcessor::getTailLengthSeconds() const {
  return 0.0;
}

int MultieffectpluginAudioProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs, so this should be at least 1, even if you're not really
            // implementing programs.
}

int MultieffectpluginAudioProcessor::getCurrentProgram() { return 0; }

void MultieffectpluginAudioProcessor::setCurrentProgram(int index) {}

const juce::String MultieffectpluginAudioProcessor::getProgramName(int index) {
  return {};
}

void MultieffectpluginAudioProcessor::changeProgramName(
    int index, const juce::String &newName) {}

//==============================================================================
void MultieffectpluginAudioProcessor::prepareToPlay(double sampleRate,
                                                    int samplesPerBlock) {

  juce::dsp::ProcessSpec spec;
  spec.sampleRate = sampleRate;
  spec.maximumBlockSize = samplesPerBlock;
  spec.numChannels = 1;

  leftChannel.prepare(spec);
  rightChannel.prepare(spec);

  for (auto smoother : getSmoothers()) {
    smoother->reset(sampleRate, 0.05);
  }
  updateSmoothersFromParams(1, SmootherUpdateMode::initialize);
}

void MultieffectpluginAudioProcessor::updateSmoothersFromParams(
    int samplesToSkip, SmootherUpdateMode smootherMode) {
  auto paramsToSmooth = std::vector{
      phaserRate,
      phaserDepth,
      phaserCenterFreq,
      phaserFeedback,
      phaserMix,
      chorusRate,
      chorusDepth,
      chorusCenterDelay,
      chorusFeedback,
      chorusMix,
      overdriveSaturation,
      ladderFilterCutoff,
      ladderFilterResonance,
      ladderFilterDrive,
      filterFreq,
      filterQuality,
      filterGain,
  };

  auto smoothers = getSmoothers();
  jassert(smoothers.size() == paramsToSmooth.size());

  for (size_t i = 0; i < smoothers.size(); ++i) {
    auto smoother = smoothers[i];
    auto param = paramsToSmooth[i];
    if (smootherMode == SmootherUpdateMode::initialize) {
      smoother->setCurrentAndTargetValue(param->get());
    } else {
      smoother->setTargetValue(param->get());
    }
    smoother->skip(samplesToSkip);
  }
}

std::vector<juce::SmoothedValue<float> *>
MultieffectpluginAudioProcessor::getSmoothers() {
  auto smoothers = std::vector{
      &phaserRateSmoother,
      &phaserDepthSmoother,
      &phaserCenterFreqSmoother,
      &phaserFeedbackSmoother,
      &phaserMixSmoother,
      &chorusRateSmoother,
      &chorusDepthSmoother,
      &chorusCenterDelaySmoother,
      &chorusFeedbackSmoother,
      &chorusMixSmoother,
      &overdriveSaturationSmoother,
      &ladderFilterCutoffSmoother,
      &ladderFilterResonanceSmoother,
      &ladderFilterDriveSmoother,
      &filterFreqSmoother,
      &filterQualitySmoother,
      &filterGainSmoother,
  };
  return smoothers;
}

void MultieffectpluginAudioProcessor::MonoChannelDSP::prepare(
    const juce::dsp::ProcessSpec &spec) {
  jassert(spec.numChannels == 1);

  std::vector<juce::dsp::ProcessorBase *> dsp{&phaser, &chorus, &overdrive,
                                              &ladderFilter, &filter};

  for (auto processor : dsp) {
    processor->prepare(spec);
    processor->reset();
  }
}
void MultieffectpluginAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MultieffectpluginAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

  // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

juce::AudioProcessorValueTreeState::ParameterLayout
MultieffectpluginAudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  const int versionHint = 1;
  juce::String name;
  juce::StringArray choices;

  // Phaser
  name = getPhaserRateName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 2.f, 0.01f, 1.f), 0.2f, "Hz"));

  name = getPhaserDepthName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f), 0.05f, "%"));

  name = getPhaserCenterFreqName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), 1000.f, "Hz"));

  name = getPhaserFeedbackName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(-1.f, 1.f, 0.01f, 1.f), 0.0f, "%"));

  name = getPhaserMixName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f), 0.05f, "%"));

  name = getPhaserBypassName();
  layout.add(std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{name, versionHint}, name, false));

  // Chorus
  name = getChorusRateName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 2.f, 0.01f, 1.f), 0.2f, "Hz"));

  name = getChorusDepthName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f), 0.05f, "%"));

  name = getChorusCenterDelayName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.f, 100.f, 1.f, 1.f), 7.f, "ms"));

  name = getChorusFeedbackName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(-1.f, 1.f, 0.01f, 1.f), 0.0f, "%"));

  name = getChorusMixName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f), 0.05f, "%"));

  name = getChorusBypassName();
  layout.add(std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{name, versionHint}, name, false));

  // Drive
  name = getOverdriveSaturationName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(1.f, 100.f, 0.1f, 1.f), 1.f, ""));

  name = getOverdriveBypassName();
  layout.add(std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{name, versionHint}, name, false));

  // Ladder Filter
  name = getLadderFilterModeName();
  choices = getLadderFilterChoices();
  layout.add(std::make_unique<juce::AudioParameterChoice>(
      juce::ParameterID{name, versionHint}, name, choices, 0));

  name = getLadderFilterCutoffName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(20.f, 20000.f, 0.1f, 1.f), 20000.f, "Hz"));

  name = getLadderFilterResonanceName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.f, 1.f, 0.1f, 1.f), 0.f, ""));

  name = getLadderFilterDriveName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(1.f, 100.f, 0.1f, 1.f), 1.f, ""));

  name = getLadderFilterBypassName();
  layout.add(std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{name, versionHint}, name, false));

  // Filter
  name = getFilterModeName();
  choices = getFilterChoices();
  layout.add(std::make_unique<juce::AudioParameterChoice>(
      juce::ParameterID{name, versionHint}, name, choices, 0));

  name = getFilterFreqName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), 1000.f, "Hz"));

  name = getFilterQualityName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.1f, 10.f, .05f, 1.f), 5.f, ""));

  name = getFilterGainName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(-24.f, 24.f, .5f, 1.f), 0.f, "dB"));

  name = getFilterBypassName();
  layout.add(std::make_unique<juce::AudioParameterBool>(
      juce::ParameterID{name, versionHint}, name, false));

  return layout;
}

void MultieffectpluginAudioProcessor::MonoChannelDSP::update() {
  phaser.dsp.setRate(processor.phaserRateSmoother.getCurrentValue());
  phaser.dsp.setCentreFrequency(
      processor.phaserCenterFreqSmoother.getCurrentValue());
  phaser.dsp.setDepth(processor.phaserDepthSmoother.getCurrentValue());
  phaser.dsp.setFeedback(processor.phaserFeedbackSmoother.getCurrentValue());
  phaser.dsp.setMix(processor.phaserMixSmoother.getCurrentValue());

  chorus.dsp.setRate(processor.chorusRateSmoother.getCurrentValue());
  chorus.dsp.setDepth(processor.chorusDepthSmoother.getCurrentValue());
  chorus.dsp.setCentreDelay(
      processor.chorusCenterDelaySmoother.getCurrentValue());
  chorus.dsp.setFeedback(processor.chorusFeedbackSmoother.getCurrentValue());
  chorus.dsp.setMix(processor.chorusMixSmoother.getCurrentValue());

  overdrive.dsp.setDrive(
      processor.overdriveSaturationSmoother.getCurrentValue());

  ladderFilter.dsp.setMode(static_cast<juce::dsp::LadderFilterMode>(
      processor.ladderFilterMode->getIndex()));
  ladderFilter.dsp.setCutoffFrequencyHz(
      processor.ladderFilterCutoffSmoother.getCurrentValue());
  ladderFilter.dsp.setResonance(
      processor.ladderFilterResonanceSmoother.getCurrentValue());
  ladderFilter.dsp.setDrive(
      processor.ladderFilterDriveSmoother.getCurrentValue());

  // Update filter coeeficients
  auto sampleRate = processor.getSampleRate();

  auto currentFilterMode = processor.filterMode->getIndex();
  auto currentFilterFreq = processor.filterFreq->get();
  auto currentFilterQuality = processor.filterQuality->get();
  auto currentFilterGain = processor.filterGain->get();

  bool filterChanged = false;
  filterChanged |= (currentFilterFreq != cachedFilterFreq);
  filterChanged |= (currentFilterQuality != cachedFilterQuality);
  filterChanged |= (currentFilterGain != cachedFilterGain);

  auto updatedMode = static_cast<FilterMode>(currentFilterMode);
  filterChanged |= (currentFilterMode != cachedFilterMode);

  if (filterChanged) {
    cachedFilterMode = updatedMode;
    cachedFilterFreq = currentFilterFreq;
    cachedFilterQuality = currentFilterQuality;
    cachedFilterGain = currentFilterGain;

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
      filter.reset();
    }
  }
}

void MultieffectpluginAudioProcessor::processBlock(
    juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // Clear buffer of garbage
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // Check if there's a new DSP order from the GUI
  auto newDSPOrder = DSP_Order();
  while (dspOrderFifo.pull(newDSPOrder)) {
    // If we pull a change, update the dspOrder
    if (newDSPOrder != DSP_Order()) {
      dspOrder = newDSPOrder;
    }
  }

  // Process audio in chunks for better parameter smoothing resolution
  const int maxChunkSize = 64;
  const auto numSamples = buffer.getNumSamples();
  auto block = juce::dsp::AudioBlock<float>(buffer);

  for (size_t startSample = 0; startSample < numSamples;) {
    // Calculate chunk size (might be smaller for the last chunk)
    auto samplesThisChunk =
        juce::jmin(maxChunkSize, static_cast<int>(numSamples - startSample));

    // Update smoothers for this chunk
    updateSmoothersFromParams(samplesThisChunk,
                              SmootherUpdateMode::updateExisting);

    // Update DSP parameters from smoothed values
    leftChannel.update();
    rightChannel.update();

    // Process this chunk
    auto subBlock = block.getSubBlock(startSample, samplesThisChunk);
    leftChannel.process(subBlock.getSingleChannelBlock(0), dspOrder);
    rightChannel.process(subBlock.getSingleChannelBlock(1), dspOrder);

    startSample += samplesThisChunk;
  }
}

void MultieffectpluginAudioProcessor::MonoChannelDSP::process(
    juce::dsp::AudioBlock<float> block, const DSP_Order &dspOrder) {
  // Convert dspOrder into pointers
  DSP_Pointers dspPointers;
  dspPointers.fill({});

  for (size_t i = 0; i < dspPointers.size(); ++i) {
    switch (dspOrder[i]) {
    case DSP_Option::Phase:
      dspPointers[i].processor = &phaser;
      dspPointers[i].bypassed = processor.phaserBypass->get();
      break;
    case DSP_Option::Chorus:
      dspPointers[i].processor = &chorus;
      dspPointers[i].bypassed = processor.chorusBypass->get();
      break;
    case DSP_Option::OverDrive:
      dspPointers[i].processor = &overdrive;
      dspPointers[i].bypassed = processor.overdriveBypass->get();
      break;
    case DSP_Option::LadderFilter:
      dspPointers[i].processor = &ladderFilter;
      dspPointers[i].bypassed = processor.ladderFilterBypass->get();
      break;
    case DSP_Option::Filter:
      dspPointers[i].processor = &filter;
      dspPointers[i].bypassed = processor.filterBypass->get();
      break;
    case DSP_Option::END_OF_LIST:
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
};

//==============================================================================
bool MultieffectpluginAudioProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *MultieffectpluginAudioProcessor::createEditor() {
  // return new MultieffectpluginAudioProcessorEditor(*this);
  return new juce::GenericAudioProcessorEditor(*this);
}
//==============================================================================

template <>
struct juce::VariantConverter<MultieffectpluginAudioProcessor::DSP_Order> {
  static MultieffectpluginAudioProcessor::DSP_Order
  fromVar(const juce::var &variable) {
    using T = MultieffectpluginAudioProcessor::DSP_Order;
    T dspOrder;
    jassert(variable.isBinaryData());

    if (variable.isBinaryData() == false) {
      dspOrder.fill(MultieffectpluginAudioProcessor::DSP_Option::END_OF_LIST);
    } else {

      auto memoryBlock = *variable.getBinaryData();

      juce::MemoryInputStream stream(memoryBlock, false);
      std::vector<int> array;
      while (!stream.isExhausted()) {
        array.push_back(stream.readInt());
      }

      jassert(array.size() == dspOrder.size());
      for (size_t i = 0; i < dspOrder.size(); ++i) {
        dspOrder[i] =
            static_cast<MultieffectpluginAudioProcessor::DSP_Option>(array[i]);
      }
    }
    return dspOrder;
  };

  static juce::var toVar(const MultieffectpluginAudioProcessor::DSP_Order &t) {
    juce::MemoryBlock memoryBlock;
    {
      juce::MemoryOutputStream stream(memoryBlock, false);
      for (const auto &value : t) {
        stream.writeInt(static_cast<int>(value));
      }
    }
    return memoryBlock;
  };
};

void MultieffectpluginAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {

  apvts.state.setProperty(
      "dspOrder",
      juce::VariantConverter<MultieffectpluginAudioProcessor::DSP_Order>::toVar(
          dspOrder),
      nullptr);

  juce::MemoryOutputStream memoryStream(destData, false);
  apvts.state.writeToStream(memoryStream);
}

void MultieffectpluginAudioProcessor::setStateInformation(const void *data,
                                                          int sizeInBytes) {
  auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
  if (tree.isValid()) {
    apvts.replaceState(tree);
    if (apvts.state.hasProperty("dspOrder")) {
      auto order =
          juce::VariantConverter<MultieffectpluginAudioProcessor::DSP_Order>::
              fromVar(apvts.state.getProperty("dspOrder"));
      dspOrderFifo.push(order);
    }
    DBG(apvts.state.toXmlString());
  }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new MultieffectpluginAudioProcessor();
}
