///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: ControlPanelPitchView
//
// Description: The ControlPanelPitchView contains parameters which guide pitch analysis
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "User_Interfaces/Control_Panels/ControlPanelPitchView.h"
#include "User_Interfaces/Control_Panels/ControlPanelView.h"
#include <QWidget>
#include <QtWidgets>

ControlPanelPitchView::ControlPanelPitchView(QWidget *parent): ControlPanelView("Pitch Analysis", parent) {
    auto line2 = new QFrame(this);
    line2->setFrameShape(QFrame::HLine);

    hpfCheckbox = new QCheckBox("Highpass filter (Hz)", this);
    hpfLine = new QLineEdit("100", this);
    lpfCheckbox = new QCheckBox("Lowpass filter (Hz)", this);
    lpfLine = new QLineEdit("800", this);
    preemphCheckbox = new QCheckBox("Pre-emphasis filter (alpha)", this);
    preemphLine = new QLineEdit("0.9375", this);
    maxPitchLabel = new QLabel("Max pitch (Hz)", this);
    maxPitchLine = new QLineEdit("500", this);
    minPitchLabel = new QLabel("Min pitch (Hz)", this);
    minPitchLine = new QLineEdit("50", this);

    auto row = grid->rowCount();

    grid->addWidget(hpfCheckbox, row, 0);
    grid->addWidget(hpfLine, row++, 1);

    grid->addWidget(lpfCheckbox, row, 0);
    grid->addWidget(lpfLine, row++, 1);

    grid->addWidget(preemphCheckbox, row, 0);
    grid->addWidget(preemphLine, row++, 1);

    grid->addWidget(line2, row++, 0, 1, 2);
    
    grid->addWidget(maxPitchLabel, row, 0);
    grid->addWidget(maxPitchLine, row++, 1);

    grid->addWidget(minPitchLabel, row, 0);
    grid->addWidget(minPitchLine, row, 1);
}

void ControlPanelPitchView::configureSlots() {
    connect(hpfCheckbox, &QCheckBox::released, this, &ControlPanelView::stateChangeSlot);
    connect(hpfLine, &QLineEdit::editingFinished, this, &ControlPanelView::stateChangeSlot);
    connect(lpfCheckbox, &QCheckBox::released, this, &ControlPanelView::stateChangeSlot);
    connect(lpfLine, &QLineEdit::editingFinished, this, &ControlPanelView::stateChangeSlot);
    connect(preemphCheckbox, &QCheckBox::released, this, &ControlPanelView::stateChangeSlot);
    connect(preemphLine, &QLineEdit::editingFinished, this, &ControlPanelView::stateChangeSlot);
    connect(maxPitchLine, &QLineEdit::editingFinished, this, &ControlPanelView::stateChangeSlot);
    connect(minPitchLine, &QLineEdit::editingFinished, this, &ControlPanelView::stateChangeSlot);
}

void ControlPanelPitchView::reset() {
    hpfCheckbox->setChecked(false);
    hpfLine->setText("100");

    lpfCheckbox->setChecked(true);
    lpfLine->setText("800");

    preemphCheckbox->setChecked(false);
    preemphLine->setText("0.9375");

    maxPitchLine->setText("500");
    minPitchLine->setText("50");
}

bool ControlPanelPitchView::hpfEnabled() {
    return hpfCheckbox->isChecked();
}

int ControlPanelPitchView::hpfCutoff() {
    return hpfLine->text().toInt();
}

bool ControlPanelPitchView::lpfEnabled() {
    return lpfCheckbox->isChecked();
}

int ControlPanelPitchView::lpfCutoff() {
    return lpfLine->text().toInt();
}

bool ControlPanelPitchView::preemphEnabled() {
    return preemphCheckbox->isChecked();
}

float ControlPanelPitchView::preemphAlpha() {
    return preemphLine->text().toFloat();
}

int ControlPanelPitchView::maxPitchFrq() {
    return maxPitchLine->text().toInt();
}

int ControlPanelPitchView::minPitchFrq() {
    return minPitchLine->text().toInt();
}
