// Copyright (C) 2022-2024 Joseph Bellahcen <joeclb@icloud.com>

#include "ui/cli/CommandLineApp.hpp"

#include "ui/cli/EncoderCommand.hpp"
#include "ui/cli/GuiCommand.hpp"
#include "ui/cli/SynthesizerCommand.hpp"

namespace tms_express::ui {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CommandLineApp::CommandLineApp() {
    EncoderCommand::setup(this);
    SynthesizerCommand::setup(this);
    // GuiCommand::setup(this);

    require_subcommand(1);
}

/*
void CommandLineApp::setupRenderer() {
    // Shared paremeters
    renderer
        ->add_option("--samplerate", shared_params_.sample_rate_hz,
                     "Audio sampling rate (Hz)")
        ->default_val(kDefaultSampleRateHz);

    renderer
        ->add_option("--window", shared_params_.window_width_ms,
                     "Analysis window width (ms)")
        ->default_val(kDefaultWindowWidthMs);

    // Upper tract
    renderer
        ->add_option("--uhpf", upper_params_.highpass_cutoff_hz,
                     "Highpass filter cutoff for upper tract (Hz)")
        ->default_val(kDefaultHighpassCutoffHz);

    renderer
        ->add_option("--ulpf", upper_params_.lowpass_cutoff_hz,
                     "Lowpass filter cutoff for upper tract (Hz)")
        ->default_val(kDefaultLowpassCutoffHz);

    renderer
        ->add_option("--ualpha", upper_params_.pre_emphasis_alpha,
                     "Pre-emphasis coeff for upper tract")
        ->default_val(kDefaultPreEmphasisAlpha);

    // Lower tract
    renderer
        ->add_option("--lhpf", lower_params_.highpass_cutoff_hz,
                     "Highpass filter cutoff for lower tract (Hz)")
        ->default_val(kDefaultHighpassCutoffHz);

    renderer
        ->add_option("--llpf", lower_params_.lowpass_cutoff_hz,
                     "Lowpass filter cutoff for lower tract (Hz)")
        ->default_val(kDefaultLowpassCutoffHz);

    renderer
        ->add_option("--lalpha", lower_params_.pre_emphasis_alpha,
                     "Pre-emphasis coeff for lower tract")
        ->default_val(kDefaultPreEmphasisAlpha);

    renderer
        ->add_option("--minpitch", lower_params_.min_pitch_hz,
                     "Pitch estimate floor (Hz)")
        ->default_val(kDefaultMinPitchHz);

    renderer
        ->add_option("--maxpitch", lower_params_.max_pitch_hz,
                     "Pitch estimate ceiling (Hz)")
        ->default_val(kDefaultMaxPitchHz);

    // Post-processor
    renderer->add_option("--gainshift", post_params_.gain_shift, "Gain shift")
        ->default_val(kDefaultGainShift);

    renderer
        ->add_option("--gainnorm", post_params_.normalize_gain,
                     "Gain normalize")
        ->default_val(kDefaultNormalizeGain);

    renderer
        ->add_option("--voicedgain", post_params_.max_voiced_gain_db,
                     "Voiced gain estimate ceiling (dB)")
        ->default_val(kDefaultVoicedGainDb);

    renderer
        ->add_option("--unvoicedgain", post_params_.max_unvoiced_gain_db,
                     "Unvoiced gain estimate ceiling (dB)")
        ->default_val(kDefaultUnvoicedGainDb);

    renderer
        ->add_option("--repeat", post_params_.detect_repeat_frames,
                     "Detect repeat frames")
        ->default_val(kDefaultDetectRepeatFrames);

    // Required
    renderer->add_option("-i,--input,input", input_path_, "Path to audio file")
        ->required();

    renderer
        ->add_option("-o,--output,output", output_path_, "Path to output file")
        ->required();
}
*/

};  // namespace tms_express::ui
