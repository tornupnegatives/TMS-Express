// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_CONTROLPANELPITCHVIEW_H
#define TMS_EXPRESS_CONTROLPANELPITCHVIEW_H

#include "User_Interfaces/Control_Panels/ControlPanelView.h"

#include <QWidget>
#include <QtWidgets>
#include <string>

namespace tms_express {

class ControlPanelPitchView: public ControlPanelView {
Q_OBJECT
public:
    explicit ControlPanelPitchView(QWidget *parent = nullptr);

    void reset() override;
    void configureSlots() override;

    // Getters
    bool hpfEnabled();
    int hpfCutoff();
    bool lpfEnabled();
    int lpfCutoff();
    bool preemphEnabled();
    float preemphAlpha();
    int maxPitchFrq();
    int minPitchFrq();

private:
    QCheckBox *hpfCheckbox;
    QLineEdit *hpfLine;
    QCheckBox *lpfCheckbox;
    QLineEdit *lpfLine;
    QCheckBox *preemphCheckbox;
    QLineEdit *preemphLine;
    QLabel *maxPitchLabel;
    QLineEdit *maxPitchLine;
    QLabel *minPitchLabel;
    QLineEdit *minPitchLine;
};

};  // namespace tms_express

#endif //TMS_EXPRESS_CONTROLPANELPITCHVIEW_H
