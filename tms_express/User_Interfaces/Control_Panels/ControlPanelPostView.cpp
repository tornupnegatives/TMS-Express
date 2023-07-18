///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: ControlPanelPostView
//
// Description: The ControlPanelPostView contains parameters which guide bitstream post-processing
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Frame_Encoding/Tms5220CodingTable.h"
#include "User_Interfaces/Control_Panels/ControlPanelPostView.h"

#include <QWidget>
#include <QtWidgets>

namespace tms_express {

using namespace Tms5220CodingTable;

ControlPanelPostView::ControlPanelPostView(QWidget *parent): ControlPanelView("Post-Processing", parent) {
    auto line2 = new QFrame(this);
    line2->setFrameShape(QFrame::HLine);
    auto line3 = new QFrame(this);
    line3->setFrameShape(QFrame::HLine);

    pitchShiftCheckbox = new QCheckBox("Pitch shift", this);
    pitchShiftSlider = new QSlider(Qt::Horizontal, this);
    pitchOverrideCheckbox = new QCheckBox("Pitch override", this);
    pitchOverrideSlider = new QSlider(Qt::Horizontal, this);
    repeatFramesCheckbox = new QCheckBox("Repeat frames", this);
    gainShiftCheckbox = new QCheckBox("Gain shift", this);
    gainShiftSlider = new QSlider(Qt::Horizontal, this);
    gainNormalizationCheckbox = new QCheckBox("Gain normalization", this);
    auto maxUnvoicedGainLabel = new QLabel("Max unvoiced gain (dB)", this);
    maxUnvoicedGainLine = new QLineEdit("37.5", this);
    auto maxVoicedGainLabel = new QLabel("Max voiced gain (dB)", this);
    maxVoicedGainLine = new QLineEdit("37.5", this);

    // Setup sliders based on TMS5220 coding table
    pitchShiftSlider->setRange(-pitch.size(), pitch.size());
    pitchShiftSlider->setTickInterval(pitch.size() / 8);

    pitchOverrideSlider->setRange(0, pitch.size());
    pitchOverrideSlider->setTickInterval(pitch.size() / 16);

    gainShiftSlider->setRange(-rms.size(), rms.size());
    gainShiftSlider->setTickInterval(rms.size() / 8);

    // Construct layout
    auto row = grid->rowCount();

    grid->addWidget(pitchShiftCheckbox, row, 0);
    grid->addWidget(pitchShiftSlider, row++, 1);

    grid->addWidget(pitchOverrideCheckbox, row, 0);
    grid->addWidget(pitchOverrideSlider, row++, 1);

    grid->addWidget(line2, row++, 0, 1, 2);

    grid->addWidget(repeatFramesCheckbox, row++, 0);

    grid->addWidget(line3, row++, 0, 1, 2);

    grid->addWidget(gainShiftCheckbox, row, 0);
    grid->addWidget(gainShiftSlider, row++, 1);

    grid->addWidget(gainNormalizationCheckbox, row++, 0);

    grid->addWidget(maxUnvoicedGainLabel, row, 0);
    grid->addWidget(maxUnvoicedGainLine, row++, 1);

    grid->addWidget(maxVoicedGainLabel, row, 0);
    grid->addWidget(maxVoicedGainLine, row, 1);
}

void ControlPanelPostView::configureSlots() {
    connect(pitchShiftCheckbox, &QCheckBox::released, this, &ControlPanelView::stateChangeSlot);
    connect(pitchShiftSlider, &QSlider::sliderReleased, this, &ControlPanelView::stateChangeSlot);
    connect(pitchOverrideCheckbox, &QCheckBox::released, this, &ControlPanelView::stateChangeSlot);
    connect(pitchOverrideSlider, &QSlider::sliderReleased, this, &ControlPanelView::stateChangeSlot);
    connect(repeatFramesCheckbox, &QCheckBox::released, this, &ControlPanelView::stateChangeSlot);
    connect(gainShiftCheckbox, &QCheckBox::released, this, &ControlPanelView::stateChangeSlot);
    connect(gainShiftSlider, &QSlider::sliderReleased, this, &ControlPanelView::stateChangeSlot);
    connect(gainNormalizationCheckbox, &QCheckBox::released, this, &ControlPanelView::stateChangeSlot);
    connect(maxUnvoicedGainLine, &QLineEdit::editingFinished, this, &ControlPanelView::stateChangeSlot);
    connect(maxVoicedGainLine, &QLineEdit::editingFinished, this, &ControlPanelView::stateChangeSlot);
}

void ControlPanelPostView::reset() {
    pitchShiftCheckbox->setChecked(false);
    pitchShiftSlider->setSliderPosition(0);
    pitchOverrideCheckbox->setChecked(false);
    pitchOverrideSlider->setSliderPosition(pitch.size() / 2);

    repeatFramesCheckbox->setChecked(false);

    gainShiftCheckbox->setChecked(false);
    gainShiftSlider->setSliderPosition(0);
    //gainNormalizationCheckbox->setChecked(enableGainNormalization);
    maxUnvoicedGainLine->setText("37.5");
    maxVoicedGainLine->setText("37.5");
}

bool ControlPanelPostView::pitchShiftEnabled() {
    return pitchShiftCheckbox->isChecked();
}

int ControlPanelPostView::pitchShift() {
    return pitchShiftSlider->value();
}

bool ControlPanelPostView::pitchOverrideEnabled() {
    return pitchOverrideCheckbox->isChecked();
}

int ControlPanelPostView::pitchOverride() {
    return pitchOverrideSlider->value();
}

bool ControlPanelPostView::repeatFramesEnabled() {
    return repeatFramesCheckbox->isChecked();
}

bool ControlPanelPostView::gainShiftEnabled() {
    return gainShiftCheckbox->isChecked();
}

int ControlPanelPostView::gainShift() {
    return gainShiftSlider->value();
}

bool ControlPanelPostView::gainNormalizationEnabled() {
    return gainNormalizationCheckbox->isChecked();
}

float ControlPanelPostView::maxUnvoicedGain() {
    return maxUnvoicedGainLine->text().toFloat();
}

float ControlPanelPostView::maxVoicedGain() {
    return maxVoicedGainLine->text().toFloat();
}

};  // namespace tms_express
