/**
 * Example use of GJSON (embedded portable JSON library) - In this example a JSON string is parsed, no object 
 * chain is created.  A specific attribute/property parser is used to look for something and return its value. Local 
 * I/O and parser objects are used for the search.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Input: [ "direction", "backward", "motor", { "interface" : "driver" }, "stepper" ]
 * Found value: driver
 * Done
 * 
 * DG, 2019
 */

// Include
#include "gjson.h"

using namespace gjson;

/*
 * Global JSON string to parse
 */
char js[128] = "[ \"direction\", \"backward\", \"motor\", { \"interface\" : \"driver\" }, \"stepper\" ]";


void setup() {
    UserBuffer ubjio(js, sizeof(js), strlen(js));            // Maximum N, string dimension (including NULL) for JSON string parsing and building
    JSONSimpleStringParseAttr<15, sizeof(js)> pr(&ubjio);    // Maximum 128B object chain can be generated

    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    Serial.print("Input: ");
    Serial.println(ubjio.Get());

    // Look for attribute/property "interface"
    uint16_t fci = pr.FromStringSearch("interface");
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

    Serial.println("Done");
}

void loop() {
}