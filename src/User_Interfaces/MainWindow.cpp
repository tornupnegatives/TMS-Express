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
    // Load the compiled .gui file
    setWindowTitle("TMS Express");
    ui->setupUi(this);

    // Menu bar signals
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::importAudioFile);
    //connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(exportBitstream()));

    // Waveform controls
    connect(ui->playAudioButton, &QPushButton::pressed, this, &MainWindow::playAudio);

    // Pre-processing signals
    connect(ui->windowWidthLine, &QLineEdit::textChanged, this, &MainWindow::applyAudioPreprocessing);

    connect(ui->hpfEnable, &QCheckBox::stateChanged, this, &MainWindow::applyAudioPreprocessing);
    connect(ui->hpfLine, &QLineEdit::textChanged, this, &MainWindow::applyAudioPreprocessing);

    connect(ui->lpfEnable, &QCheckBox::stateChanged, this, &MainWindow::applyAudioPreprocessing);
    connect(ui->lpfLine, &QLineEdit::textChanged, this, &MainWindow::applyAudioPreprocessing);

    connect(ui->preEmphEnable, &QCheckBox::stateChanged, this, &MainWindow::applyAudioPreprocessing);
    connect(ui->preEmphLine, &QLineEdit::textChanged, this, &MainWindow::applyAudioPreprocessing);

    connect(ui->showPitchEnable, &QCheckBox::stateChanged, this, &MainWindow::applyAudioPreprocessing);

    // Audio waveform plot
    audioWaveform = new AudioWaveform(ui->audioWaveform);
    audioWaveform->setMinimumSize(750, 150);
    audioWaveform->show();

    // TMS Express objects
    audioBuffer = nullptr;
    audioFilter = AudioFilter();

    pitchEstimator.setMaxPeriod(minPitchHz());
    pitchEstimator.setMinPeriod(maxPitchHz());

    // Analysis buffers
    pitchTable = {};

    // Start the application with all controls disabled, as audio has yet to be imported
    toggleGroupBoxes(true);
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

    applyAudioPreprocessing();
    toggleGroupBoxes(false);
}

/// Apply filters to audio buffer before performing LPC analysis, then refresh the AudioWaveform plot
void MainWindow::applyAudioPreprocessing() {
    if (audioBuffer == nullptr) {
        qDebug() << "Triggered preprocessing but no audio file imported";
        return;
    }

    // Reset all buffer edits, including window width
    audioBuffer->reset();
    audioBuffer->setWindowWidth(windowWidthMs());

    // Apply filters
    if (ui->hpfEnable->isChecked()) {
        audioFilter.highpass(*audioBuffer, highpassFilterCutoffHz());
    }

    if (ui->lpfEnable->isChecked()) {
        audioFilter.lowpass(*audioBuffer, lowpassFilterCutoffHz());
    }

    if (ui->preEmphEnable->isChecked()) {
        audioFilter.preEmphasis(*audioBuffer, preEmphasisAlpha());
    }

    computePitchTable();
    updatePlot();
}

void MainWindow::updatePlot() {
    audioWaveform->plotSamples(audioBuffer->getSamples());

    if (ui->showPitchEnable->isChecked())
        audioWaveform->plotPitch(pitchTable);
    else
        audioWaveform->plotPitch({});
}

void MainWindow::playAudio() {
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
    if (!std::filesystem::exists(tempDir) || ui->preEmphEnable->isChecked()) {
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

///////////////////////////////////////////////////////////////////////////////
//                             Analysis functions
///////////////////////////////////////////////////////////////////////////////

void MainWindow::computePitchTable() {
    pitchTable = std::vector<float>(audioBuffer->size());

    for (int i = 0; i < audioBuffer->size(); i++) {
        auto segment = audioBuffer->segment(i);
        auto acf = Autocorrelator::process(segment);

        // Normalize samples by max period
        pitchTable[i] = pitchEstimator.estimateFrequency(acf) / float(maxPitchHz());
    }
}

///////////////////////////////////////////////////////////////////////////////
//                             UI state controls
///////////////////////////////////////////////////////////////////////////////

void MainWindow::toggleGroupBoxes(bool enabled) {
    ui->showPitchEnable->setDisabled(enabled);
    ui->playAudioButton->setDisabled(enabled);
    ui->preProcGroupBox->setDisabled(enabled);
    ui->bitstreamCtrlGroupBox->setDisabled(enabled);
}

///////////////////////////////////////////////////////////////////////////////
//                               UI getters
///////////////////////////////////////////////////////////////////////////////

float MainWindow::windowWidthMs() {
    return ui->windowWidthLine->text().toFloat();
}

int MainWindow::highpassFilterCutoffHz() {
    return ui->hpfLine->text().toInt();
}

int MainWindow::lowpassFilterCutoffHz() {
    return ui->lpfLine->text().toInt();
}

float MainWindow::preEmphasisAlpha() {
    return ui->preEmphLine->text().toFloat();
}

float MainWindow::maxVoicedGainDb() {
    return ui->maxVoicedGainLine->text().toFloat();
}

float MainWindow::maxUnvoicedGainDb() {
    return ui->maxUnvoicedGainLine->text().toFloat();
}

int MainWindow::maxPitchHz() {
    return ui->maxPitchLine->text().toInt();
}

int MainWindow::minPitchHz() {
    return ui->minPitchLine->text().toInt();
}
