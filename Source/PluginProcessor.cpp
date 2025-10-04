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

auto getChorusRateName() { return juce::String("Chorus Rate"); }
auto getChorusDepthName() { return juce::String("Chorus Depth"); }
auto getChorusCenterDelayName() { return juce::String("Chorus Center Delay"); }
auto getChorusFeedbackName() { return juce::String("Chorus Feedback"); }
auto getChorusMixName() { return juce::String("Chorus Mix"); }

auto getOverdriveSaturationName() { return juce::String("OverDrive"); }

auto getLadderFilterModeName() { return juce::String("Ladder Filter Mode"); }
auto getLadderFilterCutoffName() {
  return juce::String("Ladder Filter Cutoff");
}
auto getLadderFilterResonanceName() {
  return juce::String("Ladder Filter Resonance");
}
auto getLadderFilterDriveName() { return juce::String("Ladder Filter Drive"); }

auto getLadderFilterChoices() {
  return juce::StringArray{"LPF12"
                           "HPF12"
                           "BPF12"
                           "LPF24"
                           "HPF24"
                           "BPF24"};
}

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
  auto params = std::array{&phaserRate,
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
                           &ladderFilterDrive};

  auto functions = std::array{&getPhaserRateName,
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
                              &getLadderFilterDriveName};

  for (size_t i = 0; i < params.size(); ++i) {
    auto paramPointer = params[i];
    *paramPointer = dynamic_cast<juce::AudioParameterFloat *>(
        apvts.getParameter(functions[i]()));

    jassert(*paramPointer != nullptr);
  }

  ladderFilterMode = dynamic_cast<juce::AudioParameterChoice *>(
      apvts.getParameter(getLadderFilterModeName()));
  jassert(ladderFilterMode != nullptr);
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
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
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

  // Phaser
  name = getPhaserRateName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 2.f, 0.01f, 1.f), 0.2f, "Hz"

      ));
  name = getPhaserDepthName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f), 0.05f, "%"

      ));
  name = getPhaserCenterFreqName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), 1000.f, "Hz"

      ));
  name = getPhaserFeedbackName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(-1.f, 1.f, 0.01f, 1.f), 0.0f, "%"

      ));
  name = getPhaserMixName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f), 0.05f, "%"

      ));

  // Chorus
  name = getChorusRateName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 2.f, 0.01f, 1.f), 0.2f, "Hz"

      ));
  name = getChorusDepthName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f), 0.05f, "%"

      ));
  name = getChorusCenterDelayName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.f, 100.f, 1.f, 1.f), 7.f, "ms"

      ));
  name = getChorusFeedbackName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(-1.f, 1.f, 0.01f, 1.f), 0.0f, "%"

      ));
  name = getChorusMixName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.01f, 1.f, 0.01f, 1.f), 0.05f, "%"

      ));

  // Drive
  name = getOverdriveSaturationName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(1.f, 100.f, 0.1f, 1.f), 1.f, ""

      ));

  // Ladder Filter
  name = getLadderFilterModeName();
  auto choices = getLadderFilterChoices();
  layout.add(std::make_unique<juce::AudioParameterChoice>(
      juce::ParameterID{name, versionHint}, name, choices, 0

      ));
  name = getLadderFilterCutoffName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(20.f, 20000.f, 0.1f, 1.f), 20000.f, "Hz"

      ));
  name = getLadderFilterResonanceName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(0.f, 1.f, 0.1f, 1.f), 0.f, ""

      ));
  name = getLadderFilterDriveName();
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID{name, versionHint}, name,
      juce::NormalisableRange<float>(1.f, 100.f, 0.1f, 1.f), 1.f, ""

      ));

  return layout;
}

void MultieffectpluginAudioProcessor::processBlock(
    juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  auto newDSPOrder = DSP_Order();
  while (dspOrderFifo.pull(newDSPOrder))

    // If we pull a change, update the dspOrder
    if (newDSPOrder != DSP_Order()) {
      dspOrder = newDSPOrder;
    }

  // Convert dspOrder into pointers
  DSP_Pointers dspPointers;
  for (size_t i = 0; i < dspPointers.size(); ++i) {
    switch (dspOrder[i]) {
    case DSP_Option::Phase:
      dspPointers[i] = &phaser;
      break;
    case DSP_Option::Chorus:
      dspPointers[i] = &chorus;
      break;
    case DSP_Option::OverDrive:
      dspPointers[i] = &overdrive;
      break;
    case DSP_Option::LadderFilter:
      dspPointers[i] = &ladderFilter;
      break;
    case DSP_Option::END_OF_LIST:
      jassertfalse;
      break;
    }
  }

  // Process
  auto block = juce::dsp::AudioBlock<float>(buffer);
  auto context = juce::dsp::ProcessContextReplacing<float>(block);

  for (size_t i = 0; i < dspPointers.size(); ++i) {
    if (dspPointers[i] != nullptr) {
      dspPointers[i]->process(context);
    }
  }
}

//==============================================================================
bool MultieffectpluginAudioProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *MultieffectpluginAudioProcessor::createEditor() {
  return new MultieffectpluginAudioProcessorEditor(*this);
}

//==============================================================================
void MultieffectpluginAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
}

void MultieffectpluginAudioProcessor::setStateInformation(const void *data,
                                                          int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new MultieffectpluginAudioProcessor();
}
