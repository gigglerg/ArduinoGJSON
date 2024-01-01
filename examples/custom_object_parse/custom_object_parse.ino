/**
 * Example use of GJSON (embedded portable JSON library) - In this example an object chain is created on stack which  
 * is used to by a custom JSON parser.  JSON builder classes are employed as helpers for attributes/properties.
 * No I/O is allocated because the custom parser in this example doesn't create a JSON string, instead the callback 
 * interface is employed as part of parsing and various strings are output over Arduino debug serial.
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

/*! \cond PRIVATE */

#if defined(GJSON_PACKING)
#pragma pack(push, GJSON_PACKING)
#endif


/*
 * JSON non-recursive simple object parser by subclass example.  Parse user supplied object chain with callbacks
 * where user can dictate what is done with parsed data, here it is simply written to Arduino debug Serial and 
 * there is no I/O because in this example no string is created.
 *
 * \tparam LENGTH Maximum length (including NULL terminator) in characters of a string attribute/property
 */
template<uint8_t LENGTH>
class JSONCustomObjectParse : protected gjson::JSONParse<LENGTH, 0> {
public:
    using gjson::JSONParse<LENGTH, 0>::ERR;
    using gjson::JSONParse<LENGTH, 0>::GetLastError;
    using gjson::JSONParse<LENGTH, 0>::LimitNodeLevel;


    /*
     * Default constructor, make stable instance.  Setup NULL I/O, base parser and enable callbacks
     */
    JSONCustomObjectParse() : jio_(), gjson::JSONParse<LENGTH, 0>(&jio_, 0U) {
        gjson::JSONParse<LENGTH, 0>::Callbacks(true);
    }


    /*
     * Parse given JSON object chain, parsed JSON objects will invoke the callback interface
     *
     * \note No JSON is built because I/O buffer doesn't exist (NULL)
     *
     * \note If object chain not start then node alignments will not be met, i.e.
     * there maybe more Array or Map closes than opens.  In this situation caller should 
     * disable via /ref LimitNodeLevel
     *
     * \param[in] e Pointer to first JSON element in object chain
     * \retval true Success
     * \retval false Failure, check /ref GetLastError
     */
    bool FromObject(gjson::JSONElement<LENGTH>* e) {
        return gjson::JSONParse<LENGTH, 0>::FromObject(e);
    }


    /*
     * See JSONTokenizer::TokenArray.  Here we simply output strings over Arduino UART
     */
    bool TokenArray(const gjson::JSONArray<LENGTH>* e, char *s=NULL, uint16_t i=0) {
        // String parsing?
        if (gjson::JSONConstant::OP_FROM_OBJECT == gjson::JSONParse<LENGTH, 0>::GetLastOperation()) {
            if (e->IsOpen()) {
                Serial.println("[");
            }else {
                Serial.println("]");
            }
        }

        return false; // We don't want parsing to stop
    }


    /*
     * \copydoc JSONTokenizer::TokenMap.  Here we simply output strings over Arduino UART
     */
    bool TokenMap(const gjson::JSONArray<LENGTH>* e, char *s=NULL, uint16_t i=0) {
        // String parsing?
        if (gjson::JSONConstant::OP_FROM_OBJECT == gjson::JSONParse<LENGTH, 0>::GetLastOperation()) {
            if (e->IsOpen()) {
                Serial.println("{");
            }else {
                Serial.println("}");
            }
        }

        return false; // We don't want parsing to stop
    }


    /*
     * \copydoc JSONTokenizer::TokenString.  Here we simply output strings over Arduino UART
     */
    bool TokenString(const gjson::JSONString<LENGTH>* e, char *s=NULL, int16_t si=0, int16_t ei=0) {
        // String parsing?
        if (gjson::JSONConstant::OP_FROM_OBJECT == gjson::JSONParse<LENGTH, 0>::GetLastOperation()) {
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

        return false; // We don't want parsing to stop
    }

protected:
    gjson::NullBuffer    jio_;
}; // class JSONCustomObjectParse


#if defined(GJSON_PACKING)
#pragma pack(pop)
#endif

/*! \endcond */

void setup() {
    Serial.begin(9600);
    while(!Serial) { }

    Serial.println("setup()");

    // Expect something like: [ { "a":[ [ [ "b", { "c":"d" } ] ] ] } ]
    auto a = gjson::JSONArray<5>();                              // Array(o) >
    auto b = gjson::JSONMap<5>(&a);                              // Map(o) >
    auto c = gjson::JSONMapTupleArray<5>("a", &b);               // String("a") > Array(o) >
    auto d = gjson::JSONArray<5>(&c);                            // Array(o) >
    auto e = gjson::JSONArray<5>(&d);                            // Array(o) >
    auto f = gjson::JSONString<5>("b", &e);                      // String("b") >
    auto g = gjson::JSONMap<5>(&f);                              // Map(o) >
    auto h = gjson::JSONMapTuple<5>("c", "d", &g);               // String("c") > String("d") >
    auto i = gjson::JSONMap<5>(&h,false);                        // Map(c)
    auto j = gjson::JSONArray<5>(&i,false);                      // Array(c)
    auto k = gjson::JSONArray<5>(&j,false);                      // Array(c)
    auto l = gjson::JSONArray<5>(&k,false);                      // Array("a", c)
    auto m = gjson::JSONMap<5>(&l,false);                        // Map(c)
    auto n = gjson::JSONArray<5>(&m,false);                      // Array(c)

    JSONCustomObjectParse<5> cpr;
    bool err = cpr.FromObject(&a);
    if (!err) {
        Serial.print("Last error: ");
        Serial.println(cpr.GetLastError());
        return;
    }
    Serial.println("Done");
}

void loop() {
}