// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#include "ui/gui/MainWindow.hpp"

#include <QAction>
#include <QMessageBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenuBar>
#include <QVBoxLayout>

#include <QtMultimedia/QAudioOutput>

#include <fstream>
#include <iostream>
#include <vector>

#include "lib/CRC.h"

#include "audio/AudioBuffer.hpp"
#include "encoding/FramePostprocessor.hpp"
#include "analysis/Autocorrelation.hpp"
#include "ui/gui/audiowaveform/AudioWaveformView.hpp"
#include "ui/gui/controlpanels/ControlPanelPitchView.hpp"
#include "ui/gui/controlpanels/ControlPanelLpcView.hpp"
#include "ui/gui/controlpanels/ControlPanelPostView.hpp"

namespace tms_express::ui {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Set the minimum requirements for window dimensions and margins
    setMinimumSize(TE_WINDOW_MIN_WIDTH, TE_WINDOW_MIN_HEIGHT);

    setContentsMargins(TE_WINDOW_MARGINS, TE_WINDOW_MARGINS,
        TE_WINDOW_MARGINS, TE_WINDOW_MARGINS);

    // The main widget will hold all contents of the Main Window
    main_widget_ = new QWidget(this);
    setCentralWidget(main_widget_);

    // The main layout separates the main widget into rows
    main_layout_ = new QVBoxLayout(main_widget_);

    // The control panel group holds a horizontal layout, and each column is
    // occupied by a control panel
    control_panel_group_ = new QGroupBox("Control Panel", this);

    // The control panel layout must never be allowed to become smaller than
    // its contents
    control_panel_layout_ = new QHBoxLayout(control_panel_group_);
    control_panel_layout_->setSizeConstraint(QLayout::SetMinimumSize);

    pitch_control_ = new ControlPanelPitchView(this);
    pitch_control_->configureSlots();
    pitch_control_->reset();

    lpc_control_ = new ControlPanelLpcView(this);
    lpc_control_->configureSlots();
    lpc_control_->reset();

    post_control_ = new ControlPanelPostView(this);
    post_control_->configureSlots();
    post_control_->reset();

    control_panel_layout_->addWidget(pitch_control_);
    control_panel_layout_->addWidget(lpc_control_);
    control_panel_layout_->addWidget(post_control_);
    main_layout_->addWidget(control_panel_group_);

    // The main layouts final two rows are occupied by waveforms
    input_waveform_ = new AudioWaveformView("Input Signal", 750, 150, this);
    lpc_waveform_ = new AudioWaveformView("Synthesized Signal", 750, 150, this);

    // The waveforms may be
    input_waveform_->setSizePolicy(QSizePolicy::Expanding,
        QSizePolicy::Expanding);

    lpc_waveform_->setSizePolicy(QSizePolicy::Expanding,
        QSizePolicy::Expanding);

    main_layout_->addWidget(input_waveform_);
    main_layout_->addWidget(lpc_waveform_);

    // Menu Bar
    action_export_ = new QAction(this);
    action_export_->setText("Export");
    action_export_->setShortcut(QKeySequence("Ctrl+E"));

    action_open_ = new QAction(this);
    action_open_->setText("Open");
    action_open_->setShortcut(QKeySequence("Ctrl+O"));

    action_save_ = new QAction(this);
    action_save_->setText("Save");
    action_save_->setShortcut(QKeySequence("Ctrl+S"));

    menu_bar_ = new QMenuBar(this);
    auto menu_file = new QMenu(menu_bar_);
    menu_file->setTitle("File");
    setMenuBar(menu_bar_);

    menu_bar_->addAction(menu_file->menuAction());
    menu_file->addAction(action_open_);
    menu_file->addAction(action_save_);
    menu_file->addAction(action_export_);

    player = new QMediaPlayer(this);
    audio_output_ = new QAudioOutput(this);

    input_buffer_ = AudioBuffer();
    lpc_buffer_ = AudioBuffer();

    frame_table_ = {};
    pitch_period_table_ = {};
    pitch_curve_table_ = {};

    configureUiSlots();
    configureUiState();
}

