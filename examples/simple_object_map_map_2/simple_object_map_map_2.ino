/**
 * Example use of GJSON (embedded portable JSON library) - In this example a JSON map which has an attribute that itself is 
 * another map is used to generate a JSON string.  These object chains are created on stack within function calls.  JSON 
 * builder classes are employed as helpers for attributes/properties. A global I/O buffer is used as string storage for the 
 * JSON parser.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Length: 35
 * Output: { "x":true, "abc":{ "v":1.3e-5 } }
 * Length: 38
 * Output: { "a":{ "b":"c", "d":"e" }, "f":"g" }
 * Length: 36
 * Output: { "x":false, "abc":{ "v":1.3e-5 } }
 * Done
 * 
 * DG, 2019
 */

// Include
#include "gjson.h"

using namespace gjson;

/*
 * Application maximum JSON property/attribute or value string length (including NULL or any escapes), Characters
 */
#define APP_STRLEN    8


/*
 * Global I/O buffer + JSON Parsing and building instance
 */
Buffer<64> jio;                       // Maximum 64 character JSON string including NULL we can work with
JSONParse<APP_STRLEN, 0> pr(&jio);    // Maximum 0B object chain can be generated.  We don't need them as taking existing objects into string, not creating object chains


bool json_builder_msg1() {
    // Expect something like: { "a":{ "b":"c", "d":"e" }, "f":"g" }
    auto a = JSONMap<APP_STRLEN>();                              // Map(o) >
    auto b = JSONMapTupleMap<APP_STRLEN>("a", &a);               // String("a") > Map(o) >
    auto c = JSONMapTuple<APP_STRLEN>("b", "c", &b);             // String("b") > String("c") >
    auto d = JSONMapTuple<APP_STRLEN>("d", "e", &c);             // String("d") > String("e") >
    auto e = JSONMap<APP_STRLEN>(&d,false);                      // Map(c) >
    auto f = JSONMapTuple<APP_STRLEN>("f", "g", &e);             // String("f") > String("g") >
    auto g = JSONMap<APP_STRLEN>(&f,false);                      // Map(c)

    return pr.FromObject(&a);
}


bool json_builder_msg2(bool x) {
    char boolean[8];
    
    if (x) {
        strcpy(boolean, "true");
    }else {
        strcpy(boolean, "false");
    }

    // Expect something like: { "x":true, "abc":{ "v":1.3e-5 } }
    auto a = JSONMap<APP_STRLEN>();                              // Map(o) >
    auto b = JSONMapTuple<APP_STRLEN>("x", boolean, &a, true);   // String("x") > String( ["true" | "false"], literal) >
    auto c = JSONMapTupleMap<APP_STRLEN>("abc", &b);             // String("a") > Map(o) >
    auto d = JSONMapTuple<APP_STRLEN>("v", "1.3e-5", &c, true);  // String("b") > String("c") >
    auto e = JSONMap<APP_STRLEN>(&d,false);                      // Map(c) >
    auto f = JSONMap<APP_STRLEN>(&e,false);                      // Map(c)

    return pr.FromObject(&a);
}


void setup() {
    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    // NOTE: Each message builder reuses the global buffer

    if (!json_builder_msg2(true)) {
        Serial.print("Last error: ");
        Serial.println(pr.GetLastError());
    }else {
        Serial.print("Length: ");
        Serial.println(pr.Length());
        Serial.print("Output: ");
        // Get JSON from I/O instance...
        Serial.println(jio.Get());
    }

    if (!json_builder_msg1()) {
        Serial.print("Last error: ");
        Serial.println(pr.GetLastError());
    }else {
        Serial.print("Length: ");
        Serial.println(pr.Length());
        Serial.print("Output: ");
        Serial.println(pr.Get());
    }
    
    if (!json_builder_msg2(false)) {
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
