//
// Created by Joseph Bellahcen on 5/1/23.
//

#include "Audio/AudioBuffer.h"
#include "Frame_Encoding/Frame.h"
#include "Frame_Encoding/FramePostprocessor.h"
#include "LPC_Analysis/Autocorrelator.h"
#include "User_Interfaces/AudioWaveform.h"
#include "User_Interfaces/MainWindow.h"

#include "CRC.h"
#include "../../gui/ui_mainwindow.h"

#include <QFileDialog>
#include <QMediaPlayer>
#include <QtMultimedia/QAudioOutput>

/// Setup the main window of the application
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    // Load compiled .gui file
    ui->setupUi(this);

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);

    inputWaveform = new AudioWaveform();
    ui->inputWaveformLayout->insertWidget(1, inputWaveform);
    inputWaveform->show();

    lpcWaveform = new AudioWaveform();
    ui->outputWaveformLayout->insertWidget(1, lpcWaveform);
    lpcWaveform->show();

    inputBuffer = nullptr;
    lpcBuffer = nullptr;

    frameTable = {};
    pitchPeriodTable = {};
    pitchFrqTable = {};

    configureUiSlots();
    configureUiState();

    // Enable default lowpass filter for pitch estimation
    ui->pitchLpfEnable->setChecked(true);
    ui->pitchLpfLine->setText("800");

    // Enable initial pre-emphasis filter for LPC analysis
    ui->lpcPreemphEnable->setChecked(true);

}

/// Free pointers associated with UI
MainWindow::~MainWindow() {
    delete player;
    delete audioOutput;

    delete inputWaveform;
    delete lpcWaveform;

    delete inputBuffer;
    delete lpcBuffer;
}

///////////////////////////////////////////////////////////////////////////////
//                              UI Helpers
///////////////////////////////////////////////////////////////////////////////

/// Connect UI elements to member functions
void MainWindow::configureUiSlots() {
    // Menu bar
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpenFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onSaveBitstream);
    connect(ui->actionExport, &QAction::triggered, this, &MainWindow::onExportAudio);

    // Play buttons
    connect(ui->inputAudioPlay, &QPushButton::pressed, this, &MainWindow::onInputAudioPlay);
    connect(ui->lpcAudioPlay, &QPushButton::pressed, this, &MainWindow::onLpcAudioPlay);

    // Control panel (Pitch Estimator)
    connect(ui->pitchHpfEnable, &QCheckBox::stateChanged, this, &MainWindow::onPitchParamEdit);
    connect(ui->pitchHpfLine, &QLineEdit::editingFinished, this, &MainWindow::onPitchParamEdit);
    connect(ui->pitchLpfEnable, &QCheckBox::stateChanged, this, &MainWindow::onPitchParamEdit);
    connect(ui->pitchLpfLine, &QLineEdit::editingFinished, this, &MainWindow::onPitchParamEdit);
    connect(ui->pitchPreemphEnable, &QCheckBox::stateChanged, this, &MainWindow::onPitchParamEdit);
    connect(ui->pitchPreemphLine, &QLineEdit::editingFinished, this, &MainWindow::onPitchParamEdit);
    connect(ui->pitchMaxFrqLine, &QLineEdit::editingFinished, this, &MainWindow::onPitchParamEdit);
    connect(ui->pitchMinFrqLine, &QLineEdit::editingFinished, this, &MainWindow::onPitchParamEdit);

    // Control panel (Linear Predictor)
    connect(ui->lpcWindowWidthLine, &QLineEdit::editingFinished, this, &MainWindow::onLpcParamEdit);
    connect(ui->lpcHpfEnable, &QCheckBox::stateChanged, this, &MainWindow::onLpcParamEdit);
    connect(ui->lpcHpfLine, &QLineEdit::editingFinished, this, &MainWindow::onLpcParamEdit);
    connect(ui->lpcLpfEnable, &QCheckBox::stateChanged, this, &MainWindow::onLpcParamEdit);
    connect(ui->lpcLpfLine, &QLineEdit::editingFinished, this, &MainWindow::onLpcParamEdit);
    connect(ui->lpcPreemphEnable, &QCheckBox::stateChanged, this, &MainWindow::onLpcParamEdit);
    connect(ui->lpcPreemphLine, &QLineEdit::editingFinished, this, &MainWindow::onLpcParamEdit);
    connect(ui->lpcMaxUnvoicedGainLine, &QLineEdit::editingFinished, this, &MainWindow::onLpcParamEdit);
    connect(ui->lpcMaxVoicedGainLine, &QLineEdit::editingFinished, this, &MainWindow::onLpcParamEdit);

    // Control panel (Frame Post-Processor)
    //connect(ui->postPitchOffsetToggle, &QRadioButton::clicked, this, &MainWindow::onPostProcEdit);
    //connect(ui->postPitchOffsetLine, &QLineEdit::editingFinished, this, &MainWindow::onPostProcEdit);
    //connect(ui->postPitchOverrideToggle, &QRadioButton::clicked, this, &MainWindow::onPostProcEdit);
    //connect(ui->postPitchOffsetLine, &QLineEdit::editingFinished, this, &MainWindow::onPostProcEdit);
    connect(ui->postPitchInterpEnable, &QCheckBox::stateChanged, this, &MainWindow::onPostProcEdit);
    connect(ui->postRepeatFramesEnable, &QCheckBox::stateChanged, this, &MainWindow::onPostProcEdit);
}

