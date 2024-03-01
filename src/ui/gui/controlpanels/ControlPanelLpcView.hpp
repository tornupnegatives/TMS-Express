// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_USER_INTERFACES_CONTROL_PANELS_CONTROLPANELLPCVIEW_HPP_
#define TMS_EXPRESS_USER_INTERFACES_CONTROL_PANELS_CONTROLPANELLPCVIEW_HPP_

#include <QCheckBox>
#include <QLineEdit>
#include <QWidget>

#include "ui/gui/controlpanels/ControlPanelView.hpp"

namespace tms_express::ui {

/// @brief Control Panel View for LPC analysis parameters
class ControlPanelLpcView: public ControlPanelView {
    Q_OBJECT

 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Creates a new Control Panel View for LPC analysis parameters
    /// @param parent Parent Qt widget
    explicit ControlPanelLpcView(QWidget *parent = nullptr);

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

    /// @brief Accesses LPC analysis (segmentation) window width
    /// @return Analysis window width, in milliseconds
    /// @note The analysis window width guides both pitch and LPC analysis by
    ///         establishing Frame segmentation boundaries
    float getAnalysisWindowWidth();

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

 private:
    ///////////////////////////////////////////////////////////////////////////
    // Members ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    QLineEdit *analysis_window_line_;

    QCheckBox *hpf_checkbox_;
    QLineEdit *hpf_line_;

    QCheckBox *lpf_checkbox_;
    QLineEdit *lpf_line_;

    QCheckBox *preemphasis_checkbox_;
    QLineEdit *preemphasis_line_;
};

};  // namespace tms_express::ui

#endif  // TMS_EXPRESS_USER_INTERFACES_CONTROL_PANELS_CONTROLPANELLPCVIEW_HPP_
