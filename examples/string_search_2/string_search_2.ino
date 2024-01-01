/**
 * Example use of GJSON (embedded portable JSON library) - In this example a JSON string is parsed, no object 
 * chain is created.  A specific attribute/property parser is used to look something and return its value. Global 
 * I/O and parser objects are used for the search.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * Input: [100,200,300,{"value":"1000"},2000,"5",10,20]
 * Look for 200
 * Found at character index 5
 * Continue to look for 2000
 * Found at character index 30
 * Look for 10
 * Found at character index 39
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
 * Application maximum JSON buffered string length (including NULL or any escapes), Characters
 */
#define APP_MAXSTRLEN 96


void setup() {
    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    static char js[APP_MAXSTRLEN] = "[100,200,300,{\"value\":\"1000\"},2000,\"5\",10,20]";

    gjson::UserBuffer ubjio(js, sizeof(js), strlen(js));
    JSONSimpleStringParse<APP_STRLEN, sizeof(js)> pr(&ubjio);

    Serial.print("Input: ");
    Serial.println(ubjio.Get());

    // Look for any string as "200"
    Serial.println("Look for 200");
    uint16_t fci = pr.FromStringSearch("200");
    if (pr.GetLastError() != JSONConstant::ERR_NONE) {
        Serial.print("Last error: ");
        Serial.print(pr.GetLastError());
        Serial.print(" at character ");
        Serial.println(fci);
        return;
    }else {
        Serial.print("Found at character index ");
        Serial.println(fci);
    }

    // Continue looking for "2000"
    Serial.println("Continue to look for 2000");
    pr.LimitNodeLevel(false);
    fci = pr.FromStringSearch(fci, "2000");
    if (pr.GetLastError() != JSONConstant::ERR_NONE) {
        Serial.print("Last error: ");
        Serial.print(pr.GetLastError());
        Serial.print(" at character ");
        Serial.println(fci);
        return;
    }else {
        Serial.print("Found at character index ");
        Serial.println(fci);
    }
    pr.LimitNodeLevel(true);

    // Look for any string as "10"
    Serial.println("Look for 10");
    fci = pr.FromStringSearch("10");
    if (pr.GetLastError() != JSONConstant::ERR_NONE) {
        Serial.print("Last error: ");
        Serial.print(pr.GetLastError());
        Serial.print(" at character ");
        Serial.println(fci);
        return;
    }else {
        Serial.print("Found at character index ");
        Serial.println(fci);
    }

    Serial.println("Done");
}

void loop() {
}
