// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#include "User_Interfaces/Control_Panels/ControlPanelPostView.hpp"

#include <QCheckBox>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>

#include "Frame_Encoding/CodingTable.hpp"

namespace tms_express::ui {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ControlPanelPostView::ControlPanelPostView(QWidget *parent):
    ControlPanelView("Post-Processing", parent) {
    // Initialize parameters
    pitch_shift_checkbox_ = new QCheckBox("Pitch shift", this);
    pitch_shift_slider_ = new QSlider(Qt::Horizontal, this);

    pitch_override_checkbox_ = new QCheckBox("Pitch override", this);
    pitch_override_slider_ = new QSlider(Qt::Horizontal, this);

    repeat_frames_checkbox_ = new QCheckBox("Repeat frames", this);

    gain_shift_checkbox_ = new QCheckBox("Gain shift", this);
    gain_shift_slider_ = new QSlider(Qt::Horizontal, this);

    gain_normalization_checkbox_ = new QCheckBox("Gain normalization", this);

    auto maxUnvoicedGainLabel = new QLabel("Max unvoiced gain (dB)", this);
    max_unvoiced_gain_line_ = new QLineEdit("37.5", this);

    auto maxVoicedGainLabel = new QLabel("Max voiced gain (dB)", this);
    max_voiced_gain_line_ = new QLineEdit("37.5", this);

    // Configure sliders based on TMS5220 coding table
    pitch_shift_slider_->setRange(
        -1 * static_cast<int>(coding_table::tms5220::pitch.size()),
        coding_table::tms5220::pitch.size());

    pitch_shift_slider_->setTickInterval(
        coding_table::tms5220::pitch.size() / 8);

    pitch_override_slider_->setRange(
        0,
        coding_table::tms5220::pitch.size());

    pitch_override_slider_->setTickInterval(
        coding_table::tms5220::pitch.size() / 16);

    gain_shift_slider_->setRange(
        -1 * static_cast<int>(coding_table::tms5220::rms.size()),
        coding_table::tms5220::rms.size());

    gain_shift_slider_->setTickInterval(
        coding_table::tms5220::rms.size() / 8);

    // Construct layout
    auto row = grid->rowCount();

    grid->addWidget(pitch_shift_checkbox_, row, 0);
    grid->addWidget(pitch_shift_slider_, row++, 1);

    grid->addWidget(pitch_override_checkbox_, row, 0);
    grid->addWidget(pitch_override_slider_, row++, 1);

    auto line2 = new QFrame(this);
    line2->setFrameShape(QFrame::HLine);
    grid->addWidget(line2, row++, 0, 1, 2);

    grid->addWidget(repeat_frames_checkbox_, row++, 0);

    auto line3 = new QFrame(this);
    line3->setFrameShape(QFrame::HLine);
    grid->addWidget(line3, row++, 0, 1, 2);

    grid->addWidget(gain_shift_checkbox_, row, 0);
    grid->addWidget(gain_shift_slider_, row++, 1);

    grid->addWidget(gain_normalization_checkbox_, row++, 0);

    grid->addWidget(maxUnvoicedGainLabel, row, 0);
    grid->addWidget(max_unvoiced_gain_line_, row++, 1);

    grid->addWidget(maxVoicedGainLabel, row, 0);
    grid->addWidget(max_voiced_gain_line_, row, 1);
}

///////////////////////////////////////////////////////////////////////////////
// Overloaded Methods /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ControlPanelPostView::reset() {
    pitch_shift_checkbox_->setChecked(false);
    pitch_shift_slider_->setSliderPosition(0);
    pitch_override_checkbox_->setChecked(false);
    pitch_override_slider_->setSliderPosition(
        coding_table::tms5220::pitch.size() / 2);

    repeat_frames_checkbox_->setChecked(false);

    gain_shift_checkbox_->setChecked(false);
    gain_shift_slider_->setSliderPosition(0);
    // gain_normalization_checkbox_->setChecked(enableGainNormalization);
    max_unvoiced_gain_line_->setText("37.5");
    max_voiced_gain_line_->setText("37.5");
}

void ControlPanelPostView::configureSlots() {
    connect(pitch_shift_checkbox_, &QCheckBox::released, this,
        &ControlPanelView::stateChanged);

    connect(pitch_shift_slider_, &QSlider::sliderReleased, this,
        &ControlPanelView::stateChanged);

    connect(pitch_override_checkbox_, &QCheckBox::released, this,
        &ControlPanelView::stateChanged);

    connect(pitch_override_slider_, &QSlider::sliderReleased, this,
        &ControlPanelView::stateChanged);

    connect(repeat_frames_checkbox_, &QCheckBox::released, this,
        &ControlPanelView::stateChanged);

    connect(gain_shift_checkbox_, &QCheckBox::released, this,
        &ControlPanelView::stateChanged);

    connect(gain_shift_slider_, &QSlider::sliderReleased, this,
        &ControlPanelView::stateChanged);

    connect(gain_normalization_checkbox_, &QCheckBox::released, this,
        &ControlPanelView::stateChanged);

    connect(max_unvoiced_gain_line_, &QLineEdit::editingFinished, this,
        &ControlPanelView::stateChanged);

    connect(max_voiced_gain_line_, &QLineEdit::editingFinished, this,
        &ControlPanelView::stateChanged);
}

///////////////////////////////////////////////////////////////////////////////
// Accessors //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool ControlPanelPostView::getPitchShiftEnabled() const {
    return pitch_shift_checkbox_->isChecked();
}

int ControlPanelPostView::getPitchShift() const {
    return pitch_shift_slider_->value();
}

bool ControlPanelPostView::getPitchOverrideEnabled() const {
    return pitch_override_checkbox_->isChecked();
}

int ControlPanelPostView::getPitchOverride() const {
    return pitch_override_slider_->value();
}

bool ControlPanelPostView::getRepeatFramesEnabled() const {
    return repeat_frames_checkbox_->isChecked();
}

bool ControlPanelPostView::getGainShiftEnabled() const {
    return gain_shift_checkbox_->isChecked();
}

int ControlPanelPostView::getGainShift() const {
    return gain_shift_slider_->value();
}

bool ControlPanelPostView::getGainNormalizationEnabled() const {
    return gain_normalization_checkbox_->isChecked();
}

float ControlPanelPostView::getMaxUnvoicedGain() const {
    return max_unvoiced_gain_line_->text().toFloat();
}

float ControlPanelPostView::getMaxVoicedGain() const {
    return max_voiced_gain_line_->text().toFloat();
}

};  // namespace tms_express::ui
