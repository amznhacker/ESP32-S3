/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */

#include "Display_ST77916.h"
#include "Audio_PCM5101.h"
#include "RTC_PCF85063.h"
#include "LVGL_Driver.h"
#include "MIC_MSM.h"
#include "SD_Card.h"
#include "LVGL_Example.h"
#include "BAT_Driver.h"
#include "Face_Emote.h"

void Driver_Loop(void *parameter)
{
  while(1)
  {
    PCF85063_Loop();
    BAT_Get_Volts();
    
    // Update face with audio energy
    uint16_t audio_energy = Music_Energy();
    Face_Update_Audio(audio_energy);
    
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
void Driver_Init()
{
  Flash_test();
  BAT_Init();
  I2C_Init();
  TCA9554PWR_Init(0x00);   
  Backlight_Init();
  PCF85063_Init();
}
void setup()
{
  Driver_Init();

  SD_Init();
  Audio_Init();
  MIC_Init();
  LCD_Init();
  Lvgl_Init();

  // Initialize face instead of example UI
  Face_Init();
  
  xTaskCreatePinnedToCore(
    Driver_Loop,           
    "DriverTask",         
    4096,                 
    NULL,                 
    3,                    
    NULL,                 
    0                     
  );  

}
int Time_Loop=0;
void loop() {
  Lvgl_Loop();
  vTaskDelay(pdMS_TO_TICKS(5));

}
