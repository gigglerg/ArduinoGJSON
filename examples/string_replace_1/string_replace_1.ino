/**
 * Example use of GJSON (embedded portable JSON library) - In this example a JSON string is parsed looking for an  
 * attribute/property, then a later attribute/property with a known name has its value replaced.  Local  I/O and 
 * parser objects are used with a global string as I/O source.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Input: { "test":{ "date": "25Oct19" }, "time" : "12.45.34", "date" : "29Oct19", "code" : "3" }
 * Found value: 12.45.34
 * Replaced proceeding date with 26Oct19
 * Output: { "test":{ "date": "25Oct19" }, "time" : "12.45.34", "date" : "26Oct19", "code" : "3" }
 * Done
 * 
 * DG, 2019
 */

// Include
#include "gjson.h"

using namespace gjson;

/*
 * Global JSON string to parse or build
 */
char js[128] = "{ \"test\":{ \"date\": \"25Oct19\" }, \"time\" : \"12.45.34\", \"date\" : \"29Oct19\", \"code\" : \"3\" }";


void setup() {
    UserBuffer ubjio(js, sizeof(js), strlen(js));                // Maximum N, string dimension (including NULL) for JSON string parsing and building
    JSONSimpleStringParseAttr<15, sizeof(js)> pr(&ubjio);        // Maximum 256B (default) object chain can be generated

    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    Serial.print("Input:  ");
    Serial.println(ubjio.Get());

    // Look for attribute/property "time"
    uint16_t fci = pr.FromStringSearch("time");
    if (pr.GetLastError() != JSONConstant::ERR_NONE) {
        Serial.print("Last error: ");
        Serial.print(pr.GetLastError());
        Serial.print(" at character ");
        Serial.println(fci);
        return;
    }else {
        Serial.print("Found value: ");
        Serial.println(pr.GetAttrValue());
    }

    // We've moved over the test date as we searched for time, so we should replace the next date, 29Oct19 with another
    pr.LimitNodeLevel(false);  // Optional, so we don't get to the end with an error other than not found so we specifically don't want to make sure brackets align (we started parsing at some place in middle of JSON string)
    fci = pr.FromStringReplace(fci, "date", "26Oct19");
    if (pr.GetLastError() != JSONConstant::ERR_NONE) {
        Serial.print("Last error: ");
        Serial.print(pr.GetLastError());
        Serial.print(" at character ");
        Serial.println(fci);
        return;
    }else {
        Serial.print("Replaced proceeding date with ");
        Serial.println(pr.GetAttrValue());
    }
    pr.LimitNodeLevel(true);

    Serial.print("Output: ");
    Serial.println(ubjio.Get());

    Serial.println("Done");
}

void loop() {
}