// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_CONTROLPANELLPCVIEW_H
#define TMS_EXPRESS_CONTROLPANELLPCVIEW_H

#include "User_Interfaces/Control_Panels/ControlPanelView.hpp"

#include <QWidget>
#include <QtWidgets>

namespace tms_express::ui {

class ControlPanelLpcView: public ControlPanelView {
Q_OBJECT
public:
    explicit ControlPanelLpcView(QWidget *parent = nullptr);

    void reset() override;
    void configureSlots() override;

    // Getters
    float analysisWindowWidth();
    bool getHpfEnabled();
    int getHpfCutoff();
    bool getLpfEnabled();
    int getLpfCutoff();
    bool getPreEmphasisEnabled();
    float getPreEmphasisAlpha();

private:
    QLineEdit *analysisWindowLine;
    QCheckBox *hpf_checkbox_;
    QLineEdit *hpf_line_;
    QCheckBox *lpf_checkbox_;
    QLineEdit *lpf_line_;
    QCheckBox *preemphasis_checkbox_;
    QLineEdit *preemphasis_line_;
};

};  // namespace tms_express

#endif //TMS_EXPRESS_CONTROLPANELLPCVIEW_H
