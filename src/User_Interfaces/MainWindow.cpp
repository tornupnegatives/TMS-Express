//
// Created by Joseph Bellahcen on 5/1/23.
//

#include "Audio/AudioBuffer.h"
#include "Audio/AudioFilter.h"
#include "LPC_Analysis/Autocorrelator.h"
#include "User_Interfaces/AudioWaveform.h"
#include "User_Interfaces/MainWindow.h"

#include "CRC.h"
#include "../../gui/ui_mainwindow.h"

#include <QFileDialog>
#include <QMediaPlayer>
#include <QtMultimedia/QAudioOutput>

/// Configure the GUI window
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    // Load compiled .gui file
    setWindowTitle("TMS Express");
    ui->setupUi(this);

    // Menu bar signals
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::importAudioFile);
    //connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(exportBitstream()));

    // Pitch analysis controls
    connect(ui->pitchHpfEnable, &QCheckBox::stateChanged, this, &MainWindow::performPitchAnalysis);
    connect(ui->pitchHpfLine, &QLineEdit::textChanged, this, &MainWindow::performPitchAnalysis);

    connect(ui->pitchLpfEnable, &QCheckBox::stateChanged, this, &MainWindow::performPitchAnalysis);
    connect(ui->pitchLpfLine, &QLineEdit::textChanged, this, &MainWindow::performPitchAnalysis);

    connect(ui->pitchPreemphEnable, &QCheckBox::stateChanged, this, &MainWindow::performPitchAnalysis);
    connect(ui->pitchPreemphLine, &QLineEdit::textChanged, this, &MainWindow::performPitchAnalysis);

    connect(ui->pitchOverrideToggle, &QRadioButton::toggled, this, &MainWindow::performPitchAnalysis);

    // Input audio waveform
    connect(ui->inputAudioPlay, &QPushButton::pressed, this, &MainWindow::playInputAudio);
    audioWaveform = new AudioWaveform(ui->inputAudioWaveformWidget);
    audioWaveform->setMinimumSize(ui->inputAudioWaveformWidget->minimumSize());
    audioWaveform->setMaximumSize(ui->inputAudioWaveformWidget->maximumSize());
    audioWaveform->show();


    // TODO: LPC analysis controls

    // TODO: Synthesized audio waveform

    // TMS Express objects
    audioBuffer = nullptr;
    audioFilter = AudioFilter();

    pitchEstimator.setMaxPeriod(pitchMinFrq());
    pitchEstimator.setMinPeriod(pitchMaxFrq());

    // Analysis buffers
    pitchTable = {};

    // Start the application with all controls disabled, as audio has yet to be imported
    updatePitchControls();
}

/// Delete any persistent data
MainWindow::~MainWindow() {
    delete ui;
    delete audioBuffer;
    delete audioWaveform;
}

///////////////////////////////////////////////////////////////////////////////
//                             Qt UI Slots
///////////////////////////////////////////////////////////////////////////////

/// Import an audio file from the disk using the system file picker
void MainWindow::importAudioFile() {
    auto filePath = QFileDialog::getOpenFileName(this, "Import audio file", QDir::homePath(),
                                                 "Audio Files (*.wav *.aif *.mp3)"
                                                 );

    audioBuffer = new AudioBuffer(filePath.toStdString());
    pitchTable = std::vector<float>(audioBuffer->size());

    performPitchAnalysis();
}

/// Apply filters to audio buffer before performing LPC analysis, then refresh the AudioWaveform plot
void MainWindow::performPitchAnalysis() {
    updatePitchControls();

    if (audioBuffer == nullptr) {
        qDebug() << "Triggered preprocessing but no audio file imported";
        return;
    }

    // Reset all buffer edits, including window width
    audioBuffer->reset();
    audioBuffer->setWindowWidth(windowWidthMs());

    // Apply filters
    if (ui->pitchHpfEnable->isChecked()) {
        audioFilter.highpass(*audioBuffer, pitchHpfCutoff());
    }

    if (ui->pitchLpfEnable->isChecked()) {
        audioFilter.lowpass(*audioBuffer, pitchLpfCutoff());
    }

    if (ui->pitchPreemphEnable->isChecked()) {
        audioFilter.preEmphasis(*audioBuffer, pitchPreemph());
    }

    computePitchTable();
    plotInputAudio();
}

