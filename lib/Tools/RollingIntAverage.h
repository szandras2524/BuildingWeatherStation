/*
   András Székely
   Copyright (C) 2021-2023, Hungary
   Released under the MIT License.
*/

#include <Arduino.h>


class RollingIntAverage {
private:
    int* values;      // Pointer for dynamic array storage
    uint8_t maxLength; // Maximum number of elements
    int count = 0;    // Current count of added values
    float sum = 0;    // Sum of all current values in the array


public:

    // Constructor and destructor for managing dynamic memory
    RollingIntAverage() : values(nullptr), maxLength(0) {}
    ~RollingIntAverage() {
        delete[] values;
    }

    // Function to initialize or reinitialize the array size
    void begin(uint8_t size) {
        if (values != nullptr) {
            delete[] values; // Free existing array if it exists
        }
        maxLength = size;
        values = new int[maxLength]; // Allocate new array of given size
        count = 0;
        sum = 0;
        for (int i = 0; i < maxLength; i++) {
            values[i] = 0;
        }
    }


    // Function to add a new value and calculate the rolling average
    float addValue(int newValue) {
        // Add new value to the sum
        sum += newValue;

        // If the array is not yet full, simply add the value
        if (count < maxLength ) {
            values[count] = newValue;
            count++;
        } else {
            // If the array is full, replace the oldest value and adjust the sum
            sum -= values[0]; // Subtract the oldest value from the sum

            // Shift all elements to the left
            for (int i = 1; i < maxLength ; i++) {
                values[i - 1] = values[i];
            }

            // Add the new value at the end
            values[maxLength  - 1] = newValue;
        }

        // Calculate the average
        Serial.println("Count: " + String(count) + " Sum: " + String(sum));
        for(int i = 0; i < count; i++) {
            Serial.print( String(i) + ":" + String(values[i]) + ", ");
        }
        Serial.println();
        return sum / count;
    }
};
