//
// Created by Joseph Bellahcen on 6/1/23.
//

#ifndef TMS_EXPRESS_CONTROLPANELLPCVIEW_H
#define TMS_EXPRESS_CONTROLPANELLPCVIEW_H

#include <QWidget>
#include <QtWidgets>
#include <string>

class ControlPanelLpcView: public QWidget {
Q_OBJECT
public:
    explicit ControlPanelLpcView(QWidget *parent = nullptr);

    void reset();

    float analysisWindowWidth();

    bool hpfEnabled();
    int hpfCutoff();

    bool lpfEnabled();
    int lpfCutoff();

    bool preemphEnabled();
    float preemphAlpha();

public slots:
    void slotStateChanged();

signals:
    void signalStateChanged();

private:
    QGridLayout *grid;
    QLabel *title;
    QFrame *line1;

    QLabel *analysisWindowLabel;
    QLineEdit *analysisWindowLine;
    QCheckBox *hpfCheckbox;
    QLineEdit *hpfLine;
    QCheckBox *lpfCheckbox;
    QLineEdit *lpfLine;
    QCheckBox *preemphCheckbox;
    QLineEdit *preemphLine;
};

#endif //TMS_EXPRESS_CONTROLPANELLPCVIEW_H
