//
// Created by Joseph Bellahcen on 6/3/23.
//

#include "Frame_Encoding/Tms5220CodingTable.h"
#include "User_Interfaces/Control_Panels/ControlPanelPostView.h"

#include <QWidget>
#include <QtWidgets>

using namespace Tms5220CodingTable;

ControlPanelPostView::ControlPanelPostView(QWidget *parent) {
    // Initialize UI elements
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    grid = new QGridLayout(parent);
    grid->setSizeConstraint(QLayout::SetMinimumSize);

    title = new QLabel("Post-Processing");
    line1 = new QFrame();
    line1->setFrameShape(QFrame::HLine);
    line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);
    line3 = new QFrame();
    line3->setFrameShape(QFrame::HLine);

    pitchShiftCheckbox = new QCheckBox("Pitch shift", this);
    pitchShiftSlider = new QSlider(Qt::Horizontal, this);
    pitchOverrideCheckbox = new QCheckBox("Pitch override", this);
    pitchOverrideSlider = new QSlider(Qt::Horizontal, this);
    repeatFramesCheckbox = new QCheckBox("Repeat frames", this);
    gainShiftCheckbox = new QCheckBox("Gain shift", this);
    gainShiftSlider = new QSlider(Qt::Horizontal, this);
    gainNormalizationCheckbox = new QCheckBox("Gain normalization", this);
    maxUnvoicedGainLabel = new QLabel("Max unvoiced gain (dB)", this);
    maxUnvoicedGainLine = new QLineEdit("37.5", this);
    maxVoicedGainLabel = new QLabel("Max voiced gain (dB)", this);
    maxVoicedGainLine = new QLineEdit("37.5", this);

    // Setup sliders based on TMS5220 coding table
    pitchShiftSlider->setRange(-pitch.size(), pitch.size());
    pitchShiftSlider->setTickInterval(pitch.size() / 8);

    pitchOverrideSlider->setRange(0, pitch.size());
    pitchOverrideSlider->setTickInterval(pitch.size() / 16);

    gainShiftSlider->setRange(-rms.size(), rms.size());
    gainShiftSlider->setTickInterval(rms.size() / 8);

    // Configure slots
    connect(pitchShiftCheckbox, &QCheckBox::released, this, &ControlPanelPostView::slotStateChanged);
    connect(pitchShiftSlider, &QSlider::sliderReleased, this, &ControlPanelPostView::slotStateChanged);
    connect(pitchOverrideCheckbox, &QCheckBox::released, this, &ControlPanelPostView::slotStateChanged);
    connect(pitchOverrideSlider, &QSlider::sliderReleased, this, &ControlPanelPostView::slotStateChanged);
    connect(repeatFramesCheckbox, &QCheckBox::released, this, &ControlPanelPostView::slotStateChanged);
    connect(gainShiftCheckbox, &QCheckBox::released, this, &ControlPanelPostView::slotStateChanged);
    connect(gainShiftSlider, &QSlider::sliderReleased, this, &ControlPanelPostView::slotStateChanged);
    connect(gainNormalizationCheckbox, &QCheckBox::released, this, &ControlPanelPostView::slotStateChanged);
    connect(maxUnvoicedGainLine, &QLineEdit::editingFinished, this, &ControlPanelPostView::slotStateChanged);
    connect(maxVoicedGainLine, &QLineEdit::editingFinished, this, &ControlPanelPostView::slotStateChanged);


    // Construct layout
    grid->addWidget(title, 0, 0);
    grid->addWidget(line1, 1, 0, 1, 2);

    grid->addWidget(pitchShiftCheckbox, 2, 0);
    grid->addWidget(pitchShiftSlider, 2, 1);
    grid->addWidget(pitchOverrideCheckbox, 3, 0);
    grid->addWidget(pitchOverrideSlider, 3, 1);
    grid->addWidget(line2, 4, 0, 1, 2);

    grid->addWidget(repeatFramesCheckbox, 5, 0);
    grid->addWidget(line3, 6, 0, 1, 2);

    grid->addWidget(gainShiftCheckbox, 7, 0);
    grid->addWidget(gainShiftSlider, 7, 1);
    grid->addWidget(gainNormalizationCheckbox, 8, 0);
    grid->addWidget(maxUnvoicedGainLabel, 9, 0);
    grid->addWidget(maxUnvoicedGainLine, 9, 1);
    grid->addWidget(maxVoicedGainLabel, 10, 0);
    grid->addWidget(maxVoicedGainLine, 10, 1);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setLayout(grid);
    reset();
}

void ControlPanelPostView::reset(bool enableGainNormalization) {
    pitchShiftCheckbox->setChecked(false);
    pitchShiftSlider->setSliderPosition(0);
    pitchOverrideCheckbox->setChecked(false);
    pitchOverrideSlider->setSliderPosition(pitch.size() / 2);

    repeatFramesCheckbox->setChecked(false);

    gainShiftCheckbox->setChecked(false);
    gainShiftSlider->setSliderPosition(0);
    gainNormalizationCheckbox->setChecked(enableGainNormalization);
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

void ControlPanelPostView::slotStateChanged() {
    emit signalStateChanged();
}
