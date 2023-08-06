///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: ControlPanelPitchView
//
// Description: The ControlPanelPitchView contains parameters which guide pitch analysis
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "User_Interfaces/Control_Panels/ControlPanelPitchView.hpp"

#include <QCheckBox>
#include <QFrame>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>

#include <string>

#include "User_Interfaces/Control_Panels/ControlPanelView.hpp"

namespace tms_express::ui {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

ControlPanelPitchView::ControlPanelPitchView(QWidget *parent):
    ControlPanelView("Pitch Analysis", parent) {
    // Initialize parameters
    hpf_checkbox_ = new QCheckBox("Highpass filter (Hz)", this);
    hpf_line_ = new QLineEdit("100", this);

    lpf_checkbox_ = new QCheckBox("Lowpass filter (Hz)", this);
    lpf_line_ = new QLineEdit("800", this);

    preemphasis_checkbox_ = new QCheckBox("Pre-emphasis filter (alpha)", this);
    preemphasis_line_ = new QLineEdit("0.9375", this);

    auto maxPitchLabel = new QLabel("Max pitch (Hz)", this);
    max_pitch_frq_line_ = new QLineEdit("500", this);

    auto minPitchLabel = new QLabel("Min pitch (Hz)", this);
    min_pitch_frq_line_ = new QLineEdit("50", this);

    // Construct layout
    auto row = grid->rowCount();

    grid->addWidget(hpf_checkbox_, row, 0);
    grid->addWidget(hpf_line_, row++, 1);

    grid->addWidget(lpf_checkbox_, row, 0);
    grid->addWidget(lpf_line_, row++, 1);

    grid->addWidget(preemphasis_checkbox_, row, 0);
    grid->addWidget(preemphasis_line_, row++, 1);

    auto line2 = new QFrame(this);
    line2->setFrameShape(QFrame::HLine);
    grid->addWidget(line2, row++, 0, 1, 2);

    grid->addWidget(maxPitchLabel, row, 0);
    grid->addWidget(max_pitch_frq_line_, row++, 1);

    grid->addWidget(minPitchLabel, row, 0);
    grid->addWidget(min_pitch_frq_line_, row, 1);
}

///////////////////////////////////////////////////////////////////////////////
// Overloaded Methods /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void ControlPanelPitchView::configureSlots() {
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

    connect(max_pitch_frq_line_, &QLineEdit::editingFinished, this,
        &ControlPanelView::stateChanged);

    connect(min_pitch_frq_line_, &QLineEdit::editingFinished, this,
        &ControlPanelView::stateChanged);
}

void ControlPanelPitchView::reset() {
    hpf_checkbox_->setChecked(false);
    hpf_line_->setText("100");

    lpf_checkbox_->setChecked(true);
    lpf_line_->setText("800");

    preemphasis_checkbox_->setChecked(false);
    preemphasis_line_->setText("0.9375");

    max_pitch_frq_line_->setText("500");
    min_pitch_frq_line_->setText("50");
}

///////////////////////////////////////////////////////////////////////////////
// Accessors //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


bool ControlPanelPitchView::getHpfEnabled() {
    return hpf_checkbox_->isChecked();
}

int ControlPanelPitchView::getHpfCutoff() {
    return hpf_line_->text().toInt();
}

bool ControlPanelPitchView::getLpfEnabled() {
    return lpf_checkbox_->isChecked();
}

int ControlPanelPitchView::getLpfCutoff() {
    return lpf_line_->text().toInt();
}

bool ControlPanelPitchView::getPreEmphasisEnabled() {
    return preemphasis_checkbox_->isChecked();
}

float ControlPanelPitchView::getPreEmphasisAlpha() {
    return preemphasis_line_->text().toFloat();
}

int ControlPanelPitchView::getMaxPitchFrq() {
    return max_pitch_frq_line_->text().toInt();
}

int ControlPanelPitchView::getMinPitchFrq() {
    return min_pitch_frq_line_->text().toInt();
}

};  // namespace tms_express::ui
