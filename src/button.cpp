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
            }

            lastButtonStates[i] = buttonState;
        }

        switch (buttonPressed)
        {
        case REC_SW:
            if (currentRecordingMode == NONE)
            {
                currentRecordingMode = SD_CARD;
            }
            else{
                currentRecordingMode = NONE;
            }
            break;
        case BT_SW:
            if (currentBluetoothMode == MODE_DISCONNECTED)
            {
                currentBluetoothMode = MODE_CONNECT;
            }
            else if (currentBluetoothMode != MODE_CONNECT)
            {
                currentBluetoothMode = MODE_DISCONNECT;
            }
            break;
        case MODE_SW:
            switch (currentOperationMode)
            {
            case MODE_LINACQUAD:
                currentOperationMode = MODE_NONE;
                break;
            case MODE_NONE:
                currentOperationMode = MODE_LINACQUAD;
                break;
            }
            break;
        case CLK_SYNC_SW:
            if (currentBluetoothMode == MODE_RECEIVER)
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
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}