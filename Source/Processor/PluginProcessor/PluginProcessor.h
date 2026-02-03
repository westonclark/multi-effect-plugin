#pragma once

#include "../../Utils/Fifos/DspOrderFifo.h"
#include "../../Utils/Fifos/InputLevelFifo.h"
#include "../../Utils/Fifos/OutputLevelFifo.h"
#include "../DSP/DSP.h"
#include "../Parameters/Parameters.h"
#include <JuceHeader.h>

// AUDIO PROCESSOR
//==============================================================================
class MultieffectpluginAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
    ,
                                        public juce::AudioProcessorARAExtension
#endif
{
public:
  // LIFECYCLE
  //==============================================================================
  MultieffectpluginAudioProcessor();
  ~MultieffectpluginAudioProcessor() override;

  // AUDIO PROCESSING
  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  // EDITOR
  //==============================================================================
  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override;

  // PLUGIN INFO
  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  // PROGRAMS
  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;

  // STATE MANAGEMENT
  //==============================================================================
  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  // DSP ORDER
  //==============================================================================
  static juce::String getDspNameFromOption(DspOption dspOption);
  static DspOption getDspOptionFromName(const juce::String &name);

  DSPOrderFifo<DspOrder> dspOrderFifo;
  InputLevelFifo<std::vector<float>> inputLevelFifo;
  OutputLevelFifo<std::vector<float>> outputLevelFifo;

  juce::dsp::Gain<float> inputGain;
  juce::dsp::Gain<float> outputGain;

  void saveDspOrderToState(const DspOrder &order);
  DspOrder getDspOrderFromState() const;

  void saveSelectedTabToState(const DspOption &selectedTab);
  DspOption getSelectedTabFromState() const;

  // MANAGERS
  //==============================================================================
  Parameters parameters;
  DSP dsp;

private:
  // DSP ORDER STATE
  //==============================================================================
  DspOrder dspOrder;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultieffectpluginAudioProcessor)
};
