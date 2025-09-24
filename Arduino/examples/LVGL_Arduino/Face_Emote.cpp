#include "Face_Emote.h"

// Face components
static lv_obj_t * face_screen;
static lv_obj_t * left_eye;
static lv_obj_t * right_eye;
static lv_obj_t * mouth;
static lv_obj_t * face_bg;

// Animation states
static bool is_talking = false;
static uint32_t mouth_state = 0;
static lv_timer_t * mouth_timer;
static lv_timer_t * blink_timer;

// BLE variables
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        printf("BLE Client Connected\n");
    }
    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        printf("BLE Client Disconnected\n");
        BLEDevice::startAdvertising();
    }
};

void Face_Init() {
    // Create face screen
    face_screen = lv_obj_create(lv_scr_act());
    lv_obj_set_size(face_screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(face_screen, lv_color_hex(0x000000), 0);
    lv_obj_center(face_screen);
    
    // Create face background (head shape)
    face_bg = lv_obj_create(face_screen);
    lv_obj_set_size(face_bg, 200, 240);
    lv_obj_set_style_bg_color(face_bg, lv_color_hex(0x2D2D2D), 0);
    lv_obj_set_style_radius(face_bg, 100, 0);
    lv_obj_center(face_bg);
    
    // Create eyes
    left_eye = lv_obj_create(face_bg);
    lv_obj_set_size(left_eye, 30, 30);
    lv_obj_set_style_bg_color(left_eye, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_radius(left_eye, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(left_eye, LV_ALIGN_CENTER, -40, -30);
    
    right_eye = lv_obj_create(face_bg);
    lv_obj_set_size(right_eye, 30, 30);
    lv_obj_set_style_bg_color(right_eye, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_radius(right_eye, LV_RADIUS_CIRCLE, 0);
    lv_obj_align(right_eye, LV_ALIGN_CENTER, 40, -30);
    
    // Create mouth
    mouth = lv_obj_create(face_bg);
    lv_obj_set_size(mouth, 60, 20);
    lv_obj_set_style_bg_color(mouth, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_radius(mouth, 10, 0);
    lv_obj_align(mouth, LV_ALIGN_CENTER, 0, 40);
    
    // Start timers
    mouth_timer = lv_timer_create(mouth_animation_cb, 100, NULL);
    blink_timer = lv_timer_create(blink_animation_cb, 3000, NULL);
    
    // Initialize BLE
    BLE_Init();
    
    printf("Face initialized\n");
}

void Face_Update_Audio(uint16_t audio_energy) {
    // Determine if talking based on audio energy
    bool was_talking = is_talking;
    is_talking = (audio_energy > AUDIO_THRESHOLD);
    
    if (is_talking != was_talking) {
        if (is_talking) {
            printf("Started talking - Energy: %d\n", audio_energy);
            lv_timer_set_period(mouth_timer, 80); // Faster mouth animation
        } else {
            printf("Stopped talking\n");
            lv_timer_set_period(mouth_timer, 200); // Slower mouth animation
            Face_Set_Emotion(EMOTION_NEUTRAL);
        }
    }
    
    // Update emotion based on audio energy
    if (is_talking) {
        if (audio_energy > 8000) {
            Face_Set_Emotion(EMOTION_EXCITED);
        } else if (audio_energy > 4000) {
            Face_Set_Emotion(EMOTION_HAPPY);
        } else {
            Face_Set_Emotion(EMOTION_TALKING);
        }
    }
}

void Face_Set_Emotion(face_emotion_t emotion) {
    switch (emotion) {
        case EMOTION_HAPPY:
            lv_obj_set_style_bg_color(left_eye, lv_color_hex(0x00FF00), 0);
            lv_obj_set_style_bg_color(right_eye, lv_color_hex(0x00FF00), 0);
            lv_obj_set_size(mouth, 80, 25);
            lv_obj_set_style_radius(mouth, 15, 0);
            BLE_Send_Emotion("happy");
            break;
            
        case EMOTION_EXCITED:
            lv_obj_set_style_bg_color(left_eye, lv_color_hex(0xFFFF00), 0);
            lv_obj_set_style_bg_color(right_eye, lv_color_hex(0xFFFF00), 0);
            lv_obj_set_size(mouth, 90, 30);
            lv_obj_set_style_radius(mouth, 20, 0);
            BLE_Send_Emotion("excited");
            break;
            
        case EMOTION_TALKING:
            lv_obj_set_style_bg_color(left_eye, lv_color_hex(0x0080FF), 0);
            lv_obj_set_style_bg_color(right_eye, lv_color_hex(0x0080FF), 0);
            BLE_Send_Emotion("talking");
            break;
            
        case EMOTION_NEUTRAL:
        default:
            lv_obj_set_style_bg_color(left_eye, lv_color_hex(0x00FF00), 0);
            lv_obj_set_style_bg_color(right_eye, lv_color_hex(0x00FF00), 0);
            lv_obj_set_size(mouth, 60, 20);
            lv_obj_set_style_radius(mouth, 10, 0);
            BLE_Send_Emotion("neutral");
            break;
    }
    lv_obj_align(mouth, LV_ALIGN_CENTER, 0, 40);
}

void mouth_animation_cb(lv_timer_t * timer) {
    if (is_talking) {
        // Animate mouth for talking
        mouth_state = (mouth_state + 1) % 4;
        switch (mouth_state) {
            case 0: lv_obj_set_size(mouth, 60, 15); break;
            case 1: lv_obj_set_size(mouth, 70, 25); break;
            case 2: lv_obj_set_size(mouth, 65, 20); break;
            case 3: lv_obj_set_size(mouth, 75, 30); break;
        }
    } else {
        // Gentle breathing animation
        mouth_state = (mouth_state + 1) % 2;
        if (mouth_state == 0) {
            lv_obj_set_size(mouth, 60, 20);
        } else {
            lv_obj_set_size(mouth, 62, 22);
        }
    }
    lv_obj_align(mouth, LV_ALIGN_CENTER, 0, 40);
}

void blink_animation_cb(lv_timer_t * timer) {
    // Blink animation
    lv_obj_set_height(left_eye, 5);
    lv_obj_set_height(right_eye, 5);
    
    // Reset after 150ms
    lv_timer_t * reset_timer = lv_timer_create([](lv_timer_t * t) {
        lv_obj_set_height(left_eye, 30);
        lv_obj_set_height(right_eye, 30);
        lv_timer_del(t);
    }, 150, NULL);
    lv_timer_set_repeat_count(reset_timer, 1);
}

void BLE_Init() {
    BLEDevice::init("ESP32-Face");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    
    BLEService *pService = pServer->createService("12345678-1234-1234-1234-123456789abc");
    
    pCharacteristic = pService->createCharacteristic(
        "87654321-4321-4321-4321-cba987654321",
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID("12345678-1234-1234-1234-123456789abc");
    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();
    
    printf("BLE Face service started\n");
}

void BLE_Send_Emotion(const char* emotion) {
    if (deviceConnected && pCharacteristic) {
        pCharacteristic->setValue(emotion);
        pCharacteristic->notify();
    }
}