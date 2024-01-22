// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#include "ui/Control_Panels/ControlPanelLpcView.hpp"

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>

#include "ui/Control_Panels/ControlPanelView.hpp"

namespace tms_express::ui {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ControlPanelLpcView::ControlPanelLpcView(QWidget *parent):
    ControlPanelView("LPC Analysis", parent)  {
    //
    // Initialize parameters
    auto analysis_window_label = new QLabel("Analysis window (ms)", this);
    analysis_window_line_ = new QLineEdit("25.0", this);

    hpf_checkbox_ = new QCheckBox("Highpass filter (Hz)", this);
    hpf_line_ = new QLineEdit("100", this);

    lpf_checkbox_ = new QCheckBox("Lowpass filter (Hz)", this);
    lpf_line_ = new QLineEdit("800", this);

    preemphasis_checkbox_ = new QCheckBox("Pre-emphasis filter (alpha)", this);
    preemphasis_line_ = new QLineEdit("0.9375", this);

    // Construct layout
    auto row = grid->rowCount();

    grid->addWidget(analysis_window_label, row, 0);
    grid->addWidget(analysis_window_line_, row++, 1);

    grid->addWidget(hpf_checkbox_, row, 0);
    grid->addWidget(hpf_line_, row++, 1);

    grid->addWidget(lpf_checkbox_, row, 0);
    grid->addWidget(lpf_line_, row++, 1);

    grid->addWidget(preemphasis_checkbox_, row, 0);
    grid->addWidget(preemphasis_line_, row, 1);
}

///////////////////////////////////////////////////////////////////////////////
// Overloaded Methods /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ControlPanelLpcView::reset() {
    hpf_checkbox_->setChecked(false);
    hpf_line_->setText("100");

    lpf_checkbox_->setChecked(false);
    lpf_line_->setText("800");

    preemphasis_checkbox_->setChecked(true);
    preemphasis_line_->setText("0.9375");
}

void ControlPanelLpcView::configureSlots() {
    connect(analysis_window_line_, &QLineEdit::editingFinished, this,
        &ControlPanelView::stateChanged);

    connect(hpf_checkbox_, &QCheckBox::released, this,
        &ControlPanelView::stateChanged);

    connect(hpf_line_, &QLineEdit::editingFinished, this,
        &ControlPanelView::stateChanged);

    connect(lpf_checkbox_, &QCheckBox::released, this,
        &ControlPanelView::stateChanged);

    connect(lpf_line_, &QLineEdit::editingFinished, this,
        &ControlPanelView::stateChanged);

    connect(preemphasis_checkbox_, &QCheckBox::released, this,
        &ControlPanelView::stateChanged);

    connect(preemphasis_line_, &QLineEdit::editingFinished, this,
        &ControlPanelView::stateChanged);
}

///////////////////////////////////////////////////////////////////////////////
// Accessors //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

float ControlPanelLpcView::getAnalysisWindowWidth() {
    return analysis_window_line_->text().toFloat();
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

};  // namespace tms_express::ui
