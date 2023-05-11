//
// Created by Joseph Bellahcen on 5/1/23.
//
#include "Audio/AudioBuffer.h"
#include "Audio/AudioFilter.h"

#include "User_Interfaces/AudioWaveform.h"
#include "User_Interfaces/MainWindow.h"

#include "../../gui/ui_MainWindow.h"

#include <QAction>
#include <QDir>
#include <QFileDialog>
#include <string.h>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Menu bar signals
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::importAudioFile);
    //connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(exportBitstream()));

    // Pre-processing signals
    connect(ui->windowWidthLine, &QLineEdit::textChanged, this, &MainWindow::applyAudioPreprocessing);

    connect(ui->hpfEnable, &QCheckBox::stateChanged, this, &MainWindow::applyAudioPreprocessing);
    connect(ui->hpfLine, &QLineEdit::textChanged, this, &MainWindow::applyAudioPreprocessing);

    connect(ui->lpfEnable, &QCheckBox::stateChanged, this, &MainWindow::applyAudioPreprocessing);
    connect(ui->lpfLine, &QLineEdit::textChanged, this, &MainWindow::applyAudioPreprocessing);

    connect(ui->preEmphEnable, &QCheckBox::stateChanged, this, &MainWindow::applyAudioPreprocessing);
    connect(ui->preEmphLine, &QLineEdit::textChanged, this, &MainWindow::applyAudioPreprocessing);

    // Attach audio waveform plot
    audioWaveform = new AudioWaveform(ui->audioWaveform);
    audioWaveform->setMinimumSize(750, 150);
    audioWaveform->show();

    // TMS Express objects
    audioBuffer = nullptr;
    audioFilter = AudioFilter();
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
    audioWaveform->addSamples(audioBuffer->getSamples());
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

    audioWaveform->clear();
    audioWaveform->addSamples(audioBuffer->getSamples());
}