/// Toggle UI elements based on the state of the application
void MainWindow::configureUiState() {
    // Get UI state
    auto disableAudioDependentObject = !isAudioFileLoaded();
    auto disableBitstreamDependentObject = !isBitstreamLoaded();

    auto disablePitchControl = disableAudioDependentObject;
    auto disableLpcControl = disableBitstreamDependentObject;

    // Menu bar
    ui->actionSave->setDisabled(disableAudioDependentObject);
    ui->actionExport->setDisabled(disableAudioDependentObject);

    // Play buttons
    ui->inputAudioPlay->setDisabled(disableAudioDependentObject);
    ui->lpcAudioPlay->setDisabled(disableBitstreamDependentObject);

    // Control panel
    ui->pitchHpfLine->setDisabled(disablePitchControl || !ui->pitchHpfEnable->isChecked());
    ui->pitchLpfLine->setDisabled(disablePitchControl || !ui->pitchLpfEnable->isChecked());
    ui->pitchPreemphLine->setDisabled(disablePitchControl || !ui->pitchPreemphEnable->isChecked());
    ui->pitchMaxFrqLine->setDisabled(disablePitchControl);
    ui->pitchMinFrqLine->setDisabled(disablePitchControl);

    ui->lpcWindowWidthLine->setDisabled(disableLpcControl);
    ui->lpcHpfLine->setDisabled(disableLpcControl || !ui->lpcHpfEnable->isChecked());
    ui->lpcLpfLine->setDisabled(disableLpcControl || !ui->lpcLpfEnable->isChecked());
    ui->lpcPreemphLine->setDisabled(disableLpcControl || !ui->lpcPreemphEnable->isChecked());
    ui->lpcMaxUnvoicedGainLine->setDisabled(disableLpcControl);
    ui->lpcMaxVoicedGainLine->setDisabled(disableLpcControl);

    //ui->postPitchOffsetToggle->setDisabled(disableLpcControl || !ui->postPitchOverrideToggle->isChecked());
    //ui->postPitchOverrideToggle->setDisabled(disableLpcControl || !ui->postPitchOffsetToggle->isChecked());
    ui->postPitchInterpEnable->setDisabled(disableLpcControl);
    ui->postRepeatFramesEnable->setDisabled(disableLpcControl);

    // Allow un-checking of radio buttons
    /*
    if (ui->postPitchOffsetToggle->isEnabled() && ui->postPitchOffsetToggle->isChecked() && ui->postPitchOffsetToggle->isDown()) {
        ui->postPitchOffsetToggle->setChecked(false);
    }

    if (ui->postPitchOverrideToggle->isEnabled() && ui->postPitchOverrideToggle->isChecked() && ui->postPitchOverrideToggle->isDown()) {
        ui->postPitchOverrideToggle->setChecked(false);
    }
     */
}

