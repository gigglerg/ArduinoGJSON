/**
 * Example use of GJSON (embedded portable JSON library) - In this example a JSON map containing attributes/property and 
 * values as objects is used to generate a JSON string.  Local instance of parser and a stacked string is used as I/O.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Length: 33
 * Output: { "item":"computer", "id":1234 }
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

    // Expect something like: { "item":"computer", "id":345 }
    char js[48];
    UserBuffer ubjio(js, sizeof(js));
    gjson::JSONParse<12, 0> pr(&ubjio);    // Maximum 0B object chain can be generated.  We don't need them as taking existing objects into string

    auto a = JSONMap<12>();                                      // Map(o) >
    auto b = JSONString<12>("item", &a, false);                  // String("item", attr) >
    auto c = JSONString<12>("computer", &b);                     // String("computer", value) >
    auto d = JSONString<12>("id", &c, false);                    // String("id", attr) >
    auto e = JSONString<12>("1234", &d, true, true);             // String("1234", value, literal) >
    auto f = JSONMap<12>(&e,false);                              // Map(c)

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