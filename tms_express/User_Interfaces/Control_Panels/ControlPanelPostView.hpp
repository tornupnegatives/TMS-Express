// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_USER_INTERFACES_CONTROL_PANELS_CONTROLPANELPOSTVIEW_HPP_
#define TMS_EXPRESS_USER_INTERFACES_CONTROL_PANELS_CONTROLPANELPOSTVIEW_HPP_

#include <QCheckBox>
#include <QSlider>
#include <QLineEdit>
#include <QWidget>

#include "User_Interfaces/Control_Panels/ControlPanelView.hpp"

namespace tms_express::ui {

class ControlPanelPostView: public ControlPanelView {
    Q_OBJECT

 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Creates a new Control Panel View for LPC post-processing
    ///         parameters
    /// @param parent Parent Qt widget
    explicit ControlPanelPostView(QWidget *parent = nullptr);

    ///////////////////////////////////////////////////////////////////////////
    // Overloaded Methods /////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @copydoc ControlPanelView::reset()
    void reset() override;

    /// @copydoc ControlPanelView::configureSlots()
    void configureSlots() override;

    ///////////////////////////////////////////////////////////////////////////
    // Accessors //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Checks if pitch shift should be applied to bitstream
    /// @return true if pitch shift should be applied, false otherwise
    bool getPitchShiftEnabled() const;

    /// @brief Accesses pitch shift (offset) slider
    /// @return Pitch offset, from pitch entry in coding table
    int getPitchShift() const;

    /// @brief Checks if pitch override (constant pitch) should be applied to
    ///         bitstream
    /// @return true if pitch override should be applied, false otherwise
    bool getPitchOverrideEnabled() const;

    /// @brief Accesses pitch override (constant pitch)
    /// @return Index of coding table pitch entry to be applied to entire
    ///         bitstream
    int getPitchOverride() const;

    /// @brief Checks if repeat frame detection should be applied to bitstream
    /// @return true if repeat frame detection should be applied, false
    ///         otherwise
    bool getRepeatFramesEnabled() const;

    /// @brief Checks if gain offset should be applied to bitstream
    /// @return true if gain shift should be applied, false otherwise
    bool getGainShiftEnabled() const;

    /// @brief Accesses gain shift (offset) slider
    /// @return Gain offset, from gain/energy entry in coding table
    int getGainShift() const;

    /// @brief Checks if gain normalization should be applied to bitstream
    /// @return true if gain normalization should be applied, false otherwise
    bool getGainNormalizationEnabled() const;

    /// @brief Accesses max unvoiced (consonant) gain line
    /// @return Max unvoiced gain, in decibels
    float getMaxUnvoicedGain() const;

    /// @brief Access max voiced (vowel) gain line
    /// @return Max voiced gain, in decibels
    float getMaxVoicedGain() const;

 private:
    ///////////////////////////////////////////////////////////////////////////
    // Members ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    QCheckBox *pitch_shift_checkbox_;
    QSlider *pitch_shift_slider_;
    QCheckBox *pitch_override_checkbox_;
    QSlider *pitch_override_slider_;

    QCheckBox *repeat_frames_checkbox_;

    QCheckBox *gain_shift_checkbox_;
    QSlider *gain_shift_slider_;

    QCheckBox *gain_normalization_checkbox_;

    QLineEdit *max_unvoiced_gain_line_;
    QLineEdit *max_voiced_gain_line_;
};

};  // namespace tms_express::ui

#endif  // TMS_EXPRESS_USER_INTERFACES_CONTROL_PANELS_CONTROLPANELPOSTVIEW_HPP_
