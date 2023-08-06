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
    hpf_checkbox_ = new QCheckBox("Highpass filter (Hz)", this);
    hpf_line_ = new QLineEdit("100", this);
    lpf_checkbox_ = new QCheckBox("Lowpass filter (Hz)", this);
    lpf_line_ = new QLineEdit("800", this);
    preemphasis_checkbox_ = new QCheckBox("Pre-emphasis filter (alpha)", this);
    preemphasis_line_ = new QLineEdit("0.9375", this);

    // Construct layout
    auto row = grid->rowCount();
    grid->addWidget(analysisWindowLabel, row, 0);
    grid->addWidget(analysisWindowLine, row++, 1);

    grid->addWidget(hpf_checkbox_, row, 0);
    grid->addWidget(hpf_line_, row++, 1);

    grid->addWidget(lpf_checkbox_, row, 0);
    grid->addWidget(lpf_line_, row++, 1);

    grid->addWidget(preemphasis_checkbox_, row, 0);
    grid->addWidget(preemphasis_line_, row, 1);
}

void ControlPanelLpcView::configureSlots() {
    connect(analysisWindowLine, &QLineEdit::editingFinished, this, &ControlPanelView::stateChanged);
    connect(hpf_checkbox_, &QCheckBox::released, this, &ControlPanelView::stateChanged);
    connect(hpf_line_, &QLineEdit::editingFinished, this, &ControlPanelView::stateChanged);
    connect(lpf_checkbox_, &QCheckBox::released, this, &ControlPanelView::stateChanged);
    connect(lpf_line_, &QLineEdit::editingFinished, this, &ControlPanelView::stateChanged);
    connect(preemphasis_checkbox_, &QCheckBox::released, this, &ControlPanelView::stateChanged);
    connect(preemphasis_line_, &QLineEdit::editingFinished, this, &ControlPanelView::stateChanged);
}

void ControlPanelLpcView::reset() {
    hpf_checkbox_->setChecked(false);
    hpf_line_->setText("100");

    lpf_checkbox_->setChecked(false);
    lpf_line_->setText("800");

    preemphasis_checkbox_->setChecked(true);
    preemphasis_line_->setText("0.9375");
}

float ControlPanelLpcView::analysisWindowWidth() {
    return analysisWindowLine->text().toFloat();
}
bool ControlPanelLpcView::getHpfEnabled() {
    return hpf_checkbox_->isChecked();
}

int ControlPanelLpcView::getHpfCutoff() {
    return hpf_line_->text().toInt();
}

bool ControlPanelLpcView::getLpfEnabled() {
    return lpf_checkbox_->isChecked();
}

int ControlPanelLpcView::getLpfCutoff() {
    return lpf_line_->text().toInt();
}

bool ControlPanelLpcView::getPreEmphasisEnabled() {
    return preemphasis_checkbox_->isChecked();
}

float ControlPanelLpcView::getPreEmphasisAlpha() {
    return preemphasis_line_->text().toFloat();
}

};  // namespace tms_express
