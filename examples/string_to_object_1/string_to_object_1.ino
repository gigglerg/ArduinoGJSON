/**
 * Example use of GJSON (embedded portable JSON library) - In this example JSON string is parsed and an object chain 
 * created on heap.  Local instance of parser and a stacked string is used as I/O.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Str: [ { "hello":{ "world":"testing", "one":"two" }, "three":"four" } ]
 * Size 157
 * Done
 *
 * DG, 2019
 */

// Include
#include "gjson.h"

using namespace gjson;

void setup() {
    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    // JSON string to parse:
    char js[] = "[ { \"hello\":{ \"world\":\"testing\", \"one\":\"two\" }, \"three\":\"four\" } ]";

    Serial.print("Str: ");
    Serial.println(js);

    NullBuffer njio;                      // We arn't creating a JSON string so no buffer
    JSONParse<12, 0> pr(&njio, 256);      // 256B maximum object chain on heap
    JSONElement<12>* a = NULL;            // Hold our object chain root
    uint16_t fci = pr.FromString(js, &a);
    
    Serial.print("Size ");                // Bytes on heap
    Serial.println(pr.GetBinarySize());

    // Dont do anything with object chain in this example, just release it
    pr.Release(&a);

    // Error check object chain creation
    if (pr.GetLastError() != JSONConstant::ERR_NONE) {
        Serial.print("Last error: ");
        Serial.print(pr.GetLastError());
        Serial.print(" at character ");
        Serial.println(fci);
        return;
    }

    Serial.println("Done");
}

void loop() {
}