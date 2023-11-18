// This example demonstrates the ESP RainMaker with a standard Switch device.
#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include "AppInsights.h"

const char *service_name = "PROV_PcSwitch";
const char *pop = "PopPcSwitch";

// Define switch and relay
static int PcSwitch = 34;
static int Relay = 27;

// Global switch state
bool Switch_state = false;

// Switch device
static Switch *my_switch = NULL;

void write_callback(Device *device, Param *param, const param_val_t val,
                    void *priv_data, write_ctx_t *ctx)
{
    const char *device_name = device->getDeviceName();
    const char *param_name = param->getParamName();

    if (strcmp(param_name, "Power") == 0) {
        Serial.printf("Value from rainmaker\nReceived value = %s for %s - %s\n",
                      val.val.b ? "true" : "false", device_name, param_name);
        if (val.val.b != Switch_state) {
            Switch_state = val.val.b;
            powerComputer(Switch_state);
            param->updateAndReport(val);
        }
        else {
            Serial.printf("The device could not be turned %d as it is already in that state\n", val.val.b);
        }
    }
}

void sysProvEvent(arduino_event_t *sys_event)
{
    switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
#if CONFIG_IDF_TARGET_ESP32S2
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n",
                      service_name, pop);
        printQR(service_name, pop, "softap");
#else
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n",
                      service_name, pop);
        printQR(service_name, pop, "ble");
#endif
        break;
    case ARDUINO_EVENT_PROV_INIT:
        wifi_prov_mgr_disable_auto_stop(10000);
        break;
    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
        wifi_prov_mgr_stop_provisioning();
        break;
    default:;
    }
}

// Code for power relay control
void powerComputer(int state)
{
    switch (state) { 
        case 0: // pc on
            Serial.printf("Turn off pc\n");
            digitalWrite(Relay, HIGH);
            delay(50);
            digitalWrite(Relay, LOW);
            break;
        case 1: // pc off
            Serial.printf("Turn on pc\n");
            digitalWrite(Relay, HIGH);
            delay(50);
            digitalWrite(Relay, LOW);
            break;
        case 2: // force off pc
            Serial.printf("Force off pc\n");
            digitalWrite(Relay, HIGH);
            delay(4500);
            digitalWrite(Relay, LOW);
            Switch_state = false;
            my_switch->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,Switch_state);
            break;
        default:
            break;
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(PcSwitch, INPUT_PULLUP);
    pinMode(Relay, OUTPUT);

    //---------- ESP RainMaker initialisation ---------
        // node init
        Node my_node;
        my_node = RMaker.initNode("ESP RainMaker Node");

        // Set switch device variable & node name
        my_switch = new Switch("Computer", &PcSwitch);
        if (!my_switch) {
            return;
        }
        // Standard switch device
        my_switch->addCb(write_callback);

        // Add switch device to the node
        my_node.addDevice(*my_switch);

        // This is optional
        RMaker.enableOTA(OTA_USING_TOPICS);
        RMaker.enableTZService();
        RMaker.enableSchedule();
        RMaker.enableScenes();
        // Enable ESP Insights. Insteads of using the default http transport, this function will
        // reuse the existing MQTT connection of Rainmaker, thereby saving memory space.
        initAppInsights();
        RMaker.enableSystemService(SYSTEM_SERV_FLAGS_ALL, 2, 2, 2);

        RMaker.start();
        // If wifi not provided, start provisioning
        WiFi.onEvent(sysProvEvent);
        #if CONFIG_IDF_TARGET_ESP32S2
            WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE,
                                    WIFI_PROV_SECURITY_1, pop, service_name);
        #else
            WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM,
                                    WIFI_PROV_SECURITY_1, pop, service_name);
        #endif
    // -------------------------------------------------
}

void loop()
{
    if (digitalRead(PcSwitch) == LOW) {  // Push button pressed
        // Key debounce handling
        delay(100);
        int startTime = millis();
        // Key hold handling
        while (digitalRead(PcSwitch) == LOW) {
            delay(50);
        }
        int endTime = millis();

        if ((endTime - startTime) > 20000){ // Key pressed for more then 20secs factory reset
            Serial.printf("Reset to factory.\n");
            RMakerFactoryReset(2);
        } else if ((endTime - startTime) > 10000) { // Key pressed for more then 10secs rest wifi
            Serial.printf("Reset Wi-Fi.\n");
            RMakerWiFiReset(2);
        } else if ((endTime - startTime) > 3000) { // Key pressed for more then 3secs force shutoff
            Serial.printf("Hard stop computer");
            powerComputer(2);
        } else { // Toggle between on and off state
            Switch_state = !Switch_state;
            Serial.printf("Value from switch\nToggle State to %s.\n", Switch_state ? "true" : "false");
            if (my_switch) {
                my_switch->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,Switch_state);
            }
            powerComputer(Switch_state);
        }
    }
    delay(10);
}

