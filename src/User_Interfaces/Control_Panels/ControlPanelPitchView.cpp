//
// Created by Joseph Bellahcen on 6/1/23.
//

#include "User_Interfaces/Control_Panels/ControlPanelPitchView.h"
#include <QWidget>
#include <QtWidgets>

ControlPanelPitchView::ControlPanelPitchView(QWidget *parent) {
    // Initialize UI elements
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    grid = new QGridLayout(parent);
    grid->setSizeConstraint(QLayout::SetMinimumSize);

    title = new QLabel("Pitch Analysis");
    line1 = new QFrame();
    line1->setFrameShape(QFrame::HLine);
    line2 = new QFrame();
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

    // Configure slots
    connect(hpfCheckbox, &QCheckBox::released, this, &ControlPanelPitchView::slotStateChanged);
    connect(hpfLine, &QLineEdit::editingFinished, this, &ControlPanelPitchView::slotStateChanged);
    connect(lpfCheckbox, &QCheckBox::released, this, &ControlPanelPitchView::slotStateChanged);
    connect(lpfLine, &QLineEdit::editingFinished, this, &ControlPanelPitchView::slotStateChanged);
    connect(preemphCheckbox, &QCheckBox::released, this, &ControlPanelPitchView::slotStateChanged);
    connect(preemphLine, &QLineEdit::editingFinished, this, &ControlPanelPitchView::slotStateChanged);
    connect(maxPitchLine, &QLineEdit::editingFinished, this, &ControlPanelPitchView::slotStateChanged);
    connect(minPitchLine, &QLineEdit::editingFinished, this, &ControlPanelPitchView::slotStateChanged);

    // Construct layout
    grid->addWidget(title, 0, 0);
    grid->addWidget(line1, 1, 0, 1, 2);

    grid->addWidget(hpfCheckbox, 2, 0);
    grid->addWidget(hpfLine, 2, 1);
    grid->addWidget(lpfCheckbox, 3, 0);
    grid->addWidget(lpfLine, 3, 1);
    grid->addWidget(preemphCheckbox, 4, 0);
    grid->addWidget(preemphLine, 4, 1);
    grid->addWidget(line2, 5, 0, 1, 2);
    
    grid->addWidget(maxPitchLabel, 6, 0);
    grid->addWidget(maxPitchLine, 6, 1);
    grid->addWidget(minPitchLabel, 7, 0);
    grid->addWidget(minPitchLine, 7, 1);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setLayout(grid);
    reset();
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

void ControlPanelPitchView::slotStateChanged() {
    emit signalStateChanged();
}
