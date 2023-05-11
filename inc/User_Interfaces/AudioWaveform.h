//
// Created by Joseph Bellahcen on 4/28/23.
//

#ifndef TMS_EXPRESS_AUDIOWAVEFORM_H
#define TMS_EXPRESS_AUDIOWAVEFORM_H

#include "qcustomplot/qcustomplot.h"
#include <QtWidgets>
#include <vector>

class AudioWaveform : public QCustomPlot {
public:
    AudioWaveform(QWidget *parent = nullptr);

    void addSamples(std::vector<float> samples);
    void clear();
};


#endif //TMS_EXPRESS_AUDIOWAVEFORM_H
