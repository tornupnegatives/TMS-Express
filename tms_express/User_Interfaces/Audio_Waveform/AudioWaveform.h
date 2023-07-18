// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_AUDIOWAVEFORM_H
#define TMS_EXPRESS_AUDIOWAVEFORM_H

#include <QWidget>
#include <QtWidgets>

#include <string>
#include <vector>

namespace tms_express {

class AudioWaveform : public QWidget {
    Q_OBJECT
public:
    explicit AudioWaveform(QWidget *parent = nullptr);

    void plotPitch(const std::vector<float>& _pitchTable);
    void plotSamples(const std::vector<float>& _samples);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<float> pitchTable;
    std::vector<float> samples;
};

};  // namespace tms_express

#endif //TMS_EXPRESS_AUDIOWAVEFORM_H
