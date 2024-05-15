#include <button.h>

TaskHandle_t buttonTaskHandle = NULL;

void buttonTask(void *pvParameters)
{

    const int buttonPins[] = {REC_SW_PIN, BT_SW_PIN, MODE_SW_PIN, CLK_SYNC_SW_PIN};

    enum Buttons
    {
        None = 0,
        REC_SW = 1,
        BT_SW = 2,
        MODE_SW = 3,
        CLK_SYNC_SW = 4
    };

    int lastButtonStates[] = {HIGH, HIGH, HIGH, HIGH};
    long lastDebounceTimes[] = {0, 0, 0, 0};
    long debounceDelay = 100;

    int buttonPressed = None;

    // initialize buttons
    for (int i = 0; i < 4; i++)
    {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }

    vTaskDelay(100 / portTICK_PERIOD_MS); // buttons can 

    // loop
    for (;;)
    {
        for (int i = 0; i < 4; i++)
        {
            int buttonState = digitalRead(buttonPins[i]);

            // check for state change from HIGH to LOW
            if (buttonState == LOW && lastButtonStates[i] == HIGH && millis() - lastDebounceTimes[i] > debounceDelay)
            {
                // button state changed from HIGH to LOW
                lastDebounceTimes[i] = millis();

                buttonPressed = i + 1;
                vTaskDelay(200 / portTICK_PERIOD_MS);
            }

            lastButtonStates[i] = buttonState;
        }

        switch (buttonPressed)
        {
        case REC_SW:
            if (currentRecordingMode == NONE)
            {
                displayNotification("Recording...");
                currentRecordingMode = SD_CARD;
            }
            else{
                displayNotification("Recording stopped");
                currentRecordingMode = NONE;
            }
            break;
        case BT_SW:
            if (currentBluetoothMode == MODE_DISCONNECTED)
            {
                currentBluetoothMode = MODE_CONNECT;
                displayNotification("Connecting...");
            }
            else if (currentBluetoothMode != MODE_CONNECT)
            {
                currentBluetoothMode = MODE_DISCONNECT;
                displayNotification("Disconnecting...");
            }
            break;
        case MODE_SW:
            switch (currentOperationMode)
            {
            case MODE_LINACQUAD:
                currentOperationMode = MODE_TEMP;
                displayNotification("Temperature");
                break;
            case MODE_TEMP:
                currentOperationMode = MODE_LINACQUAD;
                displayNotification("L-Accel&Orientation");
                break;
            }
            break;
        case CLK_SYNC_SW:
            if (currentBluetoothMode == MODE_CONNECTED)
            {
                currentBluetoothMode = MODE_CLK_SYNC;
            }
            break;
        default:
            // no button pressed
            break;
        }
        buttonPressed = 0;

        // Yield to other tasks
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}