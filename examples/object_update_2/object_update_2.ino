/**
 * Example use of GJSON (embedded portable JSON library) - In this example a JSON object chain on the stack containing 
 * some attribute/properties is searched, a value updated, then parsed to a JSON string.  A global I/O buffer is used 
 * with JSON parser.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Find string(s)
 * Attribute: material
 * Length: 40
 * Output: { "door":"handle", "material":"steel" }
 * Done
 * 
 * DG, 2019
 */

// Include
#include "gjson.h"

/*
 * Global I/O buffer + JSON Parsing and building instance
 */
gjson::Buffer<48> jio;                         // Maximum 32 (including NULL) character JSON string
gjson::JSONParse<12, 100> pr(&jio);            // Maximum 100B object chain can be generated, however this isn't required for this example

void setup() {
    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    // Expect something like: { "door":"handle", "material":"brass" }
    auto a = gjson::JSONMap<12>();                                // Map(o) >
    auto b = gjson::JSONMapTuple<12>("door", "handle", &a);       // String("door") > String("handle") >
    auto c = gjson::JSONMapTuple<12>("material", "brass", &b);    // String("material") > String("brass") >
    auto d = gjson::JSONMap<12>(&c,false);                        // Map(c)

    Serial.println("Find string(s)");

    // Find material attribute
    auto dd = pr.FindString(&a, "material", false);
    if (pr.GetLastError() != gjson::JSONConstant::ERR_NONE || NULL==dd) {
        Serial.print("Last error: ");
        Serial.print(pr.GetLastError());
        return;
    }
    Serial.print("Attribute: ");
    Serial.println(dd->Data());

    // Move to next value string
    dd = pr.NextString(dd);

    // Update
    dd->Data("steel");

    // To string, should include the update...
    bool err = pr.FromObject(&a);
    if (!err) {
        Serial.print("Last error: ");
        Serial.println(pr.GetLastError());
        return;
    }else {
        // Display built JSON string from our I/O instance
        Serial.print("Length: ");
        Serial.println(pr.Length());
        Serial.print("Output: ");
        Serial.println(pr.Get());
    }

    Serial.println("Done");
}

void loop() {
}
