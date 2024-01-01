/**
 * Example use of GJSON (embedded portable JSON library) - In this example a JSON object chain on the stack containing a
 * map of attributes which is searched and then the values in the adjacent property array are iterated over.  A global 
 * NULL I/O buffer is used with a JSON parser.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Find string(s)
 * Attribute string: marmalade
 * Value string: orange
 * Value string: lemon
 * Value string: lime
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

    // Expect something like: { "jam":[ "raspberry", "strawberry" ], "marmalade" : [ "orange", "lemon", "lime" ] }
    auto a = gjson::JSONMap<12>();                               // Map(o) >
    auto b = gjson::JSONMapTupleArray<12>("jam", &a);            // String("jam") > Map(o) >
    auto c = gjson::JSONString<12>("raspberry", &b);             // String("raspberry") >
    auto d = gjson::JSONString<12>("strawberry", &c);            // String("strawberry") >
    auto e = gjson::JSONArray<12>(&d,false);                     // Array(c)
    auto f = gjson::JSONMapTupleArray<12>("marmalade", &e);      // String("marmalade") > Array(o) >
    auto g = gjson::JSONString<12>("orange", &f);                // String("orange") >
    auto h = gjson::JSONString<12>("lemon", &g);                 // String("orange") >
    auto i = gjson::JSONString<12>("lime", &h);                  // String("lime") >
    auto j = gjson::JSONArray<12>(&i,false);                     // Array(c)
    auto k = gjson::JSONMap<12>(&j,false);                       // Map(c)

    Serial.println("Find string(s)");

    // Find attribute marmalade
    auto mm = pr.FindString(&a, "marmalade", false);
    if (pr.GetLastError() != gjson::JSONConstant::ERR_NONE || NULL==mm) {
        Serial.print("Last error: ");
        Serial.print(pr.GetLastError());
        return;
    }
    Serial.print("Attribute string: ");
    Serial.println(mm->Data());

    // Iterate over value strings after above find
    do {
        auto ns = pr.NextString(mm);

        if (ns) {
            Serial.print("Value string: ");
            Serial.println(ns->Data());

            mm = ns;
        }else {
            break;
        }
    }while(1);

    Serial.println("Done");
}

void loop() {
}
