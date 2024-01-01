/**
 * Example use of GJSON (embedded portable JSON library) - In this example a JSON string on stack is parsed by a   
 * custom JSON parser.  No object chain is generated, instead the callback interface is employed as part of 
 * parsing and various strings are output over Arduino debug serial.
 *
 * Expected serial output (Arduino/Genuino Uno):
 * setup()
 * [
 * {
 * Prop(a)
 * [
 * [
 * [
 * Val(b)
 * {
 * Prop(c)
 * Val(d)
 * }
 * ]
 * ]
 * ]
 * }
 * ]
 * Done
 * 
 * DG, 2019
 */

// Include
#include "gjson.h"

using namespace gjson;

/*! \cond PRIVATE */

#if defined(GJSON_PACKING)
#pragma pack(push, GJSON_PACKING)
#endif


/*
 * JSON non-recursive simple string parser by subclass example using no buffering.  Parse user supplied string with callbacks
 * where user can dictate what is done with parsed data, here it is simply written to Arduino debug Serial and there is no 
 * I/O because in this example no string is created + the user JSON string is passed by parameter.
 *
 * \tparam LENGTH Maximum length (including NULL terminator) in characters of a string attribute/property
 */
template<uint8_t LENGTH>
class JSONCustomStringParse : protected JSONParse<LENGTH, 0> {
public:
    using JSONParse<LENGTH, 0>::ERR;
    using JSONParse<LENGTH, 0>::GetLastError;
    using JSONParse<LENGTH, 0>::LimitNodeLevel;


    /*
     * Default constructor, make stable instance.  Setup NULL I/O, base parser and enable callbacks
     */
    JSONCustomStringParse() : jio_(), JSONParse<LENGTH, 0>(&jio_, 0U) {
        JSONParse<LENGTH, 0>::Callbacks(true);
    }


    /*
     * JSON Parse user supplied string
     *
     * \attention Caller should check /ref GetLastError upon completion for any error
     * condition that may have risen during parse
     *
     * \note If JSON string part way through then node alignments will not be met, i.e.
     * there maybe more Array or Map closes than opens.  In this situation caller should 
     * disable via /ref LimitNodeLevel
     *
     * param[in] s JSON NULL terminated string
     * \return Last character index parsed.  Debug aid upon error conditions
     */
    uint16_t FromString(char *s) {
        return JSONParse<LENGTH, 0>::FromString(s, NULL);
    }


    /*
     * See JSONTokenizer::TokenArray.  Here we simply output strings over Arduino UART
     */
    bool TokenArray(const JSONArray<LENGTH>* e, char *s=NULL, uint16_t i=0) {
        // String parsing?
        if (JSONConstant::OP_FROM_STRING == JSONParse<LENGTH, 0>::GetLastOperation()) {
            if (e->IsOpen()) {
                Serial.println("[");
            }else {
                Serial.println("]");
            }
        }

        return false;
    }


    /*
     * \copydoc JSONTokenizer::TokenMap.  Here we simply output strings over Arduino UART
     */
    bool TokenMap(const JSONArray<LENGTH>* e, char *s=NULL, uint16_t i=0) {
        // String parsing?
        if (JSONConstant::OP_FROM_STRING == JSONParse<LENGTH, 0>::GetLastOperation()) {
            if (e->IsOpen()) {
                Serial.println("{");
            }else {
                Serial.println("}");
            }
        }

        return false;
    }


    /*
     * \copydoc JSONTokenizer::TokenString.  Here we simply output strings over Arduino UART
     */
    bool TokenString(const JSONString<LENGTH>* e, char *s=NULL, int16_t si=0, int16_t ei=0) {
        // String parsing?
        if (JSONConstant::OP_FROM_STRING == JSONParse<LENGTH, 0>::GetLastOperation()) {
            if (e->IsValue()) {
                if (e->IsValueLiteral()) {
                    Serial.print("LVal(");
                    Serial.print(e->Data());
                    Serial.println(")");
                }else {
                    Serial.print("Val(");
                    Serial.print(e->Data());
                    Serial.println(")");
                }
            }else {
                Serial.print("Prop(");
                Serial.print(e->Data());
                Serial.println(")");
            }
        }

        return false;
    }

protected:
    NullBuffer    jio_;
}; // class JSONCustomStringParse


#if defined(GJSON_PACKING)
#pragma pack(pop)
#endif

/*! \endcond */


void setup() {
    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    char js[] = "[ { \"a\":[ [ [ \"b\", { \"c\":\"d\" } ] ] ] } ]";
    
    // Expected parsed output would be something like:
    // Array(o) >
    // Map(o) >
    // String("a") > Array(o) >
    // Array(o) >
    // Array(o) >
    // String("b") >
    // Map(o) >
    // String("c") > String("d") >
    // Map(c)
    // Array(c)
    // Array(c)
    // Array("a", c)
    // Map(c)
    // Array(c)

    JSONCustomStringParse<5> cpr;
    uint16_t fci = cpr.FromString(js);
    if (cpr.GetLastError() != JSONConstant::ERR_NONE) {
        Serial.print("Last error: ");
        Serial.print(cpr.GetLastError());
        Serial.print(" at character ");
        Serial.println(fci);
        return;
    }
    Serial.println("Done");
}

void loop() {
}