// Author: Joseph Bellahcen <joeclb@icloud.com>

#include "User_Interfaces/Audio_Waveform/AudioWaveform.h"
#include "User_Interfaces/Audio_Waveform/AudioWaveformView.h"

#include <QWidget>
#include <QtWidgets>

AudioWaveformView::AudioWaveformView(std::string title, uint baseWidth, uint baseHeight, QWidget *parent): QWidget(parent) {
    setMinimumSize(baseWidth, baseHeight);

    rowsLayout = new QVBoxLayout(this);
    auto label = new QLabel(title.c_str());
    label->setMaximumHeight(16);
    rowsLayout->addWidget(label);

    rowsLayout->setContentsMargins(0, 0, 0, 0); // Set margins to 0
    rowsLayout->setSpacing(10); // Set spacing to 10 pixels

    waveform = new AudioWaveform();
    waveform->setMinimumSize(baseWidth * 0.75, baseHeight * 0.75);
    rowsLayout->addWidget(waveform);

    playButton = new QPushButton("Play");
    playButton->setMaximumWidth(64);
    rowsLayout->addWidget(playButton);

    connect(playButton, &QPushButton::released, this, &AudioWaveformView::onPlayButtonPressed);
}

void AudioWaveformView::plotPitch(const std::vector<float> &_pitchTable) {
    waveform->plotPitch(_pitchTable);
}

void AudioWaveformView::plotSamples(const std::vector<float> &_samples) {
    waveform->plotSamples(_samples);
}

void AudioWaveformView::onPlayButtonPressed() {
    emit signalPlayButtonPressed(true);
}