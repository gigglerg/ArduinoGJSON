/**
 * Example use of GJSON (embedded portable JSON library) - In this example a JSON map which has an attribute that itself is 
 * another map is used to generate a JSON string.  JSON builder classes are employed as helpers for attributes/properties.
 * A Local instance of parser and a stacked string is used as I/O.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Length: 30
 * Output: { "a":{ "b":"c" }, "d":null }
 * Done
 * 
 * DG, 2019
 */

// Include
#include "gjson.h"

/*
 * Application maximum JSON property/attribute or value string length (including NULL or any escapes), Characters
 */
#define APP_STRLEN    8


void setup() {
    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    char js[48];
    gjson::UserBuffer ubjio(js, sizeof(js));
    gjson::JSONParse<APP_STRLEN, 0> pr(&ubjio);    // Maximum 0B object chain can be generated.  We don't need them as taking existing objects into string

    // Expect something like: { "a":{ "b":"c" }, "d":null }
    auto a = gjson::JSONMap<APP_STRLEN>();                              // Map(o) >
    auto b = gjson::JSONMapTupleMap<APP_STRLEN>("a", &a);               // String("a") > Map(o)
    auto c = gjson::JSONMapTuple<APP_STRLEN>("b", "c", &b);             // String("b") > String("c") >
    auto d = gjson::JSONMap<APP_STRLEN>(&c,false);                      // Map(c) >
    auto e = gjson::JSONMapTuple<APP_STRLEN>("d", "null", &d, true);    // String("d") > String("null", literal) >
    auto f = gjson::JSONMap<APP_STRLEN>(&e,false);                      // Map(c)

    bool err = pr.FromObject(&a);
    if (!err) {
        Serial.print("Last error: ");
        Serial.println(pr.GetLastError());
    }else {
        Serial.print("Length: ");
        Serial.println(pr.Length());
        Serial.print("Output: ");
        // Get JSON from I/O instance...
        Serial.println(ubjio.Get());
    }
    Serial.println("Done");
}

void loop() {
}