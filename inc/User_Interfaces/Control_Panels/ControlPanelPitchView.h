//
// Created by Joseph Bellahcen on 6/1/23.
//

#ifndef TMS_EXPRESS_CONTROLPANELPITCHVIEW_H
#define TMS_EXPRESS_CONTROLPANELPITCHVIEW_H

#include "User_Interfaces/Control_Panels/ControlPanelPitchView.h"

#include <QWidget>
#include <QtWidgets>
#include <string>

class ControlPanelPitchView: public QWidget {
Q_OBJECT
public:
    explicit ControlPanelPitchView(QWidget *parent = nullptr);

    void reset();

    bool hpfEnabled();
    int hpfCutoff();

    bool lpfEnabled();
    int lpfCutoff();

    bool preemphEnabled();
    float preemphAlpha();

    int maxPitchFrq();
    int minPitchFrq();

public slots:
    void slotStateChanged();

signals:
    void signalStateChanged();

private:
    QGridLayout *grid;
    QLabel *title;
    QFrame *line1;
    QFrame *line2;

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

#endif //TMS_EXPRESS_CONTROLPANELPITCHVIEW_H
