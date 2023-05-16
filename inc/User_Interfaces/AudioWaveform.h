//
// Created by Joseph Bellahcen on 4/28/23.
//

#ifndef TMS_EXPRESS_AUDIOWAVEFORM_H
#define TMS_EXPRESS_AUDIOWAVEFORM_H

#include <QtWidgets>
#include <vector>

class AudioWaveform : public QWidget {
public:
    AudioWaveform(QWidget *parent = nullptr);

    void plotSamples(const std::vector<float>& _samples);
    void plotPitch(const std::vector<float>& _pitchTable);

protected:
    void paintEvent(QPaintEvent* event);

private:
    std::vector<float> samples;
    std::vector<float> pitchTable;
};


#endif //TMS_EXPRESS_AUDIOWAVEFORM_H
