// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_CONTROLPANELVIEW_H
#define TMS_EXPRESS_CONTROLPANELVIEW_H

#include <QWidget>
#include <QtWidgets>

#include <string>

class ControlPanelView: public QWidget {
Q_OBJECT
public:
    ControlPanelView(const std::string &panelTitle, QWidget *parent = nullptr);

    virtual void reset() = 0;
    virtual void configureSlots() = 0;

public slots:
    void stateChangeSlot();

signals:
    void stateChangeSignal();

protected:
    QGridLayout *grid;
};

#endif //TMS_EXPRESS_CONTROLPANELVIEW_H