/// Draw the input and output signal waveforms, along with an abstract representation of their associated pitch data
void MainWindow::drawPlots() {
    if (isAudioFileLoaded()) {
        inputWaveform->plotSamples(inputBuffer->getSamples());
        inputWaveform->plotPitch(pitchFrqTable);
    }

    if (isBitstreamLoaded()) {
        lpcWaveform->plotSamples(synthesizer.samples());

        auto framePitchTable = std::vector<float>(frameTable.size());
        for (int i = 0; i < frameTable.size(); i++)
            framePitchTable[i] = (8000.0f / float(frameTable[i].quantizedPitch())) / float(pitchMaxFrq());

        lpcWaveform->plotPitch(framePitchTable);
    }
}

///////////////////////////////////////////////////////////////////////////////
//                              UI Slots
///////////////////////////////////////////////////////////////////////////////

void MainWindow::onOpenFile() {
    auto filePath = QFileDialog::getOpenFileName(this, "Open file",
                                                 QDir::homePath(),
                                                 "WAV Audio (*.wav);;" \
                                                 "Bitstream (*.lpc *.h *.json)"
    );

    // Do nothing if user cancels request
    if (filePath.isNull()) {
        qDebug() << "Open file canceled";
        return;
    }

    // Import audio file
    if (filePath.endsWith(".wav", Qt::CaseInsensitive)) {
        if (isAudioFileLoaded()) {
            delete inputBuffer;
            delete lpcBuffer;
        }

        inputBuffer = new AudioBuffer(filePath.toStdString(), 8000, lpcWindowWidth());
        lpcBuffer = new AudioBuffer(filePath.toStdString(), 8000, lpcWindowWidth());

        performPitchAnalysis();
        performLpcAnalysis();
        performPostProc();

        configureUiState();
        drawPlots();
        return;
    }

    if (filePath.endsWith(".lpc", Qt::CaseInsensitive)) {
        if (isAudioFileLoaded()) {
            delete inputBuffer;
            delete lpcBuffer;
        }

        frameTable.clear();

        performBitstreamParsing(filePath.toStdString());
        performPostProc();

        configureUiState();
        drawPlots();
        return;
    }
}

/// Save bitstream to disk
void MainWindow::onSaveBitstream() {
    auto filePath = QFileDialog::getSaveFileName(this, "Save bitstream",
                                                 QDir::homePath(),
                                                 "Bitstream Files (*.lpc *.h *.json)"
    );

    if (filePath.isNull()) {
        qDebug() << "Save bitstream canceled";
        return;
    }

    exportBitstream(filePath.toStdString());
}

/// Export synthesized bitstream to disk
void MainWindow::onExportAudio() {
    // Display file picker
    auto filePath = QFileDialog::getSaveFileName(this, "Export audio file",
                                                 QDir::homePath(),
                                                 "Audio Files (*.wav *.aif *.aiff *.raw *.wav *.caf *.flac)"
    );

    if (filePath.isNull()) {
        qDebug() << "Export audio canceled";
        return;
    }

    synthesizer.render(filePath.toStdString());
}

/// Play contents of input buffer
void MainWindow::onInputAudioPlay() {
    if (!isAudioFileLoaded()) {
        qDebug() << "Requested play, but input buffer is null";
        return;
    }

    // Generate checksum of buffer to produce unique temporary filename
    //
    // The pre-emphasis alpha coefficient will be included in this computation, as its impact on the buffer may not
    // be significant enough to modify the buffer checksum alone
    char filename[31];
    auto checksum = samplesChecksum(inputBuffer->getSamples());
    snprintf(filename, 31, "tmsexpress_render_%x.wav", checksum);

    // Only render audio if this particular buffer does not exist
    auto tempDir = std::filesystem::temp_directory_path();
    tempDir.append(filename);
    qDebug() << "Playing " << tempDir.c_str();
    inputBuffer->render(tempDir);

    // Setup player and play
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile(tempDir.c_str()));
    audioOutput->setVolume(100);
    player->play();
}

