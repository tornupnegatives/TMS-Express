//
// Created by Joseph Bellahcen on 5/1/23.
//

#include "Audio/AudioBuffer.h"
#include "Audio/AudioFilter.h"
#include "LPC_Analysis/Autocorrelator.h"
#include "LPC_Analysis/PitchEstimator.h"

#include "User_Interfaces/AudioWaveform.h"
#include "User_Interfaces/MainWindow.h"

#include "CRC.h"

#include "../../gui/ui_MainWindow.h"

#include <QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
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

    // Attach audio waveform plot
    audioWaveform = new AudioWaveform(ui->audioWaveform);
    audioWaveform->setMinimumSize(750, 150);
    audioWaveform->show();

    // TMS Express objects
    audioBuffer = nullptr;
    audioFilter = AudioFilter();

    toggleGroupBoxes(true);
}

MainWindow::~MainWindow() {
    delete ui;
    delete audioBuffer;
    delete audioWaveform;
}

void MainWindow::importAudioFile() {
    auto filePath = QFileDialog::getOpenFileName(this,
                                                 tr("Import audio file"),
                                                 QDir::homePath(),
                                                 tr("Audio Files (*.wav *.aif *.mp3)")
                                                 );

    qDebug() << "Importing " << filePath;
    audioBuffer = new AudioBuffer(filePath.toStdString());
    applyAudioPreprocessing();
    toggleGroupBoxes(false);
}

void MainWindow::applyAudioPreprocessing() {
    if (audioBuffer == nullptr) {
        qDebug() << "Triggered preproc but no audio file imported";
        return;
    }

    audioBuffer->reset();

    auto windowWidth = ui->windowWidthLine->text().toFloat();
    audioBuffer->setWindowWidth(windowWidth);
    qDebug() << "Window width: " << windowWidth;

    if (ui->hpfEnable->isChecked()) {
        auto hpCutoff = ui->hpfLine->text().toInt();
        audioFilter.applyBiquad(*audioBuffer, hpCutoff, AudioFilter::FILTER_HIGHPASS);
        qDebug() << "HPF: " << hpCutoff;
    }

    if (ui->lpfEnable->isChecked()) {
        auto lpCutoff = ui->lpfLine->text().toInt();
        audioFilter.applyBiquad(*audioBuffer, lpCutoff, AudioFilter::FILTER_LOWPASS);
        qDebug() << "LPF: " << lpCutoff;
    }

    if (ui->preEmphEnable->isChecked()) {
        auto alpha = ui->preEmphLine->text().toFloat();
        audioFilter.applyPreemphasis(*audioBuffer, alpha);
        qDebug() << "Pre-emphasis alpha: " << alpha;
    }

    auto pitchEstimator = PitchEstimator(8000,
                                         ui->minPitchLine->text().toInt(),
                                         ui->maxPitchLine->text().toInt()
                                         );

    std::vector<float> pitchTable(audioBuffer->getNSegments());

    if (ui->showPitchEnable->isChecked()) {
        for (int i = 0; i < audioBuffer->getNSegments(); i++) {
            auto segment = audioBuffer->getSegment(i);
            auto acf = Autocorrelator::process(segment);

            // Normalize samples by max period
            pitchTable[i] = pitchEstimator.estimateFrequency(acf) / ui->maxPitchLine->text().toFloat();

        }
    }

    updatePlot(pitchTable);

}

void MainWindow::updatePlot(std::vector<float> pitchTable) {
    audioWaveform->plotSamples(audioBuffer->getSamples());
    audioWaveform->plotPitch(pitchTable);
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
        audioBuffer->exportAudio(tempDir);
        qDebug() << "Rendered audio to path: " << tempDir.c_str();
    }

    auto player = new QMediaPlayer();
    auto audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile(tempDir.c_str()));
    audioOutput->setVolume(100);

    player->play();
}

void MainWindow::toggleGroupBoxes(bool enabled) {
    ui->showPitchEnable->setDisabled(enabled);
    ui->playAudioButton->setDisabled(enabled);
    ui->preProcGroupBox->setDisabled(enabled);
    ui->bitstreamCtrlGroupBox->setDisabled(enabled);
}
