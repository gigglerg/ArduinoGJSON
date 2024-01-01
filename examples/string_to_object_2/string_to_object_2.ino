/**
 * Example use of GJSON (embedded portable JSON library) - In this example JSON string is parsed and an object chain 
 * created on heap, it is then dumped (object parsed) to debug serial.  Local instance of parser(s) are used and no
 * I/O is allocated or setup as no JSON string is created.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Str: [1,2,3,4,{"5":6}]
 * Size 130
 * [
 * LVal(1)
 * LVal(2)
 * LVal(3)
 * LVal(4)
 * {
 * Prop(5)
 * LVal(6)
 * }
 * ]
 * Done
 * 
 * DG, 2019
 */

// Include
#include "gjson.h"

/*
 * Application maximum JSON property/attribute or value string length (including NULL or any escapes), Characters
 */
#define APP_STRLEN    12


void setup() {
    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    // JSON string to parse:
    char js[] = "[1,2,3,4,{\"5\":6}]";

    Serial.print("Str: ");
    Serial.println(js);

    gjson::NullBuffer njio;                            // We arn't creating a JSON string so no buffer
    gjson::JSONParse<APP_STRLEN, 0> pr(&njio, 256);    // 256B maximum object chain on heap
    gjson::JSONElement<APP_STRLEN>* a = NULL;          // Hold our object chain root

    // Convert JSON string to object chain
    uint16_t fci = pr.FromString(js, &a);

    // Error check object chain creation
    if (pr.GetLastError() != gjson::JSONConstant::ERR_NONE) {
        pr.Release(&a);
        
        Serial.print("Last error: ");
        Serial.print(pr.GetLastError());
        Serial.print(" at character ");
        Serial.println(fci);
        return;
    }
    
    Serial.print("Size ");                     // Bytes on heap of object chain
    Serial.println(pr.GetBinarySize());

    // Dump (parse) object chain to debug serial
    gjson::JSONExampleObjectParse<12> cpr;
    bool err = cpr.FromObject(a);
    pr.Release(&a);                            // Dont do anything with object chain in this example, just release it
    if (!err) {
        Serial.print("Last error: ");
        Serial.print(cpr.GetLastError());
        return;
    }

    Serial.println("Done");
}

void loop() {
}