///////////////////////////////////////////////////////////////////////////////
// Qt Slots ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void MainWindow::onOpenFile() {
    auto filepath = QFileDialog::getOpenFileName(this,
        "Open file",
        QDir::homePath(),
        "Audio Files (*.wav *.aif *.aiff *.raw *.wav *.caf *.flac);;" \
        "ASCII Bitstream (*.lpc);;" \
        "Binary Bitstream (*.bin)");

    // Do nothing if user cancels request
    if (filepath.isNull()) {
        qDebug() << "Open file canceled";
        return;
    }

    if (!input_buffer_.empty()) {
        input_buffer_ = AudioBuffer();
    }

    if (!lpc_buffer_.empty()) {
        lpc_buffer_ = AudioBuffer();
    }

    frame_table_.clear();
    pitch_period_table_.clear();
    pitch_curve_table_.clear();

    // Import audio file
    if (filepath.endsWith(".wav", Qt::CaseInsensitive)) {
        // Enable gain normalization by default
        // ui->postGainNormalizeEnable->setChecked(true);

        auto input_buffer_ptr = AudioBuffer::Create(
            filepath.toStdString(), 8000,
            lpc_control_->getAnalysisWindowWidth());

        if (input_buffer_ptr == nullptr) {
            QMessageBox::critical(this, "Error", "Could not read audio file");
            return;
        }

        input_buffer_ = input_buffer_ptr->copy();
        lpc_buffer_ = input_buffer_ptr->copy();

        performPitchAnalysis();
        performLpcAnalysis();
        frame_postprocessor_ = FramePostprocessor(&frame_table_);
        performPostProc();

        configureUiState();
        drawPlots();
        return;
    }

    if (filepath.endsWith(".lpc", Qt::CaseInsensitive) ||
        filepath.endsWith(".bin", Qt::CaseInsensitive)) {
        // Disable gain normalization to preserve original bitstream gain
        // ui->postGainNormalizeEnable->setChecked(false);

        importBitstream(filepath.toStdString());
        frame_postprocessor_ = FramePostprocessor(&frame_table_);
        performPostProc();

        configureUiState();
        drawPlots();
        return;
    }
}

void MainWindow::onSaveBitstream() {
    auto filepath = QFileDialog::getSaveFileName(this,
        "Save bitstream",
        QDir::homePath(),
        "ASCII Bitstream (*.lpc);;" \
        "Binary Bitstream (*.bin)");

    if (filepath.isNull()) {
        qDebug() << "Save bitstream canceled";
        return;
    }

    exportBitstream(filepath.toStdString());
}

/// Export synthesized bitstream to disk
void MainWindow::onExportAudio() {
    // Display file picker
    auto filepath = QFileDialog::getSaveFileName(this,
        "Export audio file",
        QDir::homePath(),
        "Audio Files (*.wav *.aif *.aiff *.raw *.wav *.caf *.flac)");

    if (filepath.isNull()) {
        qDebug() << "Export audio canceled";
        return;
    }

    synthesizer_.render(synthesizer_.getSamples(),
        filepath.toStdString(), lpc_buffer_.getSampleRateHz(),
        lpc_buffer_.getWindowWidthMs());
}

void MainWindow::onInputAudioPlay() {
    if (input_buffer_.empty()) {
        qDebug() << "Requested play, but input buffer is empty";
        return;
    }

    // Generate checksum of buffer to produce unique temporary filename
    //
    // The pre-emphasis alpha coefficient will be included in this computation,
    // as its impact on the buffer may not be significant enough to modify the
    // buffer checksum alone
    char filename[31];
    auto checksum = samplesChecksum(input_buffer_.getSamples());
    snprintf(filename, sizeof(filename), "tmsexpress_render_%x.wav", checksum);

    // Only render audio if this particular buffer does not exist
    auto temp_dir = std::filesystem::temp_directory_path();
    temp_dir.append(filename);
    qDebug() << "Playing " << temp_dir.c_str();
    input_buffer_.render(temp_dir);

    // Setup player and play
    player->setAudioOutput(audio_output_);
    player->setSource(QUrl::fromLocalFile(temp_dir.c_str()));
    audio_output_->setVolume(100);
    player->play();
}

/// Play synthesized bitstream audio
void MainWindow::onLpcAudioPlay() {
    if (frame_table_.empty()) {
        return;
    }

    synthesizer_.synthesize(frame_table_);

    // Generate checksum of buffer to produce unique temporary filename
    //
    // The pre-emphasis alpha coefficient will be included in this computation,
    // as its impact on the buffer may not be significant enough to modify the
    // buffer checksum alone
    char filename[35];

    uint checksum = (!lpc_buffer_.empty()) ?
        samplesChecksum(lpc_buffer_.getSamples()) :
        samplesChecksum(synthesizer_.getSamples());

    snprintf(filename, sizeof(filename), "tmsexpress_lpc_render_%x.wav",
        checksum);

    // Only render audio if this particular buffer does not exist
    auto temp_dir = std::filesystem::temp_directory_path();
    temp_dir.append(filename);

    qDebug() << "Playing " << temp_dir.c_str();

    synthesizer_.render(synthesizer_.getSamples(),
        temp_dir, lpc_buffer_.getSampleRateHz(),
        lpc_buffer_.getWindowWidthMs());

    // Setup player and play
    player->setAudioOutput(audio_output_);
    player->setSource(QUrl::fromLocalFile(temp_dir.c_str()));
    audio_output_->setVolume(100);
    player->play();
}

