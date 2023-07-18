///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: AudioWaveform
//
// Description: The AudioWaveform implements a Qt plot of time-domain audio samples and its corresponding pitch-estimate
//              table. Both are displayed on the same graph, with the pitch table occupying the lower half of the canvas
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "User_Interfaces/Audio_Waveform/AudioWaveform.h"

#include <QPainter>
#include <QWidget>
#include <QtWidgets>

#include <string>
#include <vector>

namespace tms_express {

/// Create a new Audio Waveform plot
AudioWaveform::AudioWaveform(QWidget *parent) : QWidget(parent) {
    // Set the plot background to true black
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::black);
    setAutoFillBackground(true);
    setPalette(pal);

    samples = {};
    pitchTable = {};
}

/// Plot audio samples
void AudioWaveform::plotSamples(const std::vector<float>& _samples) {
    samples = _samples;
    repaint();
}

/// Plot pitch table corresponding to audio samples
void AudioWaveform::plotPitch(const std::vector<float>& _pitchTable) {
    pitchTable = _pitchTable;
    repaint();
}

/// Paint the plot
///
/// \note Called under the hood by Qt, not by the programmer
///
/// \param event QPaintEvent
void AudioWaveform::paintEvent(QPaintEvent * event) {
    auto width = QWidget::width();
    auto height = QWidget::height();

    // Plot axis
    QPainter painter(this);
    painter.setPen(Qt::darkGray);

    painter.drawLine(0, height / 2, width, height / 2);

    // Set plot origin to center of Y-axis
    auto penOrigin = float(height) / 2.0f;

    // Plot samples
    if (!samples.empty()) {
        painter.setPen(QColor(255, 128, 0));
        auto penSpacing = float(width) / float(samples.size());

        for (int i = 0; i < samples.size() - 1; i++) {
            auto x1 = float(i) * penSpacing;
            auto y1 = penOrigin + (samples[i]) * penOrigin;
            auto x2 = float(i + 1) * penSpacing;
            auto y2 = penOrigin + (samples[i + 1] * penOrigin);

            painter.drawLine(int(x1), int(y1), int(x2), int(y2));
        }
    }

    if (!pitchTable.empty()) {
        painter.setPen(Qt::darkRed);
        auto penSpacing = float(width) / float(pitchTable.size());

        for (int i = 0; i < pitchTable.size(); i++) {
            auto x1 = float(i) * penSpacing;
            auto y1 = float(QWidget::height()) - (pitchTable[i] * penOrigin);
            auto x2 = float(i + 1) * penSpacing;
            auto y2 = float(QWidget::height()) - (pitchTable[i + 1] * penOrigin);

            painter.drawLine(int(x1), int(y1), int(x2), int(y2));
        }
    }
}

};  // namespace tms_express
