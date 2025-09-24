#pragma once

#include <lvgl.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define AUDIO_THRESHOLD 1500  // Threshold for detecting speech

typedef enum {
    EMOTION_NEUTRAL,
    EMOTION_HAPPY,
    EMOTION_EXCITED,
    EMOTION_TALKING
} face_emotion_t;

// Face functions
void Face_Init();
void Face_Update_Audio(uint16_t audio_energy);
void Face_Set_Emotion(face_emotion_t emotion);

// Animation callbacks
void mouth_animation_cb(lv_timer_t * timer);
void blink_animation_cb(lv_timer_t * timer);

// BLE functions
void BLE_Init();
void BLE_Send_Emotion(const char* emotion);