void MainWindow::onPitchParamEdit() {
    configureUiState();

    if (!input_buffer_.empty()) {
        performPitchAnalysis();
        performLpcAnalysis();

        drawPlots();
    }
}

void MainWindow::onLpcParamEdit() {
    configureUiState();

    if (!lpc_buffer_.empty()) {
        performLpcAnalysis();
        performPostProc();

        drawPlots();
    }
}

void MainWindow::onPostProcEdit() {
    configureUiState();

    if (!frame_table_.empty()) {
        performPostProc();

        drawPlots();
    }
}

///////////////////////////////////////////////////////////////////////////////
// UI Helper Methods //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void MainWindow::configureUiSlots() {
    // Menu bar
    connect(action_open_, &QAction::triggered, this,
        &MainWindow::onOpenFile);

    connect(action_save_, &QAction::triggered, this,
        &MainWindow::onSaveBitstream);

    connect(action_export_, &QAction::triggered, this,
        &MainWindow::onExportAudio);

    // Control panels
    connect(pitch_control_, &ControlPanelPitchView::stateChangeSignal, this,
        &MainWindow::onPitchParamEdit);

    connect(lpc_control_, &ControlPanelLpcView::stateChangeSignal, this,
        &MainWindow::onLpcParamEdit);

    connect(post_control_, &ControlPanelPostView::stateChangeSignal, this,
        &MainWindow::onPostProcEdit);

    // Play buttons
    connect(input_waveform_, &AudioWaveformView::signalPlayButtonPressed, this,
        &MainWindow::onInputAudioPlay);

    connect(lpc_waveform_, &AudioWaveformView::signalPlayButtonPressed, this,
        &MainWindow::onLpcAudioPlay);
}

///////////////////////////////////////////////////////////////////////////////
// UI Helper Methods //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void MainWindow::configureUiState() {
    // Get UI state
    auto disableAudioDependentObject = (input_buffer_.empty());
    auto disableBitstreamDependentObject = frame_table_.empty();

    // Menu bar
    action_save_->setDisabled(disableAudioDependentObject);
    action_save_->setDisabled(disableBitstreamDependentObject);
    action_export_->setDisabled(disableAudioDependentObject);

    // Control panels
    pitch_control_->setDisabled(disableAudioDependentObject);
    lpc_control_->setDisabled(disableAudioDependentObject);
    post_control_->setDisabled(disableBitstreamDependentObject);
}

void MainWindow::drawPlots() {
    input_waveform_->setSamples(input_buffer_.getSamples());

    if (!frame_table_.empty()) {
        auto samples = synthesizer_.synthesize(frame_table_);
        lpc_waveform_->setSamples(samples);

        auto tmp_pitch_curve_table = std::vector<float>(frame_table_.size());
        const auto max_pitch = static_cast<float>(pitch_estimator_.getMaxFrq());

        for (int i = 0; i < static_cast<int>(frame_table_.size()); i++) {
            auto quantized_pitch = static_cast<float>(
                frame_table_[i].quantizedPitch());

            tmp_pitch_curve_table[i] =
                (TE_AUDIO_SAMPLE_RATE / quantized_pitch) / max_pitch;
        }

        lpc_waveform_->setPitchCurve(tmp_pitch_curve_table);

    } else {
        lpc_waveform_->setSamples({});
    }
}

///////////////////////////////////////////////////////////////////////////
// LPC Routines ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void MainWindow::performPitchAnalysis() {
    // Clear tables
    input_buffer_.reset();
    pitch_period_table_.clear();
    pitch_curve_table_.clear();

    // Pre-process
    if (pitch_control_->getHpfEnabled()) {
        filter_.applyHighpass(input_buffer_, pitch_control_->getHpfCutoff());
    }

    if (pitch_control_->getLpfEnabled()) {
        filter_.applyLowpass(input_buffer_, pitch_control_->getLpfCutoff());
    }

    if (pitch_control_->getPreEmphasisEnabled()) {
        filter_.applyPreEmphasis(input_buffer_,
            pitch_control_->getPreEmphasisAlpha());
    }

    pitch_estimator_.setMaxPeriod(pitch_control_->getMinPitchFrq());
    pitch_estimator_.setMinPeriod(pitch_control_->getMaxPitchFrq());

    const auto max_pitch = static_cast<float>(pitch_estimator_.getMaxFrq());

    for (const auto &segment : input_buffer_.getAllSegments()) {
        auto acf = tms_express::Autocorrelation(segment);
        auto period = pitch_estimator_.estimatePeriod(acf);
        auto frq = pitch_estimator_.estimateFrequency(acf) / max_pitch;

        pitch_period_table_.push_back(period);
        pitch_curve_table_.push_back(frq);
    }
}