/// Play synthesized bitstream audio
void MainWindow::onLpcAudioPlay() {
    if (!isBitstreamLoaded()) {
        return;
    }

    synthesizer.synthesize(frameTable);

    // Generate checksum of buffer to produce unique temporary filename
    //
    // The pre-emphasis alpha coefficient will be included in this computation, as its impact on the buffer may not
    // be significant enough to modify the buffer checksum alone
    char filename[35];

    uint checksum = (lpcBuffer != nullptr) ? samplesChecksum(lpcBuffer->getSamples()) : samplesChecksum(synthesizer.samples());
    snprintf(filename, 35, "tmsexpress_lpc_render_%x.wav", checksum);

    // Only render audio if this particular buffer does not exist
    auto tempDir = std::filesystem::temp_directory_path();
    tempDir.append(filename);
    qDebug() << "Playing " << tempDir.c_str();
    synthesizer.render(tempDir);

    // Setup player and play
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile(tempDir.c_str()));
    audioOutput->setVolume(100);
    player->play();
}

/// Re-perform pitch and LPC analysis when pitch controls are changed
void MainWindow::onPitchParamEdit() {
    configureUiState();

    if (isAudioFileLoaded()) {
        inputBuffer->reset();

        performPitchAnalysis();
        performLpcAnalysis();

        drawPlots();
    }
}

/// Re-perform LPC analysis when LPC controls are changed
void MainWindow::onLpcParamEdit() {
    configureUiState();

    if (isBitstreamLoaded()) {
        lpcBuffer->reset();

        performLpcAnalysis();
        performPostProc();

        drawPlots();
    }
}

/// Re-perform LPC analysis when bitstream controls are changed
void MainWindow::onPostProcEdit() {
    configureUiState();

    if (isBitstreamLoaded()) {
        performPostProc();

        drawPlots();
    }
}

///////////////////////////////////////////////////////////////////////////////
//                              Metadata
///////////////////////////////////////////////////////////////////////////////

/// Compute checksum of a vector of floating-point samples to uniquely identify it
///
/// \note   The checksum is used to generate a unique filename, which is stored temporarily to facilitate playback of
///         rendered audio. In addition to the samples from the vector, this checksum also incorporates the pre-emphasis
///         filter coefficients of both the input audio and synthesized bitstream buffers, as applying the small
///         pre-emphasis filter to floating point samples may not impact the checksum.
///
/// \param samples Samples of which to compute checksum
/// \return Checksum
unsigned int MainWindow::samplesChecksum(std::vector<float> samples) {
    auto bufferSize = int(samples.size());
    auto checksumBuffer = (float *) malloc(sizeof(float) * (bufferSize + 1));
    memccpy(checksumBuffer, samples.data(), bufferSize, sizeof(float));
    checksumBuffer[bufferSize] = char(lpcPreemph() + pitchPreemph());

    auto checksum = CRC::Calculate(checksumBuffer, sizeof(float), CRC::CRC_32());

    free(checksumBuffer);
    return checksum;
}

/// Return whether an audio file has been loaded from the disk
///
/// \note If an audio file has been loaded, a bitstream has also been generated and synthesized
///
/// \return Whether an audio file has been loaded from the disk
bool MainWindow::isAudioFileLoaded() {
    return (inputBuffer != nullptr);
}

/// Return whether a bitstream file has been loaded from the disk
///
/// \warning A bitstream file having been loaded does not mean that the input and LPC buffers are not null
///
/// \return Whether an audio file has been loaded from the disk
bool MainWindow::isBitstreamLoaded() {
    return (!frameTable.empty());
}

///////////////////////////////////////////////////////////////////////////////
//                              Data Maniuplation
///////////////////////////////////////////////////////////////////////////////

/// Apply filters to input buffer and perform pitch analysis to populate the pitch tables
void MainWindow::performPitchAnalysis() {
    // Clear tables
    pitchPeriodTable.clear();
    pitchFrqTable.clear();

    // Pre-process
    if (ui->pitchHpfEnable->isChecked()) {
        filter.highpass(*inputBuffer, pitchHpfCutoff());
    }

    if (ui->pitchLpfEnable->isChecked()) {
        filter.lowpass(*inputBuffer, pitchLpfCutoff());
    }

    if (ui->pitchPreemphEnable->isChecked()) {
        filter.preEmphasis(*inputBuffer, pitchPreemph());
    }

    pitchEstimator.setMaxPeriod(pitchMinFrq());
    pitchEstimator.setMinPeriod(pitchMaxFrq());

    for (const auto &segment : inputBuffer->segments()) {
        auto acf = Autocorrelator::process(segment);
        auto pitchPeriod = pitchEstimator.estimatePeriod(acf);
        auto pitchFrq = pitchEstimator.estimateFrequency(acf) / float(pitchMaxFrq());

        pitchPeriodTable.push_back(pitchPeriod);
        pitchFrqTable.push_back(pitchFrq);
    }
}

