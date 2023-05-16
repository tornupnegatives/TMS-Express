//
// Created by Joseph Bellahcen on 4/28/23.
//

#include "User_Interfaces/AudioWaveform.h"
#include <QtWidgets>
#include <QWidget>
#include <utility>
#include <vector>

AudioWaveform::AudioWaveform(QWidget *parent) : QWidget(parent) {
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::black);
    setAutoFillBackground(true);
    setPalette(pal);

    samples = std::vector<float>();
    pitchTable = std::vector<float>();
}

void AudioWaveform::plotSamples(const std::vector<float>& _samples) {
    samples = _samples;
    repaint();
}

void AudioWaveform::plotPitch(const std::vector<float>& _pitchTable) {
    pitchTable = _pitchTable;
    repaint();
}

void AudioWaveform::paintEvent(QPaintEvent * event) {
    QPainter painter(this);
    painter.setPen(Qt::darkGray);

    // Audio samples axis
    painter.drawLine(0, QWidget::height() / 2, QWidget::width(), QWidget::height() / 2);
    auto penOrigin = float(QWidget::height()) / 2.0f;

    // Plot samples
    if (!samples.empty()) {
        qDebug() << "Painting " << samples.size() << "samples";

        painter.setPen(QColor(255, 128, 0));

        auto penSpacing = float(QWidget::width()) / float(samples.size());

        for (int i = 0; i < samples.size() - 1; i++) {
            auto x1 = float(i) * penSpacing;
            auto y1 = penOrigin + (samples[i]) * penOrigin;
            auto x2 = float(i + 1) * penSpacing;
            auto y2 = penOrigin + (samples[i + 1] * penOrigin);

            painter.drawLine(x1, y1, x2, y2);
        }
    }

    if (!pitchTable.empty()) {
        qDebug() << "Painting " << pitchTable.size() << "pitches";

        painter.setPen(Qt::darkRed);

        auto penSpacing = float(QWidget::width()) / float(pitchTable.size());

        for (int i = 0; i < pitchTable.size(); i++) {
            auto x1 = float(i) * penSpacing;
            auto y1 = float(QWidget::height()) - (pitchTable[i] * penOrigin);
            auto x2 = float(i + 1) * penSpacing;
            auto y2 = float(QWidget::height()) - (pitchTable[i + 1] * penOrigin);

            painter.drawLine(x1, y1, x2, y2);
        }

    }
}
