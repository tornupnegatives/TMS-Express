// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_CONTROLPANELLPCVIEW_H
#define TMS_EXPRESS_CONTROLPANELLPCVIEW_H

#include "User_Interfaces/Control_Panels/ControlPanelView.h"

#include <QWidget>
#include <QtWidgets>

namespace tms_express {

class ControlPanelLpcView: public ControlPanelView {
Q_OBJECT
public:
    explicit ControlPanelLpcView(QWidget *parent = nullptr);

    void reset() override;
    void configureSlots() override;

    // Getters
    float analysisWindowWidth();
    bool hpfEnabled();
    int hpfCutoff();
    bool lpfEnabled();
    int lpfCutoff();
    bool preemphEnabled();
    float preemphAlpha();

private:
    QLineEdit *analysisWindowLine;
    QCheckBox *hpfCheckbox;
    QLineEdit *hpfLine;
    QCheckBox *lpfCheckbox;
    QLineEdit *lpfLine;
    QCheckBox *preemphCheckbox;
    QLineEdit *preemphLine;
};

};  // namespace tms_express

#endif //TMS_EXPRESS_CONTROLPANELLPCVIEW_H
