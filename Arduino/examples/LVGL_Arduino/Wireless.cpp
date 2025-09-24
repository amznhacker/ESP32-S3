#include "Wireless.h"

// Removed WiFi functionality to save RAM for BLE face system
bool WIFI_Connection = 0;
uint8_t WIFI_NUM = 0;
uint8_t BLE_NUM = 0;
bool Scan_finish = 1;  // Always finished since we're not scanning
int wifi_scan_number()
{
  // WiFi disabled to save RAM for BLE face system
  printf("WiFi disabled - using BLE for face emotions\r\n");
  return 0;
}
int ble_scan_number()
{
  printf("/**********BLE Test**********/\r\n"); 
  BLEDevice::init("ESP32");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);

  BLEScanResults* foundDevices = pBLEScan->start(5);  
  int count = foundDevices->getCount();
  if (count == 0)
  {
    printf("No Bluetooth device was scanned\r\n");
  }
  else{
    printf("Scanned %d Bluetooth devices\r\n",count);
  }
  pBLEScan->stop(); 
  pBLEScan->clearResults(); 
  BLEDevice::deinit(true); 
  vTaskDelay(100);         
  printf("/**********BLE Test Over**********/\r\n\r\n");
  return count;
}
extern char buffer[128];    /* Make sure buffer is enough for `sprintf` */
void Wireless_Test1(){
  // WiFi removed, BLE handled by Face_Emote system
  WIFI_NUM = 0;
  BLE_NUM = 1;  // BLE is active for face emotions
  Scan_finish = 1;
}

void WirelessScanTask(void *parameter) {
  // No scanning needed - BLE handled by Face system
  WIFI_NUM = 0;
  BLE_NUM = 1;
  Scan_finish = 1;
  vTaskDelay(pdMS_TO_TICKS(1000));
  vTaskDelete(NULL);
}
void Wireless_Test2(){
  // Simplified - no task needed
  WIFI_NUM = 0;
  BLE_NUM = 1;
  Scan_finish = 1;
}
