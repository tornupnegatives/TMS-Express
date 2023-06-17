//
// Created by Joseph Bellahcen on 6/3/23.
//

#ifndef TMS_EXPRESS_CONTROLPANELPOSTVIEW_H
#define TMS_EXPRESS_CONTROLPANELPOSTVIEW_H

#include <QWidget>
#include <QtWidgets>

class ControlPanelPostView: public QWidget {
Q_OBJECT
public:
    explicit ControlPanelPostView(QWidget *parent = nullptr);

    void reset(bool enableGainNormalization = true);

    bool pitchShiftEnabled();
    int pitchShift();

    bool pitchOverrideEnabled();
    int pitchOverride();

    bool repeatFramesEnabled();

    bool gainShiftEnabled();
    int gainShift();

    bool gainNormalizationEnabled();

    float maxUnvoicedGain();
    float maxVoicedGain();

public slots:
    void slotStateChanged();

signals:
    void signalStateChanged();

private:
    QGridLayout *grid;
    QLabel *title;
    QFrame *line1;
    QFrame *line2;
    QFrame *line3;

    QCheckBox *pitchShiftCheckbox;
    QSlider *pitchShiftSlider;
    QCheckBox *pitchOverrideCheckbox;
    QSlider *pitchOverrideSlider;
    QCheckBox *repeatFramesCheckbox;
    QCheckBox *gainShiftCheckbox;
    QSlider *gainShiftSlider;
    QCheckBox *gainNormalizationCheckbox;
    QLabel *maxUnvoicedGainLabel;
    QLineEdit *maxUnvoicedGainLine;
    QLabel *maxVoicedGainLabel;
    QLineEdit *maxVoicedGainLine;
};

#endif //TMS_EXPRESS_CONTROLPANELPOSTVIEW_H
