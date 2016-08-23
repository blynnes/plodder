/**===========================================================================*\
 * Name        : mic.cpp
 * Author      : Brett Lynnes
 * Copyright   : 2016 Palo Alto Innovation, all rights reserved
 * Description : setup and read data from the microphone
 \**===========================================================================*/

#include <iostream>

#include "mic.h"

using namespace std;

/*
 * Was having some problems making the mic work (turns out i was linking the wrong asound.so when cross compiling). call this to print stats on the
 * current state of the mic
 */
void paiMic::display_mic_stats(snd_pcm_t *capture_handle, const snd_pcm_hw_params_t *hw_params) {

    unsigned int val;
    snd_pcm_uframes_t frames;
    snd_pcm_format_t fmt;
    int dir;

    cout << " **************   DISPLAYING MIC STATS   *************** " << endl << endl;
    cout << "Card handler name: " << snd_pcm_name(capture_handle) << endl;
    cout << "Current state: " << snd_pcm_state_name(snd_pcm_state(capture_handle)) << endl;
    snd_pcm_hw_params_get_access(hw_params, (snd_pcm_access_t *) &val);
    cout << "Access type: " << snd_pcm_access_name((snd_pcm_access_t) val) << endl;
    snd_pcm_hw_params_get_format(hw_params, &fmt);
    cout << "Format: ( " << snd_pcm_format_name((snd_pcm_format_t) fmt) << ") " << snd_pcm_format_description((snd_pcm_format_t) fmt) << endl;
    snd_pcm_hw_params_get_channels(hw_params, &val);
    cout << "Channels: " << val << endl;
    snd_pcm_hw_params_get_rate(hw_params, &val, &dir);
    cout << "Rate: " << val << endl;
    snd_pcm_hw_params_get_period_time(hw_params, &val, &dir);
    cout << "Period Time: " << val << " us." << endl;
    snd_pcm_hw_params_get_period_size(hw_params, &frames, &dir);
    cout << "Period in Frames: " << frames << endl;
    snd_pcm_hw_params_get_buffer_time(hw_params, &val, &dir);
    cout << "Buffer time: " << val << " us." << endl;
    snd_pcm_hw_params_get_buffer_size(hw_params, &frames);
    cout << "Buffer size in Frames: " << frames << endl;
    snd_pcm_hw_params_get_periods(hw_params, &val, &dir);
    cout << "Periods: " << val << endl;
    cout << " ******************************************************* " << endl;
}

paiMic::paiMic() {
    bool verbose = true;
    buffer_frames = 32;
    rate = 16000;
    int num_channels = 1;
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
    string _mic_device = "default";
    mic_device = _mic_device.c_str();

    int err;
    int dir = 0;
    snd_pcm_hw_params_t *hw_params;

    err = snd_pcm_open(&capture_handle, mic_device, SND_PCM_STREAM_CAPTURE, 0);
    if (err < 0) {
        cerr << "Cannot open mic device " << mic_device << " (" << snd_strerror(err) << ")" << endl;
        exit(1);
    }
    if (verbose) {
        cout << "mic_device opened" << endl;
    }

    snd_pcm_hw_params_alloca(&hw_params);
    cout << "hw_params allocated" << endl;

    err = snd_pcm_hw_params_any(capture_handle, hw_params);
    if (err < 0) {
        cerr << "Cannot initialize hw_params to defaults" << snd_strerror(err) << endl;
        exit(1);
    }
    if (verbose) {
        cout << "hw_params initialized" << endl;
    }

    err = snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) {
        cerr << "Cannot set access type" << snd_strerror(err) << endl;
        exit(1);
    }
    if (verbose) {
        cout << "hw_params access type set" << endl;
    }

    err = snd_pcm_hw_params_set_format(capture_handle, hw_params, format);
    if (err < 0) {
        cerr << "Cannot set sample format" << snd_strerror(err) << endl;
        exit(1);
    }
    if (verbose) {
        cout << "hw_params sample format set" << endl;
    }

    if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, num_channels)) < 0) {
        cerr << "Cannot set num channels" << snd_strerror(err) << endl;
        exit(1);
    }
    if (verbose) {
        cout << "hw_params num channels set" << endl;
    }

    if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &rate, &dir)) < 0) {
        cerr << "Cannot set sample rate" << snd_strerror(err) << endl;
        exit(1);
    }
    if (verbose) {
        cout << "hw_params rate set to " << rate << endl;
    }

    err = snd_pcm_hw_params_set_period_size_near(capture_handle, hw_params, &buffer_frames, &dir);
    if (err < 0) {
        cerr << "Error setting period size: " << snd_strerror(err) << " (" << err << ")" << endl;
        exit(1);
    }
    if (verbose) {
        cout << "Period size in frames set to: " << buffer_frames << endl;
    }

    buffer_size = buffer_frames * 2 * num_channels; /* 2 bytes/sample, 1 channels */
    err = snd_pcm_hw_params_set_buffer_size_near(capture_handle, hw_params, &buffer_size);
    if (err < 0) {
        cerr << "Error setting period size: " << snd_strerror(err) << endl;
        exit(1);
    }
    if (verbose) {
        cout << "Buffer size set to: " << buffer_size << endl;
    }

    display_mic_stats(capture_handle, hw_params);

    /*err = snd_pcm_prepare(capture_handle);
     if (err < 0) {
     cerr << "Cannot prepare audio interface for use" << snd_strerror(err) << endl;
     exit(1);
     }*/

    if ((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0) {
        cerr << "Cannot set parameters (" << err << ") " << snd_strerror(err) << endl;
        exit(1);
    }

    cout << "hw_params set, microphone ready" << endl;
}

snd_pcm_uframes_t paiMic::getBufferFrames() {
    return buffer_frames;
}

snd_pcm_uframes_t paiMic::getApproxBufferSize() {
    return buffer_size;
}

snd_pcm_t * paiMic::getCaptureHandle() {
    return capture_handle;
}

bool paiMic::shutdown() {
    snd_pcm_close(capture_handle);
    cout << "Audio capture interface closed" << endl;
    return true;
}
