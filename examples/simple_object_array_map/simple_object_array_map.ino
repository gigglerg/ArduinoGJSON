/**
 * Example use of GJSON (embedded portable JSON library) - In this example a JSON array containing a map that then has some 
 * attribute/properties as objects are used to generate a JSON string.  JSON builder classes are employed as helpers for 
 * attributes/properties.  A global I/O buffer is used as string storage for the JSON parser.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Length: 46
 * Output: [ { "barcode":"90311017", "class":"goods" } ]
 * Done
 *
 * DG, 2019
 */

// Include
#include "gjson.h"

/*
 * Application maximum JSON property/attribute or value string length (including NULL or any escapes), Characters
 */
#define APP_STRLEN    10

/*
 * Application maximum JSON buffered string length (including NULL or any escapes), Characters
 */
#define APP_MAXSTRLEN 64

/*
 * Global I/O buffer + JSON Parsing and building instance
 */
gjson::Buffer<APP_MAXSTRLEN> jio;
gjson::JSONParse<APP_STRLEN, 0> pr(&jio);    // Maximum 0B object chain can be generated.  We don't need them as taking existing objects into string, not creating object chains

void setup() {
    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    // Expect something like: [ { "barcode":"90311017", "class":"goods" } ]
    auto a = gjson::JSONArray<APP_STRLEN>();                            // Array(o) >
    auto b = gjson::JSONMap<APP_STRLEN>(&a);                            // Map(o) >
    auto c = gjson::JSONMapTuple<APP_STRLEN>("barcode", "90311017", &b);// String("aaa") > String("bbb") >
    auto d = gjson::JSONMapTuple<APP_STRLEN>("class", "goods", &c);     // String("ccc") > String("ddd") >
    auto e = gjson::JSONMap<APP_STRLEN>(&d,false);                      // Map(c) >
    auto f = gjson::JSONArray<APP_STRLEN>(&e,false);                    // Array(c)

    bool err = pr.FromObject(&a);
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