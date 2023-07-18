// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_CONTROLPANELPOSTVIEW_H
#define TMS_EXPRESS_CONTROLPANELPOSTVIEW_H

#include "User_Interfaces/Control_Panels/ControlPanelView.h"

#include <QWidget>
#include <QtWidgets>

namespace tms_express {

class ControlPanelPostView: public ControlPanelView {
Q_OBJECT
public:
    explicit ControlPanelPostView(QWidget *parent = nullptr);

    void reset() override;
    void configureSlots() override;

    // Getters
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

private:
    QCheckBox *pitchShiftCheckbox;
    QSlider *pitchShiftSlider;
    QCheckBox *pitchOverrideCheckbox;
    QSlider *pitchOverrideSlider;
    QCheckBox *repeatFramesCheckbox;
    QCheckBox *gainShiftCheckbox;
    QSlider *gainShiftSlider;
    QCheckBox *gainNormalizationCheckbox;
    QLineEdit *maxUnvoicedGainLine;
    QLineEdit *maxVoicedGainLine;
};

};  // namespace tms_express

#endif //TMS_EXPRESS_CONTROLPANELPOSTVIEW_H
