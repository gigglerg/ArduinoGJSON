/**
 * Example use of GJSON (embedded portable JSON library) - In this example a JSON array containing values all as objects is 
 * used to generate a JSON string.  A global I/O buffer is used as string storage for the JSON parser.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Length: 43
 * Output: [ "apple", "orange", "pineapple", "pear" ]
 * Done
 * 
 * DG, 2019
 */

// Include
#include "gjson.h"

/*
 * Global I/O buffer + JSON Parsing and building instance
 */
gjson::Buffer<64> jio;
gjson::JSONParse<12, 256> pr(&jio);    // Maximum 256B object chain can be generated.  Not required however for this example

void setup() {
    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    // Expect something like: [ "apple", "orange", "pineapple", "pear" ]
    auto a = gjson::JSONArray<12>();                            // Array(o) >
    auto b = gjson::JSONString<12>("apple", &a);                // String("name") >
    auto c = gjson::JSONString<12>("orange", &b);               // String("dave") >
    auto d = gjson::JSONString<12>("pineapple", &c);            // String("name") >
    auto e = gjson::JSONString<12>("pear", &d);                 // String("james") >
    auto f = gjson::JSONArray<12>(&e,false);                    // Array(c)

    bool err = pr.FromObject(reinterpret_cast<gjson::JSONElement<12>*>(&a));
    if (!err) {
        Serial.print("Last error: ");
        Serial.println(pr.GetLastError());
    }else {
        Serial.print("Length: ");
        Serial.println(pr.Length());
        Serial.print("Output: ");
        Serial.println(pr.Get());
    }
    Serial.println("Done");
}

void loop() {
}
