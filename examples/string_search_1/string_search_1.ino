/**
 * Example use of GJSON (embedded portable JSON library) - In this example a JSON string is parsed, no object 
 * chain is created.  A specific attribute/property parser is used to look something and return its value. Global 
 * I/O and parser objects are used for the search.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Input: { "speed" : "fast", "type" : "machine",  "wire" : { "type" : null } , "ctrl" : false }
 * Found value: machine
 * Found value: null
 * Done
 * 
 * DG, 2019
 */

// Include
#include "gjson.h"


/*
 * Global JSON string to parse
 */
char js[128] = "{ \"speed\" : \"fast\", \"type\" : \"machine\",  \"wire\" : { \"type\" : null } , \"ctrl\" : false }";


/*
 * Global I/O buffer + JSON Parsing and building instance
 */
gjson::UserBuffer ubjio(js, sizeof(js), strlen(js));      // Maximum N, string dimension (including NULL) for JSON string parsing and building
gjson::JSONSimpleStringParseAttr<12, 256> pr(&ubjio);     // Maximum 256B (default) object chain can be generated

void setup() {
    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    Serial.print("Input: ");
    Serial.println(ubjio.Get());

    // Look for attribute/property "type"
    uint16_t fci = pr.FromStringSearch("type");
    if (pr.GetLastError() != gjson::JSONConstant::ERR_NONE) {
        Serial.print("Last error: ");
        Serial.print(pr.GetLastError());
        Serial.print(" at character ");
        Serial.println(fci);
        return;
    }else {
        Serial.print("Found value: ");
        Serial.println(pr.GetAttrValue());
    }

    // Continue looking for same attribute/property
    pr.LimitNodeLevel(false);  // So we don't get to the end with an error other than not found so we specifically don't want brackets alignment
    fci = pr.FromStringSearch(fci, "type");
    if (pr.GetLastError() != gjson::JSONConstant::ERR_NONE) {
        Serial.print("Last error: ");
        Serial.print(pr.GetLastError());
        Serial.print(" at character ");
        Serial.println(fci);
        return;
    }else {
        Serial.print("Found value: ");
        Serial.println(pr.GetAttrValue());
    }
    pr.LimitNodeLevel(true);

    Serial.println("Done");
}

void loop() {
}