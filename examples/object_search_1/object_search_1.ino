/**
 * Example use of GJSON (embedded portable JSON library) - In this example a JSON object chain on the stack containing 
 * some value strings with specific duplicated text is searched.  A global NULL I/O buffer is used with a JSON parser.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Find string(s)
 * Found value string orange
 * Found value string ORAnge
 * Find next array close
 * Found: 8F4, should be 8F4
 * Done
 *
 * DG, 2019
 */

// Include
#include "gjson.h"

/*
 * Global I/O buffer + JSON Parsing and building instance
 */
gjson::NullBuffer njio;               // We arn't creating a JSON string so no buffer
gjson::JSONParse<12, 0> pr(&njio);    // Maximum 0B object chain can be generated.  Not required for this example

void setup() {
    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    // Expect something like: [ "apple", "orange", "pineapple", "another" : [ "ORAnge" ], "pear" ]
    auto a = gjson::JSONArray<12>();                             // Array(o) >
    auto b = gjson::JSONString<12>("apple", &a);                 // String("apple") >
    auto c = gjson::JSONString<12>("orange", &b);                // String("orange") >
    auto d = gjson::JSONString<12>("pineapple", &c);             // String("pineapple") >
    auto e = gjson::JSONMapTupleArray<12>("another", &d);        // String("another") > Map(o) >
    auto f = gjson::JSONString<12>("ORAnge", &e);                // String("ORAnge") >
    auto g = gjson::JSONArray<12>(&f,false);                     // Array(c)
    auto h = gjson::JSONString<12>("pear", &g);                  // String("pear") >
    auto i = gjson::JSONArray<12>(&h,false);                     // Array(c)

    Serial.println("Find string(s)");

    // Find first orange
    auto dd = pr.FindString(&a, "orange", true, false);
    if (pr.GetLastError() != gjson::JSONConstant::ERR_NONE || NULL==dd) {
        Serial.print("Last error: ");
        Serial.print(pr.GetLastError());
        return;
    }
    Serial.print("Found value string ");
    Serial.println(dd->Data());

    if (dd->Next()) {
        // Find 2nd ORAnge
        auto ee = pr.FindString(dd->Next(), "orange", true, false);
        if (pr.GetLastError() != gjson::JSONConstant::ERR_NONE || NULL==dd) {
            Serial.print("Last error: ");
            Serial.print(pr.GetLastError());
            return;
        }
        Serial.print("Found value string ");
        Serial.println(ee->Data());
    
        if (ee->Next()) {
            // Find next array closing
            Serial.println("Find next array close");
            auto ac = pr.NextArray(dd->Next(), false);
            if (!ac) {
                Serial.print("Last error: ");
                Serial.print(pr.GetLastError());
                return;
            }
            Serial.print("Found: ");
            Serial.print(reinterpret_cast<uint32_t>(ac), HEX);
            Serial.print(", should be ");
            Serial.println(reinterpret_cast<uint32_t>(&g), HEX);
        }else {
            Serial.println("Expected ee->Next() != NULL");
            return;
        }
    }else {
        Serial.println("Expected dd->Next() != NULL");
        return;
    }

    Serial.println("Done");
}

void loop() {
}