void MainWindow::playInputAudio() {
    if (audioBuffer == nullptr) {
        qDebug() << "Triggered play but no audio file imported";
        return;
    }

    // Generate checksum of audio buffer for unique filename
    char filename[31];
    auto crc = CRC::Calculate(audioBuffer->getSamples().data(), sizeof(float), CRC::CRC_32());
    snprintf(filename, 31, "tmsexpress_render_%x.wav", crc);

    auto tempDir = std::filesystem::temp_directory_path();
    tempDir.append(filename);

    // Only render audio if this version of buffer doesn't already exist
    // Unfortunately, the pre-emphasis filter produces small changes that won't be picked up by this checksum,
    // so it will override this check for the time being
    if (!std::filesystem::exists(tempDir) || ui->pitchPreemphEnable->isChecked()) {
        audioBuffer->render(tempDir);
        qDebug() << "Rendered audio to path: " << tempDir.c_str();
    }

    auto player = new QMediaPlayer();
    auto audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile(tempDir.c_str()));
    audioOutput->setVolume(100);

    player->play();
}

void MainWindow::plotInputAudio() {
    audioWaveform->plotSamples(audioBuffer->getSamples());
    audioWaveform->plotPitch(pitchTable);
}

///////////////////////////////////////////////////////////////////////////////
//                             Analysis functions
///////////////////////////////////////////////////////////////////////////////

void MainWindow::computePitchTable() {
    pitchTable = std::vector<float>(audioBuffer->size());

    for (int i = 0; i < audioBuffer->size(); i++) {
        auto segment = audioBuffer->segment(i);
        auto acf = Autocorrelator::process(segment);

        // Normalize samples by max period
        pitchTable[i] = pitchEstimator.estimateFrequency(acf) / float(pitchMaxFrq());
    }
}

///////////////////////////////////////////////////////////////////////////////
//                             UI state controls
///////////////////////////////////////////////////////////////////////////////

void MainWindow::updatePitchControls() {
    // If no audio file has been imported, do not allow playback
    if (audioBuffer == nullptr) {
        ui->inputAudioPlay->setDisabled(true);
    } else {
        ui->inputAudioPlay->setDisabled(false);
    }

    // Disallow editing text fields if disabled
    if (ui->pitchHpfEnable->isChecked()) {
        ui->pitchHpfLine->setDisabled(false);
    } else {
        ui->pitchHpfLine->setDisabled(true);
    }

    if (ui->pitchLpfEnable->isChecked()) {
        ui->pitchLpfLine->setDisabled(false);
    } else {
        ui->pitchLpfLine->setDisabled(true);
    }

    // Disallow editing text fields if disabled
    if (ui->pitchPreemphEnable->isChecked()) {
        ui->pitchPreemphLine->setDisabled(false);
    } else {
        ui->pitchPreemphLine->setDisabled(true);
    }

    // If pitch override is enabled, disallow pitch interpolation
    if (ui->pitchOverrideToggle->isChecked()) {
        ui->pitchInterpEnable->setDisabled(true);
    } else {
        ui->pitchInterpEnable->setDisabled(false);
    }
}

///////////////////////////////////////////////////////////////////////////////
//                             Pitch UI getters
///////////////////////////////////////////////////////////////////////////////

int MainWindow::pitchHpfCutoff() {
    return ui->pitchHpfLine->text().toInt();
}

int MainWindow::pitchLpfCutoff() {
    return ui->pitchLpfLine->text().toInt();
}

float MainWindow::pitchPreemph() {
    return ui->pitchPreemphLine->text().toFloat();
}

int MainWindow::pitchMaxFrq() {
    return ui->pitchMaxFrqLine->text().toInt();
}

int MainWindow::pitchMinFrq() {
    return ui->pitchMinFrqLine->text().toInt();
}

///////////////////////////////////////////////////////////////////////////////
//                              LPC UI getters
///////////////////////////////////////////////////////////////////////////////

float MainWindow::maxVoicedGainDb() {
    return ui->lpcMaxVoicedGainLine->text().toFloat();
}

float MainWindow::maxUnvoicedGainDb() {
    return ui->lpcMaxUnvoicedGainLine->text().toFloat();
}

float MainWindow::windowWidthMs() {
    return ui->lpcWindowWidth->text().toFloat();
}
