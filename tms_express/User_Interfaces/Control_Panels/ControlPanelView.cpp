// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#include "User_Interfaces/Control_Panels/ControlPanelView.hpp"

#include <QFrame>
#include <QLabel>
#include <QGridLayout>

namespace tms_express::ui {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ControlPanelView::ControlPanelView(const std::string &title, QWidget *parent):
    QWidget(parent) {
    grid = new QGridLayout(this);

    auto panel_title = new QLabel(title.c_str(), this);
    auto separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);

    grid->addWidget(panel_title, 0, 0);
    grid->addWidget(separator, 1, 0, 1, 2);

    setLayout(grid);
}

///////////////////////////////////////////////////////////////////////////////
// Qt Slots ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ControlPanelView::stateChanged() {
    emit stateChangeSignal();
}

};  // namespace tms_express::ui
