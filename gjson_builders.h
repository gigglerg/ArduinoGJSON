/**
 * \file
 * Embedded JSON builder and parser - Builders
 * PROJECT: GJSON library
 * TARGET SYSTEM: Arduino, STM32, Atmel AT91SAM, portable
 */

#ifndef GJSON_BUILDERS_H
#define GJSON_BUILDERS_H

namespace gjson {

/**
 * A builder, helper class representing a JSON tuple (Map) between 2 strings a property/attribute and its assoicated value.  Parser will not use this type of element in-memory.
 *
 * This class inherits from JSON string for the property value and contains another string for the property/attribute name.  It was important to keep this layout as
 * \ref JSONElement::Next isn't virtual.
 *
 * \tparam LENGTH Maximum length (including NULL terminator) in characters of a string attribute/property
 * \tparam T Numeric data type used in conversion to string.  Default uint8_t
 */
template<uint8_t LENGTH, typename T = uint8_t>
class JSONMapTuple : public JSONString<LENGTH, T> {
public:
    /**
     * Constructor, make stable instance.  Specify a tuple with name and value strings + optional parent element
     *
     * \param[in] p Pointer attribute/property NULL terminated string
     * \param[in] v Pointer value NULL terminated string
     * \param[in] e Pointer, parent element or NULL (no parent yet)
     * \param[in] is_literal IsLiteral boolean, when true indicates string value is literal, default false
     */
    JSONMapTuple(const char* p, const char* v, JSONElement<LENGTH>* e=NULL, const bool is_literal=false) : JSONString<LENGTH>(v, true, is_literal), p_(JSONString<LENGTH>(p, e, false)) {    // p String : v String
        p_.Next(this);
    }


    /**
     * Constructor, make stable instance.  Specify a tuple with name and value numeric + optional parent element
     *
     * \param[in] p Pointer attribute/property NULL terminated string
	 * \param[in] v Value data to convert
	 * \param[in] format Format string for conversion using sprintf
     * \param[in] e Pointer, parent element or NULL (no parent yet)
     * \param[in] prefer_literal Prefer Literal flag, indicates data string contains a JSON literal value, default true
     */
    JSONMapTuple(const char* p, const T v, const char* format, JSONElement<LENGTH>* e=NULL, const bool prefer_literal=true) : JSONString<LENGTH,T>(v, format, true, prefer_literal), p_(JSONString<LENGTH>(p, e, false)) {    // p String : v String
        p_.Next(this);
    }

protected:
    JSONString<LENGTH>            p_;       ///< Attribute/property item
}; // class JSONMapTuple


/**
 * A builder, helper class representing a JSON tuple (Map) between a string property/attribute and another Map.  Parser will not use this type of element in-memory.
 *
 * This class inherits from JSON map for the property value and contains another string for the property/attribute name.  It was important to keep this layout as
 * \ref JSONElement::Next() isn't virtual.
 *
 * \tparam LENGTH Maximum length (including NULL terminator) in characters of a string attribute/property
 */
template<uint8_t LENGTH>
class JSONMapTupleMap : public JSONMap<LENGTH> {
public:
    /**
     * Constructor, make stable instance.  Specify a tuple with name and optional parent element
     *
     * \param[in] p Pointer attribute/property NULL terminated string
     * \param[in] e Pointer, parent element or NULL (no parent yet)
     */
    JSONMapTupleMap(const char* p, JSONElement<LENGTH>* e=NULL) : JSONMap<LENGTH>(), p_(JSONString<LENGTH>(p, e, false)) {        // p String : v Map(o)
        p_.Next(this);
    }

protected:
    JSONString<LENGTH>            p_;       ///< Attribute/property item
}; // class JSONMapTupleMap


/**
 * A builder, helper class representing a JSON tuple (Map) between a string property/attribute and another Array.  Parser will not use this type of element in-memory.
 *
 * This class inherits from JSON array for the property value and contains another string for the property/attribute name.  It was important to keep this layout as
 * \ref JSONElement::Next() isn't virtual.
 *
 * \tparam LENGTH Maximum length (including NULL terminator) in characters of a string attribute/property
 */
template<uint8_t LENGTH>
class JSONMapTupleArray : public JSONArray<LENGTH> {
public:
    /**
     * Constructor, make stable instance.  Specify a tuple with name and optional parent element
     *
     * \param[in] p Pointer attribute/property NULL terminated string
     * \param[in] e Pointer, parent element or NULL (no parent yet)
     */
    JSONMapTupleArray(const char* p, JSONElement<LENGTH>* e=NULL) : JSONArray<LENGTH>(), p_(JSONString<LENGTH>(p, e, false)) {        // p String : v Array(o)
        p_.Next(this);
    }

protected:
    JSONString<LENGTH>            p_;       ///< Attribute/property item
}; // class JSONMapTupleArray

}; // namespace gjson

#endif // GJSON_BUILDERS_H
