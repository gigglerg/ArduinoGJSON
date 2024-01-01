/**
 * Example use of GJSON (embedded portable JSON library) - In this example a JSON string is parsed looking for an
 * attribute/property which is then renamed in string.  Local  I/O and parser objects are used with a global string 
 * as I/O source.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Input: { "switch" : "on", "power" : "off", "control" : "state" }
 * Renamed power with information
 * Find power
 * Didnt find it. Good, error code was 6
 * Information found value: off
 * Done
 * 
 * DG, 2019
 */

// Include
#include "gjson.h"


/*
 * Global JSON string to parse or build
 */
char js[128] = "{ \"switch\" : \"on\", \"power\" : \"off\", \"control\" : \"state\" }";


void setup() {
    gjson::UserBuffer ubjio(js, sizeof(js), strlen(js));           // Maximum N, string dimension (including NULL) for JSON string parsing and building
    gjson::JSONSimpleStringParseAttr<15, sizeof(js)> pr(&ubjio);   // Maximum 256B (default) object chain can be generated

    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    Serial.print("Input: ");
    Serial.println(ubjio.Get());


    // Rename power (attribute) to information
    uint16_t fci = pr.FromStringRename("power", "information");
    if (pr.GetLastError() != gjson::JSONConstant::ERR_NONE) {
        Serial.print("Last error: ");
        Serial.print(pr.GetLastError());
        Serial.print(" at character ");
        Serial.println(fci);
        return;
    }else {
        Serial.print("Renamed power with ");
        Serial.println(pr.GetAttrValue());
    }

    // Look for attribute/property "power"
    Serial.println("Find power");
    fci = pr.FromStringSearch("power");
    if (pr.GetLastError() != gjson::JSONConstant::ERR_NONE) {
        Serial.print("Didnt find it. Good, error code was ");
        Serial.println(pr.GetLastError());
    }else {
        Serial.print("This is wrong.  Found value: ");
        Serial.println(pr.GetAttrValue());
        return;
    }

    // Look for attribute/property "information"
    fci = pr.FromStringSearch("information");
    if (pr.GetLastError() != gjson::JSONConstant::ERR_NONE) {
        Serial.print("Last error: ");
        Serial.print(pr.GetLastError());
        Serial.print(" at character ");
        Serial.println(fci);
        return;
    }else {
        Serial.print("Information found value: ");
        Serial.println(pr.GetAttrValue());
    }

    Serial.println("Done");
}

void loop() {
}