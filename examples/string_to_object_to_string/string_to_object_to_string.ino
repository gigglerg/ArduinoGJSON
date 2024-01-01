/**
 * Example use of GJSON (embedded portable JSON library) - In this example a JSON string is parsed and an object chain 
 * created on heap, then this is parsed back into a JSON string. Global I/O and parser object are employed but the I/O 
 * reuses a global string and is used as both input JSON to parse and output JSON when building from object chain.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Input: { "item" : "socket", "where" : "shop" }
 * Size 76
 * Length: 36
 * Output: { "item":"socket", "where":"shop" }
 * Done
 * 
 * DG, 2019
 */

// Include
#include "gjson.h"


using namespace gjson;


/*
 * Global JSON string to parse or build, it is the source of our I/O buffer and could easily be an Arduino serial port data buffer
 */
char js[128] = "{ \"item\" : \"socket\", \"where\" : \"shop\" }";


/*
 * Global I/O buffer + JSON Parsing and building instance
 */
UserBuffer ubjio(js, sizeof(js), strlen(js));  // Maximum N, string dimension (including NULL) for JSON string parsing and building
JSONParse<10, 300> pr(&ubjio);                 // Maximum 300B object chain can be generated


void setup() {
    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    Serial.print("Input: ");
    Serial.println(ubjio.Get());

    // To object chain...
    gjson::JSONElement<10>* ojson = NULL;      // Hold our object chain root
    uint16_t fci = pr.FromString(js, &ojson);
    if (pr.GetLastError() != gjson::JSONConstant::ERR_NONE) {
        pr.Release(&ojson);
        
        Serial.print("Last error: ");
        Serial.print(pr.GetLastError());
        Serial.print(" at character ");
        Serial.println(fci);
        return;
    }
    
    Serial.print("Size ");                     // Bytes on heap
    Serial.println(pr.GetBinarySize());

    // To back to string...
    bool err = pr.FromObject(ojson);
    pr.Release(&ojson);                        // Release JSON object chain, we don't need it anymore
    if (!err) {
        Serial.print("Last error: ");
        Serial.println(pr.GetLastError());
        return;
    }else {
        // Display built JSON string from our I/O instance
        Serial.print("Length: ");
        Serial.println(pr.Length());
        Serial.print("Output: ");
        // Print alter JS string from original variable
        Serial.println(js);
    }

    Serial.println("Done");
}

void loop() {
}
