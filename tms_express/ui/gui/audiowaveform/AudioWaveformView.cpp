// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#include "ui/gui/audiowaveform/AudioWaveformView.hpp"

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QPushButton>

#include "ui/gui/audiowaveform/AudioWaveform.hpp"

namespace tms_express::ui {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AudioWaveformView::AudioWaveformView(std::string title, int base_width,
    int base_height, QWidget *parent): QWidget(parent) {
    //
    // Widget properties
    setMinimumSize(base_width, base_height);

    // Layout properties
    rows_layout = new QVBoxLayout(this);
    rows_layout->setContentsMargins(0, 0, 0, 0);
    rows_layout->setSpacing(10);

    // Widget title, displayed above layout
    auto label = new QLabel(title.c_str(), this);
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    rows_layout->addWidget(label);

    // Audio Waveform plot
    waveform = new AudioWaveform(this);
    waveform->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rows_layout->addWidget(waveform);

    // Audio play button
    play_button = new QPushButton("Play", this);
    play_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    play_button->setFixedWidth(64);
    rows_layout->addWidget(play_button);

    connect(play_button, &QPushButton::released, this,
        &AudioWaveformView::onPlayButtonPressed);
}

///////////////////////////////////////////////////////////////////////////////
// Accessors //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void AudioWaveformView::setSamples(const std::vector<float> &samples) {
    waveform->setSamples(samples);
}

void AudioWaveformView::setPitchCurve(const std::vector<float> &pitch_curve) {
    waveform->setPitchCurve(pitch_curve);
}

///////////////////////////////////////////////////////////////////////////
// Qt Slots ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void AudioWaveformView::onPlayButtonPressed() {
    emit signalPlayButtonPressed(true);
}

};  // namespace tms_express::ui
