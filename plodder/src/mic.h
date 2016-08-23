/**===========================================================================*\
 * Name        : mic.h
 * Author      : Brett Lynnes
 * Copyright   : 2016 Palo Alto Innovation, all rights reserved
 * Description : setup and read data from the microphone
 \**===========================================================================*/
#ifndef PAI_MIC_H
#define PAI_MIC_H

#include <alsa/asoundlib.h>

typedef struct {
    snd_pcm_uframes_t buffer_frames;
    snd_pcm_t *capture_handle;
    char *buffer;

} micHandle;

class paiMic {
public:
    paiMic();
    snd_pcm_uframes_t getApproxBufferSize();
    snd_pcm_uframes_t getBufferFrames();
    snd_pcm_t * getCaptureHandle();
    bool shutdown();

    static void display_mic_stats(snd_pcm_t *capture_handle, const snd_pcm_hw_params_t *hw_params);
private:
    snd_pcm_uframes_t buffer_frames;
    unsigned int rate;
    int num_channels;
    snd_pcm_format_t format;
    snd_pcm_t *capture_handle;
    const char *mic_device;
    snd_pcm_uframes_t buffer_size;
};

#endif
