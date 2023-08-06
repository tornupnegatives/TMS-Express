///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: ControlPanelLpcView
//
// Description: The ControlPanelLpcView contains parameters which guide LPC analysis
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "User_Interfaces/Control_Panels/ControlPanelLpcView.h"
#include "User_Interfaces/Control_Panels/ControlPanelView.hpp"

#include <QWidget>
#include <QtWidgets>

namespace tms_express::ui {

ControlPanelLpcView::ControlPanelLpcView(QWidget *parent): ControlPanelView("LPC Analysis", parent)  {
    auto analysisWindowLabel = new QLabel("Analysis window (ms)", this);
    analysisWindowLine = new QLineEdit("25.0", this);
    hpfCheckbox = new QCheckBox("Highpass filter (Hz)", this);
    hpfLine = new QLineEdit("100", this);
    lpfCheckbox = new QCheckBox("Lowpass filter (Hz)", this);
    lpfLine = new QLineEdit("800", this);
    preemphCheckbox = new QCheckBox("Pre-emphasis filter (alpha)", this);
    preemphLine = new QLineEdit("0.9375", this);

    // Construct layout
    auto row = grid->rowCount();
    grid->addWidget(analysisWindowLabel, row, 0);
    grid->addWidget(analysisWindowLine, row++, 1);

    grid->addWidget(hpfCheckbox, row, 0);
    grid->addWidget(hpfLine, row++, 1);

    grid->addWidget(lpfCheckbox, row, 0);
    grid->addWidget(lpfLine, row++, 1);

    grid->addWidget(preemphCheckbox, row, 0);
    grid->addWidget(preemphLine, row, 1);
}

void ControlPanelLpcView::configureSlots() {
    connect(analysisWindowLine, &QLineEdit::editingFinished, this, &ControlPanelView::stateChanged);
    connect(hpfCheckbox, &QCheckBox::released, this, &ControlPanelView::stateChanged);
    connect(hpfLine, &QLineEdit::editingFinished, this, &ControlPanelView::stateChanged);
    connect(lpfCheckbox, &QCheckBox::released, this, &ControlPanelView::stateChanged);
    connect(lpfLine, &QLineEdit::editingFinished, this, &ControlPanelView::stateChanged);
    connect(preemphCheckbox, &QCheckBox::released, this, &ControlPanelView::stateChanged);
    connect(preemphLine, &QLineEdit::editingFinished, this, &ControlPanelView::stateChanged);
}

void ControlPanelLpcView::reset() {
    hpfCheckbox->setChecked(false);
    hpfLine->setText("100");

    lpfCheckbox->setChecked(false);
    lpfLine->setText("800");

    preemphCheckbox->setChecked(true);
    preemphLine->setText("0.9375");
}

float ControlPanelLpcView::analysisWindowWidth() {
    return analysisWindowLine->text().toFloat();
}
bool ControlPanelLpcView::hpfEnabled() {
    return hpfCheckbox->isChecked();
}

int ControlPanelLpcView::hpfCutoff() {
    return hpfLine->text().toInt();
}

bool ControlPanelLpcView::lpfEnabled() {
    return lpfCheckbox->isChecked();
}

int ControlPanelLpcView::lpfCutoff() {
    return lpfLine->text().toInt();
}

bool ControlPanelLpcView::preemphEnabled() {
    return preemphCheckbox->isChecked();
}

float ControlPanelLpcView::preemphAlpha() {
    return preemphLine->text().toFloat();
}

};  // namespace tms_express
