// Copyright 2023 Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_USER_INTERFACES_AUDIO_WAVEFORM_AUDIOWAVEFORMVIEW_HPP_
#define TMS_EXPRESS_USER_INTERFACES_AUDIO_WAVEFORM_AUDIOWAVEFORMVIEW_HPP_

#include <QWidget>
#include <QLayout>
#include <QPushButton>

#include <string>
#include <vector>

#include "User_Interfaces/Audio_Waveform/AudioWaveform.hpp"

namespace tms_express::ui {

class AudioWaveformView: public QWidget {
    Q_OBJECT

 public:
    ///////////////////////////////////////////////////////////////////////////
    // Initializers ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Creates a new Audio Waveform view, which encapsulates an Audio
    ///         waveform object in a Qt layout
    /// @param title Title to be displayed atop the layout
    /// @param base_width Base width of widget, in px
    /// @param base_height Base height of widget, in px
    /// @param parent Parent Qt widget
    AudioWaveformView(std::string title, int base_width, int base_height,
        QWidget *parent = nullptr);

    ///////////////////////////////////////////////////////////////////////////
    // Accessors //////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @copydoc AudioWaveform::setSamples(const std::vector<float>&)
    void setSamples(const std::vector<float> &samples);

    /// @copydoc  AudioWaveform::setPitchCurve(const std::vector<float>&);
    void setPitchCurve(const std::vector<float> &pitchTable);

 public slots:
    ///////////////////////////////////////////////////////////////////////////
    // Qt Slots ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Emits Qt signal when user clicks play button, prompting audio
    ///         playback
    void onPlayButtonPressed();

 signals:
    ///////////////////////////////////////////////////////////////////////////
    // Qt Signals /////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    /// @brief Qt signal which prompts audio playback
    int signalPlayButtonPressed(int val);

 private:
    ///////////////////////////////////////////////////////////////////////////
    // Members ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    AudioWaveform *waveform;
    QVBoxLayout *rows_layout;
    QPushButton *play_button;
};

};  // namespace tms_express::ui

#endif  // TMS_EXPRESS_USER_INTERFACES_AUDIO_WAVEFORM_AUDIOWAVEFORMVIEW_HPP_
