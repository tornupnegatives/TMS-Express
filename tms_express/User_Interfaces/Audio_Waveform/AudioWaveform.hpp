// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_USER_INTERFACES_AUDIO_WAVEFORM_AUDIOWAVEFORM_HPP_
#define TMS_EXPRESS_USER_INTERFACES_AUDIO_WAVEFORM_AUDIOWAVEFORM_HPP_

#include <QWidget>

#include <vector>

namespace tms_express::ui {

/// @brief Time-domain plot of audio samples and pitch
class AudioWaveform : public QWidget {
    Q_OBJECT

 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Creates a new Audio Waveform plot
    /// @param parent Parent Qt widget
    explicit AudioWaveform(QWidget *parent = nullptr);

    ///////////////////////////////////////////////////////////////////////////
    // Accessors //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

     /// @brief Replaces existing samples and repaints plot
    /// @param _samples New samples
    void setSamples(const std::vector<float>& samples);

    /// @brief Replaces existing pitch curve and re-paints plot
    /// @param pitch_curve New pitch table
    void setPitchCurve(const std::vector<float>& pitch_curve);

 protected:
    ///////////////////////////////////////////////////////////////////////////
    // Qt Widget Helpers //////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Re-paints widget
    /// @note This function is invoked by Qt and should be called in
    ///         application code
    void paintEvent(QPaintEvent* event) override;

 private:
    ///////////////////////////////////////////////////////////////////////////
    // Members ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Time-domain pitch curve, corresponding to frequency, in Hertz
    std::vector<float> pitch_curve_;

    /// @brief Time-domain audio samples
    std::vector<float> samples_;
};

};  // namespace tms_express::ui

#endif  // TMS_EXPRESS_USER_INTERFACES_AUDIO_WAVEFORM_AUDIOWAVEFORM_HPP_
