/**
 * Example use of GJSON (embedded portable JSON library) - In this example a JSON object chain on the stack containing 
 * some value strings is searched, a value updated, then parsed to a JSON string.  A global I/O buffer is used with JSON 
 * parser.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Find string(s)
 * Find value string yellow, update it with green
 * Length: 40
 * Output: [ "cyan", "magenta", "green", "black" ]
 * Done
 *
 * DG, 2019
 */

// Include
#include "gjson.h"

using namespace gjson;

/*
 * Global I/O buffer + JSON Parsing and building instance
 */
Buffer<48> jio;                         // Maximum 48 (including NULL) character JSON string
JSONParse<10, 300> pr(&jio);            // Maximum 300B object chain can be generated, however thisn't required for this example

void setup() {
    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    // Expect something like: [ "cyan", "magenta", "yellow", "black" ]
    auto a = JSONArray<10>();                             // Array(o) >
    auto b = JSONString<10>("cyan", &a);                  // String("cyan") >
    auto c = JSONString<10>("magenta", &b);               // String("magenta") >
    auto d = JSONString<10>("yellow", &c);                // String("yellow") >
    auto e = JSONString<10>("black", &d);                 // String("black") >
    auto f = JSONArray<10>(&e,false);                     // Array(c)

    Serial.println("Find string(s)");

    // Find first value of yellow
    auto dd = pr.FindString(&a, "yellow");
    if (pr.GetLastError() != JSONConstant::ERR_NONE || NULL==dd) {
        Serial.print("Last error: ");
        Serial.print(pr.GetLastError());
        return;
    }
    Serial.print("Find value string ");
    Serial.print(dd->Data());
    Serial.println(", update it with green");

    // Update
    dd->Data("green");

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