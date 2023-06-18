///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: ControlPanelView
//
// Description: The ControlPanelView is an abstract interface for GUI controls. It consists of a grid containing at
//              least a title and a line, but it may be expanded to contain Qt widgets and items. The class implements
//              a catch-all signal which may be configured to trigger when a widget's state changes
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "User_Interfaces/Control_Panels/ControlPanelView.h"

/// Initialize a new Control Panel View
///
/// At minimum, the Control Panel View will have a title with a line below it
///
/// \param panelTitle Title to be displayed at the top of the panel
/// \param parent Parent QWidget
ControlPanelView::ControlPanelView(const std::string &panelTitle, QWidget *parent): QWidget(parent) {
    grid = new QGridLayout(this);
    title = new QLabel(panelTitle.c_str(), this);

    auto separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);

    grid->addWidget(title, 0, 0);
    grid->addWidget(separator, 1, 0, 1, 2);

    setLayout(grid);
}

ControlPanelView::~ControlPanelView() noexcept {
    delete grid;
    delete title;
}

/// Notify the application manger that a control has been modified
void ControlPanelView::stateChangeSlot() {
    emit stateChangeSignal();
}
