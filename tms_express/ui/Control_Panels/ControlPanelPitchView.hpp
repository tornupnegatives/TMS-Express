// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_USER_INTERFACES_CONTROL_PANELS_CONTROLPANELPITCHVIEW_HPP_
#define TMS_EXPRESS_USER_INTERFACES_CONTROL_PANELS_CONTROLPANELPITCHVIEW_HPP_

#include <QCheckBox>
#include <QSlider>
#include <QLineEdit>
#include <QWidget>

#include <string>

#include "ui/Control_Panels/ControlPanelView.hpp"

namespace tms_express::ui {

/// @brief Control Panel View for LPC pitch analysis and post-processing
///         parameters
class ControlPanelPitchView: public ControlPanelView {
    Q_OBJECT

 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Creates a new Control Panel View for LPC pitch analysis and
    ////        post-processing parameters
    /// @param parent Parent Qt widget
    explicit ControlPanelPitchView(QWidget *parent = nullptr);

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

    /// @brief Checks if highpass filter should be applied to pitch buffer
    /// @return true if highpass filter should be applied, false otherwise
    bool getHpfEnabled();

    /// @brief Accesses highpass filter cutoff
    /// @return Highpass filter cutoff, in Hertz
    int getHpfCutoff();

    /// @brief Checks if lowpass filter should be applied to pitch buffer
    /// @return true if lowpass filter should be applied, false otherwise
    bool getLpfEnabled();

    /// @brief Accesses lowpass filter cutoff
    /// @return Lowpass filter cutoff, in Hertz
    int getLpfCutoff();

    /// @brief Checks if pre-emphasis filter should be applied to pitch buffer
    /// @return true if pre-emphasis filter should be applied, false otherwise
    bool getPreEmphasisEnabled();

    /// @brief Accesses pre-emphasis filter coefficient
    /// @return Pre-emphasis filter coefficient
    float getPreEmphasisAlpha();

    /// @brief Accesses pitch analysis ceiling
    /// @return Max pitch frequency, in Hertz
    int getMaxPitchFrq();

    /// @brief Accesses pitch analysis floor
    /// @return Min pitch frequency, in Hertz
    int getMinPitchFrq();

 private:
    ///////////////////////////////////////////////////////////////////////////
    // Members ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    QCheckBox *hpf_checkbox_;
    QLineEdit *hpf_line_;

    QCheckBox *lpf_checkbox_;
    QLineEdit *lpf_line_;

    QCheckBox *preemphasis_checkbox_;
    QLineEdit *preemphasis_line_;

    QLineEdit *max_pitch_frq_line_;
    QLineEdit *min_pitch_frq_line_;
};

};  // namespace tms_express::ui

#endif  // TMS_EXPRESS_USER_INTERFACES_CONTROL_PANELS_CONTROLPANELPITCHVIEW_HPP_
