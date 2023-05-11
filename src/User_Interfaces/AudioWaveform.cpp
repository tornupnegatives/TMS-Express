//
// Created by Joseph Bellahcen on 4/28/23.
//

#include "User_Interfaces/AudioWaveform.h"
#include "qcustomplot/qcustomplot.h"
#include <QtWidgets>
#include <vector>

AudioWaveform::AudioWaveform(QWidget *parent) : QCustomPlot(parent) {
    addGraph();
    //setSize
    //setFixedSize(750, 200);
    //setBackground(QBrush(QColor(0, 0, 0, 50)));
}

void AudioWaveform::addSamples(std::vector<float> samples) {
    // Populate QVectors with samples
    auto size = samples.size();
    QVector<double> y(size);
    QVector<double> x(size);

    for (int i = 0; i < size; i++) {
        x[i] = double(i);
        y[i] = samples[i];
    }

    graph(0)->setData(x, y);
    xAxis->setRange(0, size);
    yAxis->setRange(-1, 1);
    replot();
}

void AudioWaveform::clear() {
    graph(0)->data().data()->clear();
}
