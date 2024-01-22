// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_USER_INTERFACES_MAINWINDOW_HPP_
#define TMS_EXPRESS_USER_INTERFACES_MAINWINDOW_HPP_

#include <QAction>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenuBar>
#include <QVBoxLayout>

#include <QtMultimedia/QAudioOutput>

#include <string>
#include <vector>

#include "audio/AudioBuffer.hpp"
#include "audio/AudioFilter.hpp"
#include "bitstream/BitstreamGenerator.hpp"
#include "encoding/Frame.hpp"
#include "encoding/FrameEncoder.hpp"
#include "encoding/FramePostprocessor.hpp"
#include "encoding/Synthesizer.hpp"
#include "analysis/PitchEstimator.hpp"
#include "analysis/LinearPredictor.hpp"
#include "ui/Audio_Waveform/AudioWaveformView.hpp"
#include "ui/Control_Panels/ControlPanelPitchView.hpp"
#include "ui/Control_Panels/ControlPanelLpcView.hpp"
#include "ui/Control_Panels/ControlPanelPostView.hpp"

#define TE_WINDOW_MIN_WIDTH    1000
#define TE_WINDOW_MIN_HEIGHT   800
#define TE_WINDOW_MARGINS      5
#define TE_AUDIO_SAMPLE_RATE   8000

namespace tms_express::ui {

/// @brief GUI frontend for application
class MainWindow : public QMainWindow {
    Q_OBJECT

 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Creates and sets-up a new GUI window for application
    /// @param parent Parent Qt widget
    explicit MainWindow(QWidget *parent = nullptr);

 public slots:
    ///////////////////////////////////////////////////////////////////////////
    // Qt Slots ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Triggers audio import, pre-processing, lpc & pitch analysis, and
    ///         post-processing
    void onOpenFile();

    /// @brief Triggers bitstream export
    void onSaveBitstream();

    /// @brief Triggers synthesized audio export
    void onExportAudio();

    /// @brief Triggers input audio file playback
    void onInputAudioPlay();

    /// @brief Triggers synthesized audio playback
    void onLpcAudioPlay();

    /// @brief Triggers pitch & LPC analysis and post-processing
    void onPitchParamEdit();

    /// @brief Triggers LPC analysis and post-processing
    void onLpcParamEdit();

    /// @brief Triggers post-processing
    void onPostProcEdit();

 private:
    ///////////////////////////////////////////////////////////////////////////
    // UI Helper Methods //////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Assigns Qt slots and signals to UI elements
    /// @note This function must only be called once
    void configureUiSlots();

    /// @brief Updates the state of all UI elements, including Control Panel
    ///         and Waveform Views
    void configureUiState();

    /// @brief Re-draws Waveform View plots
    void drawPlots();

    ///////////////////////////////////////////////////////////////////////////
    // LPC Routines ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Applies pre-processing to audio buffer and perform pitch analysis
    ///         to populate pitch curve table
    void performPitchAnalysis();

    /// @brief Applies pre-processing to LPC buffer and perform LPC analysis
    ///         to populate Frame table
    /// @note This function may trigger a call to
    ///         MainWindow::performPitchAnalysis() if the analysis/segmentation
    ///         window width has been changed since pitch analysis was last
    ///         performed
    void performLpcAnalysis();

    /// @brief Applies post-processing to Frame table
    void performPostProc();

    ///////////////////////////////////////////////////////////////////////////
    // Bitstream I/O //////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Imports bitstream from disk and populates Frame table
    /// @param path Path to existing bitstream file
    void importBitstream(const std::string& path);

    /// @brief Exports Frame table to disk as bitstream file
    /// @param path Path to new bitstream file
    void exportBitstream(const std::string& path);

    ///////////////////////////////////////////////////////////////////////////
    // Helper Methods /////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Computes checksum of vector of floating-point samples
    /// @param samples Samples to uniquely identify via checksum
    /// @return Checksum of vector
    /// @details The checksum is used to generate a unique filename, which is
    ///             stored temporarily to facilitate playback of audio. In
    ///             addition to the samples from the vector, this checksum also
    ///             incorporates the pre-emphasis filter_ coefficients of both
    ///             the audio and LPC buffers, as applying the pre-emphasis
    ///             filter_ to floating point samples may not impact the checksum
    int samplesChecksum(std::vector<float> samples);

    ///////////////////////////////////////////////////////////////////////////
    // Qt Layout Members //////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    QWidget *main_widget_;
    QVBoxLayout *main_layout_;
    QGroupBox *control_panel_group_;
    QHBoxLayout *control_panel_layout_;

    ///////////////////////////////////////////////////////////////////////////
    // Qt Menu Bar Members ////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    QMenuBar *menu_bar_;
    QAction *action_export_;
    QAction *action_open_;
    QAction *action_save_;

    ///////////////////////////////////////////////////////////////////////////
    // Qt Multimedia Members //////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    QMediaPlayer *player;
    QAudioOutput *audio_output_;

    ///////////////////////////////////////////////////////////////////////////
    // Control Panel View Members /////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    ControlPanelPitchView *pitch_control_;
    ControlPanelLpcView *lpc_control_;
    ControlPanelPostView *post_control_;

    ///////////////////////////////////////////////////////////////////////////
    // Audio Waveform View Members ////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    AudioWaveformView *input_waveform_;
    AudioWaveformView *lpc_waveform_;

    ///////////////////////////////////////////////////////////////////////////
    // Audio Buffer Members ///////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    AudioBuffer input_buffer_;
    AudioBuffer lpc_buffer_;

    ///////////////////////////////////////////////////////////////////////////
    // Data Tables ////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    std::vector<Frame> frame_table_;
    std::vector<int> pitch_period_table_;
    std::vector<float> pitch_curve_table_;

    ///////////////////////////////////////////////////////////////////////////
    // LPC Analysis Object Members ////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    Synthesizer synthesizer_ = Synthesizer();
    AudioFilter filter_ = AudioFilter();
    PitchEstimator pitch_estimator_ = PitchEstimator(TE_AUDIO_SAMPLE_RATE);
    LinearPredictor linear_predictor_ = LinearPredictor();
    FramePostprocessor frame_postprocessor_ = FramePostprocessor(&frame_table_);
};

};  // namespace tms_express::ui

#endif  // TMS_EXPRESS_USER_INTERFACES_MAINWINDOW_HPP_