/// Apply filters to input buffer and perform LPC analysis to populate the frame table
///
/// \note   This function may re-trigger pitch analysis if the window width has been modified, as both the pitch and
///         frame tables must share segment boundaries
void MainWindow::performLpcAnalysis() {
    // Clear tables
    frameTable.clear();

    // Re-trigger pitch analysis if window width has changed
    if (lpcWindowWidth() != inputBuffer->getWindowWidth()) {
        inputBuffer->setWindowWidth(lpcWindowWidth());
        lpcBuffer->setWindowWidth(lpcWindowWidth());

        qDebug() << "Adjusting window width for pitch and LPC buffers";
        performPitchAnalysis();
    }

    // Pre-process
    if (ui->lpcHpfEnable->isChecked()) {
        filter.highpass(*lpcBuffer, lpcHpfCutoff());
    }

    if (ui->lpcLpfEnable->isChecked()) {
        filter.lowpass(*lpcBuffer, lpcLpfCutoff());
    }

    if (ui->lpcPreemphEnable->isChecked()) {
        filter.preEmphasis(*lpcBuffer, lpcPreemph());
    }

    for (int i = 0; i < lpcBuffer->size(); i++) {
        auto segment = lpcBuffer->segment(i);
        auto acf = Autocorrelator::process(segment);

        auto coeffs = linearPredictor.reflectorCoefficients(acf);
        auto gain = linearPredictor.gain();

        auto pitchPeriod = pitchPeriodTable[i];
        auto isVoiced = coeffs[0] < 0;

        frameTable.emplace_back(pitchPeriod, isVoiced, gain, coeffs);
    }
}

/// Perform post-processing on and synthesize bitstream from frame table
void MainWindow::performPostProc() {
    auto framePostProcessor = FramePostprocessor(&frameTable, lpcMaxVoicedGain(), lpcMaxUnvoicedGain());
    framePostProcessor.normalizeGain();

    if (ui->postRepeatFramesEnable->isChecked()) {
        auto nRepeatFrames = framePostProcessor.detectRepeatFrames();
        qDebug() << "Detected " << nRepeatFrames << " repeat frames";
    }

    synthesizer.synthesize(frameTable);
}

/// Import bitstream file from the disk and populate the frame table
void MainWindow::performBitstreamParsing(const std::string &path) {
    // Determine file extension
    std::string extension = path.substr(path.size() - 3, 4);

    // Ensure string is lowercase
    for (char &c : extension) {
        c = char(std::tolower(c));
    }

    auto frameEncoder = FrameEncoder();

    if (extension == "lpc") {
        frameEncoder.importFromAscii(path);
    } else if (extension == "h") {
        //frameEncoder.importFromEmbedded(path);
    } else if (extension == "json") {
        //frameEncoder.importFromJson(path);
    } else {
        return;
    }

    frameTable = frameEncoder.frameTable();
}

// TODO: Implement
void MainWindow::exportBitstream(const std::string &path) {
    return;
}

///////////////////////////////////////////////////////////////////////////////
//                              UI Getters
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

int MainWindow::pitchMinFrq() {
    return ui->pitchMinFrqLine->text().toInt();
}

int MainWindow::pitchMaxFrq() {
    return ui->pitchMaxFrqLine->text().toInt();
}

float MainWindow::lpcWindowWidth() {
    return ui->lpcWindowWidthLine->text().toFloat();
}

int MainWindow::lpcHpfCutoff() {
    return ui->lpcHpfLine->text().toInt();
}

int MainWindow::lpcLpfCutoff() {
    return ui->lpcLpfLine->text().toInt();
}

float MainWindow::lpcPreemph() {
    return ui->pitchPreemphLine->text().toFloat();
}

float MainWindow::lpcMaxUnvoicedGain() {
    return ui->lpcMaxUnvoicedGainLine->text().toFloat();
}

float MainWindow::lpcMaxVoicedGain() {
    return ui->lpcMaxVoicedGainLine->text().toFloat();
}
