// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#include "User_Interfaces/Audio_Waveform/AudioWaveform.hpp"

#include <QWidget>
#include <QPainter>

#include <string>
#include <vector>

namespace tms_express::ui {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AudioWaveform::AudioWaveform(QWidget *parent) : QWidget(parent) {
    // Set the plot background to true black
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::black);
    setAutoFillBackground(true);
    setPalette(pal);

    samples_ = {};
    pitch_curve_ = {};
}

///////////////////////////////////////////////////////////////////////////////
// Accessors //////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void AudioWaveform::setSamples(const std::vector<float>& samples) {
    samples_ = samples;
    repaint();
}

/// Plot pitch table corresponding to audio samples
void AudioWaveform::setPitchCurve(const std::vector<float>& pitch_curve) {
    pitch_curve_ = pitch_curve;
    repaint();
}

///////////////////////////////////////////////////////////////////////////////
// Qt Widget Helpers //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void AudioWaveform::paintEvent([[maybe_unused]] QPaintEvent * event) {
    const auto width = QWidget::width();
    const auto height = QWidget::height();

    // Consider the origin of the plot (y = 0) as the vertical center of the
    // widget, and draw an axis line through it
    const float origin = static_cast<float>(height) / 2.0f;

    QPainter painter(this);
    painter.setPen(Qt::darkGray);
    painter.drawLine(0, origin, width, origin);

    // Plot samples
    if (!samples_.empty()) {
        painter.setPen(QColor(255, 128, 0));
        const float spacing = static_cast<float>(width) /
            static_cast<float>(samples_.size());

        for (int i = 0; i < static_cast<int>(samples_.size()) - 1; i++) {
            float x_1 = static_cast<float>(i) * spacing;
            float y_1 = origin + (samples_[i] * origin);

            float x_2 = static_cast<float>(i + 1) * spacing;
            float y_2 = origin + (samples_[i + 1] * origin);

            painter.drawLine(x_1, y_1, x_2, y_2);
        }
    }

    // Plot pitch curve underneath the samples
    if (!pitch_curve_.empty()) {
        painter.setPen(Qt::darkRed);
        const float spacing = static_cast<float>(width) /
            static_cast<float>(pitch_curve_.size());

        for (int i = 0; i < static_cast<int>(pitch_curve_.size()); i++) {
            float x_1 = static_cast<float>(i) * spacing;
            float y_1 = height - (pitch_curve_[i] * origin);

            float x_2 = static_cast<float>(i + 1) * spacing;
            float y_2 = height - (pitch_curve_[i + 1] * origin);

            painter.drawLine(x_1, y_1, x_2, y_2);
        }
    }
}

};  // namespace tms_express::ui
