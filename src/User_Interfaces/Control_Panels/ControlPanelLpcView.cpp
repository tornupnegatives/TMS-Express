//
// Created by Joseph Bellahcen on 6/1/23.
///

#include "User_Interfaces/Control_Panels/ControlPanelLpcView.h"
#include <QWidget>
#include <QtWidgets>

ControlPanelLpcView::ControlPanelLpcView(QWidget *parent) {
    // Initialize UI elements
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    grid = new QGridLayout(parent);
    grid->setSizeConstraint(QLayout::SetMinimumSize);

    title = new QLabel("LPC Analysis");
    title->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    line1 = new QFrame();
    line1->setFrameShape(QFrame::HLine);

    analysisWindowLabel = new QLabel("Analysis window (ms)", this);
    analysisWindowLine = new QLineEdit("25.0", this);
    hpfCheckbox = new QCheckBox("Highpass filter (Hz)", this);
    hpfLine = new QLineEdit("100", this);
    lpfCheckbox = new QCheckBox("Lowpass filter (Hz)", this);
    lpfLine = new QLineEdit("800", this);
    preemphCheckbox = new QCheckBox("Pre-emphasis filter (alpha)", this);
    preemphLine = new QLineEdit("0.9375", this);

    // Configure slots
    connect(analysisWindowLine, &QLineEdit::editingFinished, this, &ControlPanelLpcView::slotStateChanged);
    connect(hpfCheckbox, &QCheckBox::released, this, &ControlPanelLpcView::slotStateChanged);
    connect(hpfLine, &QLineEdit::editingFinished, this, &ControlPanelLpcView::slotStateChanged);
    connect(lpfCheckbox, &QCheckBox::released, this, &ControlPanelLpcView::slotStateChanged);
    connect(lpfLine, &QLineEdit::editingFinished, this, &ControlPanelLpcView::slotStateChanged);
    connect(preemphCheckbox, &QCheckBox::released, this, &ControlPanelLpcView::slotStateChanged);
    connect(preemphLine, &QLineEdit::editingFinished, this, &ControlPanelLpcView::slotStateChanged);

    // Construct layout
    grid->addWidget(title, 0, 0);
    grid->addWidget(line1, 1, 0, 1, 2);

    grid->addWidget(analysisWindowLabel, 2, 0);
    grid->addWidget(analysisWindowLine, 2, 1);
    grid->addWidget(hpfCheckbox, 3, 0);
    grid->addWidget(hpfLine, 3, 1);
    grid->addWidget(lpfCheckbox, 4, 0);
    grid->addWidget(lpfLine, 4, 1);
    grid->addWidget(preemphCheckbox, 5, 0);
    grid->addWidget(preemphLine, 5, 1);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setLayout(grid);
    reset();
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

void ControlPanelLpcView::slotStateChanged() {
    emit signalStateChanged();
}
