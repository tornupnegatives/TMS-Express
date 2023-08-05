// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_AUDIOWAVEFORMVIEW_H
#define TMS_EXPRESS_AUDIOWAVEFORMVIEW_H

#include "User_Interfaces/Audio_Waveform/AudioWaveform.hpp"

#include <QWidget>
#include <QtWidgets>

#include <vector>

namespace tms_express {

class AudioWaveformView: public QWidget {
    Q_OBJECT
public:
    AudioWaveformView(std::string title, uint baseWidth, uint baseHeight, QWidget *parent = nullptr);

    void plotPitch(const std::vector<float>& _pitchTable);
    void plotSamples(const std::vector<float>& _samples);

public slots:
    void onPlayButtonPressed();

signals:
    int signalPlayButtonPressed(int val);

private:
    QVBoxLayout *rowsLayout;
    AudioWaveform *waveform;
    QPushButton *playButton;
};

};  // namespace tms_express

#endif //TMS_EXPRESS_AUDIOWAVEFORMVIEW_H