void MainWindow::performLpcAnalysis() {
    // Clear tables
    lpc_buffer_.reset();
    frame_table_.clear();

    // Re-trigger pitch analysis if window width has changed
    if (lpc_control_->getAnalysisWindowWidth() !=
        input_buffer_.getWindowWidthMs()) {
        //
        input_buffer_.setWindowWidthMs(lpc_control_->getAnalysisWindowWidth());
        lpc_buffer_.setWindowWidthMs(lpc_control_->getAnalysisWindowWidth());

        qDebug() << "Adjusting window width for pitch and LPC buffers";
        performPitchAnalysis();
    }

    // Pre-process
    if (lpc_control_->getHpfEnabled()) {
        qDebug() << "HPF";
        filter_.applyHighpass(lpc_buffer_, lpc_control_->getHpfCutoff());
    }

    if (lpc_control_->getLpfEnabled()) {
        qDebug() << "LPF";
        filter_.applyLowpass(lpc_buffer_, lpc_control_->getLpfCutoff());
    }

    if (lpc_control_->getPreEmphasisEnabled()) {
        qDebug() << "PEF";
        qDebug() << (lpc_buffer_.empty());
        filter_.applyPreEmphasis(lpc_buffer_,
            lpc_control_->getPreEmphasisAlpha());
    }

    for (int i = 0; i < lpc_buffer_.getNSegments(); i++) {
        auto segment = lpc_buffer_.getSegment(i);
        auto acf = tms_express::Autocorrelation(segment);

        auto coeffs = linear_predictor_.computeCoeffs(acf);
        auto gain = linear_predictor_.gain();

        auto period = pitch_period_table_[i];
        auto is_voiced = coeffs[0] < 0;

        frame_table_.emplace_back(period, is_voiced, gain, coeffs);
    }

    frame_postprocessor_ = FramePostprocessor(&frame_table_);
}

void MainWindow::performPostProc() {
    // Clear tables
    frame_postprocessor_.reset();

    // Re-configure post-processor
    frame_postprocessor_.setMaxUnvoicedGainDB(
        post_control_->getMaxUnvoicedGain());

    frame_postprocessor_.setMaxVoicedGainDB(post_control_->getMaxVoicedGain());

    if (post_control_->getGainNormalizationEnabled()) {
        frame_postprocessor_.normalizeGain();
    }

    // Perform either a pitch shift or a fixed-pitch offset
    if (post_control_->getPitchShiftEnabled()) {
        frame_postprocessor_.shiftPitch(post_control_->getPitchShift());

    } else if (post_control_->getPitchOverrideEnabled()) {
        frame_postprocessor_.overridePitch(post_control_->getPitchOverride());
    }

    if (post_control_->getRepeatFramesEnabled()) {
        auto nRepeatFrames = frame_postprocessor_.detectRepeatFrames();
        qDebug() << "Detected " << nRepeatFrames << " repeat frames";
    }

    if (post_control_->getGainShiftEnabled()) {
        frame_postprocessor_.shiftGain(post_control_->getGainShift());
    }

    synthesizer_.synthesize(frame_table_);
}

void MainWindow::importBitstream(const std::string &path) {
    // Determine file extension
    auto filepath = QString::fromStdString(path);
    auto frame_encoder = FrameEncoder();

    if (filepath.endsWith(".lpc")) {
        try {
            frame_encoder.importASCIIFromFile(path);
        } catch (...) {
            QMessageBox::critical(this, "Error",
                "Could not read bitstream. File may be corrupt");
            return;
        }
    } else {
        return;
    }

    frame_table_ = frame_encoder.getFrameTable();
}

void MainWindow::exportBitstream(const std::string& path) {
    auto filepath = QString::fromStdString(path);
    auto frame_encoder = FrameEncoder(frame_table_, true);

    if (filepath.endsWith(".lpc")) {
        auto hex = frame_encoder.toHex();

        std::ofstream lpcOut;
        lpcOut.open(path);
        lpcOut << hex;
        lpcOut.close();

    } else if (filepath.endsWith(".bin")) {
        auto bin = frame_encoder.toBytes();

        std::ofstream binOut(path, std::ios::out | std::ios::binary);
        binOut.write(
            reinterpret_cast<char*>(bin.data()),
            static_cast<int>(bin.size()));
    }
}

///////////////////////////////////////////////////////////////////////////////
// Helper Methods /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int MainWindow::samplesChecksum(std::vector<float> samples) {
    auto buffer_size = samples.size();

    auto checksum_buffer = new float[buffer_size + 1];
    memccpy(checksum_buffer, samples.data(), buffer_size, sizeof(float));

    checksum_buffer[buffer_size] =
        static_cast<char>(lpc_control_->getPreEmphasisAlpha() +
        pitch_control_->getPreEmphasisAlpha());

    auto checksum = CRC::Calculate(checksum_buffer, sizeof(float),
        CRC::CRC_32());

    delete[] checksum_buffer;
    return checksum;
}

};  // namespace tms_express::ui
