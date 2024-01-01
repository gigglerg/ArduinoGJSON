/**
 * \file
 * Embedded JSON builder and parser - Elements
 * PROJECT: GJSON library
 * TARGET SYSTEM: Arduino, STM32, Atmel AT91SAM, portable
 */

#ifndef GJSON_ELEMENTS_H
#define GJSON_ELEMENTS_H

namespace gjson {

/**
 * A class representing a basic JSON in-memory element.  This is a parent class to all possible JSON element types and
 * JSON strings ultimately get parsed into objects based off this class
 *
 * \tparam LENGTH Maximum length (including NULL terminator) in characters of a string attribute/property
 */
template<uint8_t LENGTH>
class JSONElement {
public:
    /**
     * Enum of possible JSON basic element types
     */
    typedef enum {
        NOTSET,
        ARRAY,
        MAP,
        STRING,
    }TYPE;


    /**
     * Constructor, make stable instance
     *
     * \note If parent specified then its next element pointer will be updated with this
     *
     * \param[in] e Pointer, parent element or NULL (no parent yet)
     */
    JSONElement(JSONElement* e=NULL) : e_(NULL) { // parent pointer
        if (e) {
            e->Next(this);
        }
    }


    /**
     * Get next JSON element
     *
     * \return JSON element or NULL
     */
    JSONElement* Next() const {
        return e_;
    }


    /**
     * Set next JSON element
     *
     * \param[in] e Pointer to JSON element
     * \return this
     */
    JSONElement* Next(JSONElement* e) {
        if (e) {
            e_ = e;
        }

        return this;
    }


    /**
     * Get JSON element type
     *
     * \return See \ref TYPE
     */
    TYPE Type() {
        return static_cast<TYPE>(type_);
    }


    /**
     * Set JSON element type
     *
     * \param[in] type An element Type numeric
     * \return See \ref TYPE
     */
    TYPE Type(const TYPE type) {
        type_ = type;

        return static_cast<TYPE>(type_);
    }

protected:
    uint8_t            type_;            ///< Element type
    JSONElement*    e_;                  ///< Next item
}; // class JSONElement


/**
 * A class representing a JSON in-memory string element, templated maximum dimension.  This will be either a attribute/property or value
 *
 * \tparam LENGTH Maximum length (including NULL terminator) in characters of a string attribute/property
 * \tparam T Numeric data type used in conversion to string.  Default uint8_t
 */
template<uint8_t LENGTH, typename T = uint8_t>
class JSONString : public JSONElement<LENGTH> {
public:
    /**
     * Default constructor, make stable instance.  Optionally allows indication of IsValue and parent in-memeory element
     *
     * \note If parent specified then its next element pointer will be updated with this
     *
     * \param[in] is_value IsValue boolean, when true(default) indicates string is a value
     * \param[in] e Pointer, parent element or NULL (no parent yet)
     */
    JSONString(const bool is_value=true, JSONElement<LENGTH>* e=NULL) : is_value_(is_value), length_(0), literal_(false) { // value change.  literal cannot be empty string
        JSONElement<LENGTH>::Type(JSONElement<LENGTH>::STRING);
        string_[0] = '\0';
        if (e) {
            e->Next(this);
        }
    }


    /**
     * Constructor, make stable instance with given NULL terminated string.  Optionally allows indication of IsValue and Isliteral state
     *
     * \attention No checking is performed on literals when isn't a value string.  You've been warned as there are no exceptions!
     *
     * \param[in] s NULL terminated source string to copy
     * \param[in] is_value IsValue boolean, when true(default) indicates string is a value
     * \param[in] is_literal IsLiteral boolean, when true indicates string value is literal, default false
     */
    JSONString(const char* s, const bool is_value=true, const bool is_literal=false) : is_value_(is_value), literal_(is_literal) { // str + value change
        JSONElement<LENGTH>::Type(JSONElement<LENGTH>::STRING);
        Data(s, is_literal);
    }


    /**
     * Constructor, make stable instance with given NULL terminated string, parent element and optionally indication of IsValue and IsLiteral states
     *
     * \attention No checking is performed on literals when isn't a value string.  You've been warned as there are no exceptions!
     *
     * \param[in] s NULL terminated source string to copy
     * \param[in] e Pointer, parent element or NULL (no parent yet)
     * \param[in] is_value IsValue boolean, when true(default) indicates string is a value
     * \param[in] is_literal IsLiteral boolean, when true indicates string value is literal, default false
     */
    JSONString(const char* s, JSONElement<LENGTH>* e, const bool is_value=true, const bool is_literal=false) : is_value_(is_value), literal_(is_literal) { // str + value change + parent pointer
        JSONElement<LENGTH>::Type(JSONElement<LENGTH>::STRING);
        Data(s, is_literal);
        if (e) {
            e->Next(this);
        }
    }


    /**
     * Constructor, make stable instance with given NULL terminated string, string length, parent element and optionally indication of IsValue and IsLiteral states
     *
     * \attention No checking is performed on literals when isn't a value string.  You've been warned as there are no exceptions!
     *
     * \param[in] s NULL terminated source string to copy
     * \param[in] e Pointer, parent element or NULL (no parent yet)
     * \param[in] l NULL terminated source string length (characters), not including NULL
     * \param[in] is_value IsValue boolean, when true(default) indicates string is a value
     * \param[in] is_literal IsLiteral boolean, when true indicates string value is literal, default false
     */
    JSONString(const char* s, uint16_t l, JSONElement<LENGTH>* e, const bool is_value=true, const bool is_literal=false) : is_value_(is_value), literal_(is_literal) { // str + length + value change + parent pointer
        JSONElement<LENGTH>::Type(JSONElement<LENGTH>::STRING);
        Data(s, is_literal, l);
        if (e) {
            e->Next(this);
        }
    }


    /**
     * Constructor, make stable instance with given type and sprint format string.  Optionally allows indication of IsValue and Isliteral state
     *
     * \attention No checking is performed on literals when isn't a value string.  You've been warned as there are no exceptions!
     *
	 * \param[in] d Data to convert
	 * \param[in] format Format string for conversion using sprintf
     * \param[in] is_value IsValue boolean, when true(default) indicates string is a value
     * \param[in] prefer_literal Prefer Literal flag, indicates data string contains a JSON literal value, default true
     */
    JSONString(const T d, const char* format, const bool is_value=true, const bool prefer_literal=true) : is_value_(is_value) { // str + value change
        JSONElement<LENGTH>::Type(JSONElement<LENGTH>::STRING);
        Data<T>(d, format, prefer_literal);
    }


    /**
     * Constructor, make stable instance with given type and sprint format string.  Optionally allows indication of IsValue and Isliteral state
     *
     * \attention No checking is performed on literals when isn't a value string.  You've been warned as there are no exceptions!
     *
	 * \param[in] d Data to convert
	 * \param[in] format Format string for conversion using sprintf
     * \param[in] e Pointer, parent element or NULL (no parent yet)
     * \param[in] is_value IsValue boolean, when true(default) indicates string is a value
     * \param[in] prefer_literal Prefer Literal flag, indicates data string contains a JSON literal value, default true
     */
	JSONString(const T d, const char* format, JSONElement<LENGTH>* e, const bool is_value=true, const bool prefer_literal=true) : is_value_(is_value) { // str + value change + parent pointer
        JSONElement<LENGTH>::Type(JSONElement<LENGTH>::STRING);
        Data<T>(d, format, prefer_literal);
        if (e) {
            e->Next(this);
        }
    }


    /**
     * Query string length
     *
     * \return Character count, not including NULL
     */
    uint16_t Length() const {
        return length_;
    }


    /**
     * Set string length.  Can be used to truncate, will move NULL location as required
     *
     * \param[in] l Length, characters, not including NULL
     * \return Character count, not including NULL
     */
    uint16_t Length(const uint16_t l) {
        if (l < (LENGTH-1)) {
            if (length_ > l) {
                string_[l] = '\0';
                length_ = l;
            }
        }

        return length_;
    }


	/**
	 * Convert integer type to data string
	 *
     * \attention You can only specify literal true or use literal exponents if this JSON string is IsValue=true, i.e. not a property/attribute
	 * \attention Care should be taken that the string buffer size is enough for conversion
	 *
	 * \param[in] d  Data to convert
	 * \parampin] format \ref sprintf format string
     * \param[in] prefer_literal Prefer Literal flag, indicates data string contains a JSON literal value, default true
     * \param[in] allow_exponent Allow use of literal exponent values, default GJSON_DEFAULT_EXPONENTIAL_NOTATION
     * \tparam DT Data type
     * \retval true Data accepted
     * \retval false Data accepted but not literal
	 */
	template<typename DT>
	bool Data(const DT d, const char* format, const bool prefer_literal=true, const bool allow_exponent=GJSON_DEFAULT_EXPONENTIAL_NOTATION) {
        bool r = true;

		length_ = snprintf(string_, LENGTH, format, d);
		if (length_>LENGTH) {
			length_ = 0;
			string_[0] = '\0';
		}
		
		//		char yyy[32];

//			strcpy(string_, "0");
//			length_ = 1;
//		sprintf(yyy, format, d);
//		Serial.println(format);
/*		length_ = strlen(string_);
		if (length_ >= LENGTH) {
			Serial.println("Over");
			strcpy(string_, "0");
			length_ = 1;
		}else {
			memcpy(string_, yyy, length_);
		}*/

        if (is_value_) {
            if (prefer_literal) {
                if (JSONSupport::IsLiteral(string_, length_, allow_exponent)) {
                    literal_ = true;
                }else {
                    r = false;    // Didn't get what you wanted
                }
            }else {
                // Value is quoted string
                literal_ = false;
            }
        }else {
            if (prefer_literal) {
                r = false;    // Didn't get what you wanted
            }
            literal_ = false;
        }

		return r;
	} // Data(...)


    /**
     * Set string data and additional type meta like IsLiteral and allow literal exponents
     *
     * \attention You can only specify literal true or use literal exponents if this JSON string is IsValue=true, i.e. not a property/attribute
     * \note Parser will use this method as main JSON data is handled like a large string
     *
     * \param[in] d Pointer to data string, NULL terminated for copy
     * \param[in] prefer_literal Prefer Literal flag, indicates data string contains a JSON literal value, default false
     * \param[in] length Data string length, characters.  0 find string length internally via strlen(...)
     * \param[in] allow_exponent Allow use of literal exponent values, default GJSON_DEFAULT_EXPONENTIAL_NOTATION
     * \retval true Data accepted
     * \retval false Data accepted but not literal
     */
    bool Data(const char* d, const bool prefer_literal=false, const uint16_t length=0, const bool allow_exponent=GJSON_DEFAULT_EXPONENTIAL_NOTATION) {
        bool r = true;

        if (!length) {
            uint8_t dl = strlen(d);
            uint8_t l = (dl<sizeof(string_) ? dl : (sizeof(string_)-1));

            string_[l] = '\0';
            length_ = l;
            strncpy(string_, d, l);
        }else {
            uint8_t l = (sizeof(string_)<length ? sizeof(string_)-1 : length);

            string_[0] = '\0';
            strncpy(string_, d, l);
            string_[l] = '\0';
            length_ = l;
        }

        if (is_value_) {
            if (prefer_literal) {
                if (JSONSupport::IsLiteral(string_, length_, allow_exponent)) {
                    literal_ = true;
                }else {
                    r = false;    // Didn't get what you wanted
                }
            }else {
                // Value is quoted string
                literal_ = false;
            }
        }else {
            if (prefer_literal) {
                r = false;    // Didn't get what you wanted
            }
            literal_ = false;
        }

        return r;
    } // Data(...)


    /**
     * Get string data
     *
     * \attention To simply design this pointer is to an internally managed string.  Care should be taken not to corrupt
     *
     * \return String pointer
     */
    const char* Data() const {
        return static_cast<const char* const>(string_);
    }


    /**
     * Query is string data a value or property/attribute
     *
     * \retval true Value string
     * \retval false Property/attribute
     */
    bool IsValue() const {
        return is_value_;
    }


    /**
     * Set string data IsValue
     *
     * \param[in] is_value Boolean indicating (true) string data is value otherwise (false) property/attribute
     * \return See \ref IsValue
     */
    bool Value(const bool is_value) {
        is_value_ = is_value;

        return IsValue();
    }


    /**
     * Query string data value, is JSON literal
     *
     * \return Boolean
     */
    bool IsValueLiteral() const {
        return literal_;
    }


    /**
     * Query string value for JSON literal null
     *
     * \return Boolean
     */
    bool IsNull() const {
        bool b = false;

        if (is_value_ && literal_ && JSONSupport::IsNull(string_, length_)) {
            b = true;
        }

        return b;
    }


    /**
     * Set data string to JSON literal null.  IsLiteral will be set when element a value string
     *
     * \attention You can only use this function correctly if your string maximum has enough space for literal string
     */
    void Null() {
        if (sizeof(string_)<5) {
            // Cannot do much in this situation, it does mean words like "true", "false" arn't going to work, really this should be an exception
            Data("0", true);
        }else {
            Data("null", true);
        }
        literal_ = true;
    }


    /**
     * Query string value for JSON literal boolean
     *
     * \return Boolean
     */
    bool IsBoolean() const {
        bool b = false;

        if (is_value_ && literal_ && JSONSupport::IsBoolean(string_, length_)) {
            b = true;
        }

        return b;
    }


    /**
     * Set data string to JSON literal boolean.  IsLiteral will be set when element a value string
     *
     * \attention You can only use this function correctly if your string maximum has enough space for literal string
     */
    void Boolean(const bool v) {
        if (sizeof(string_)<6) {
            // Cannot do much in this situation, it does mean words like "true", "false" arn't going to work, really this should be an exception
            if (v) {
                Data("0", true);
            }else {
                Data("1", true);
            }
        }else {
            if (v) {
                Data("true", true);
            }else {
                Data("false", true);
            }
        }
        literal_ = true;
    }


    /**
     * Query string value is integer with optional leading zero inclusion
     *
     * \param[in] allow_leading_zero Default(false), see \ref JSONSupport::IsInt
     * \return Boolean
     */
    bool IsInt(bool allow_leading_zero=false) const {
        return JSONSupport::IsInt(string_, length_, allow_leading_zero);
    }


    /**
     * Query string value is decimal.  See \ref JSONSupport::IsDecimal
     *
     * \return Boolean
     */
    bool IsDecimal() const {
        return JSONSupport::IsDecimal(string_, length_, GJSON_DEFAULT_EXPONENTIAL_NOTATION);
    }


    /**
     * Query string value is hexadecimal.  See \ref JSONSupport::IsDecimal
     *
     * \return Boolean
     */
    bool IsHex(const bool prefix_0x=true) const {
        return JSONSupport::IsHex(string_, length_, prefix_0x);
    }


    /**
     * Equals comparison operator, check string against given subject (case sensitive)
     *
     * \param[in] rhs JSONString<LENGTH> reference string (subject)
     * \retval true strings match
     * \retval false strings differ
     */
    bool operator==(const JSONString<LENGTH>& rhs) const {
        bool result = false;

        if (Length() == rhs.length()) {
            !JSONSupport::StrNCmp(Data(), rhs.Data(), rhs.Length()) ? result=true: result=false;
        }

        return result;
    }


    /**
     * Not equals comparison operator, check string against given subject (case sensitive)
     *
     * \param[in] rhs JSONString<LENGTH> reference string (subject)
     * \retval true strings differ
     * \retval false strings match
     */
    bool operator!=(const JSONString<LENGTH>& rhs) const {
        return !(*this == rhs);
    }


   /**
     * Equals comparison operator, check string against given subject (case sensitive)
     *
     * \param[in] rhs ASCII subject string (subject)
     * \retval true strings match
     * \retval false strings differ
     */
    bool operator==(const char* rhs) const {
        uint8_t l;
        bool result = false;

        if (rhs && (l=strlen(rhs)) == Length()) {
            !JSONSupport::StrNCmp(Data(), rhs, l) ? result=true: result=false;
        }

        return result;
    }


   /**
     * Not equals comparison operator, check string against given subject (case sensitive)
     *
     * \param[in] rhs ASCII subject string (subject)
     * \retval true strings differ
     * \retval false strings match
     */
    bool operator!=(const char* rhs) const {
        return !(*this == rhs);
    }

protected:
    char            string_[LENGTH];        ///< String
    uint16_t        length_;                ///< String length
    bool            is_value_;              ///< Is value or key
    bool            literal_;               ///< Value is literal
}; // JSONString


/**
 * A class representing a JSON in-memory array element
 *
 * \tparam LENGTH Maximum length (including NULL terminator) in characters of a string attribute/property
 */
template<uint8_t LENGTH>
class JSONArray : public JSONElement<LENGTH> {
public:
    /**
     * Default constructor, make stable instance.  Optionally allows indication of IsOpen and parent in-memeory element
     *
     * \note If parent specified then its next element pointer will be updated with this
     *
     * \param[in] e Pointer, parent element or NULL (no parent yet)
     * \param[in] is_open IsOpen boolean, when true(default) indicates opening (start)
     */
    JSONArray(JSONElement<LENGTH>* e=NULL, const bool is_open=true) : JSONElement<LENGTH>(e), is_open_(is_open) {    // e before is_open because close will always refer to an e instance
        JSONElement<LENGTH>::Type(JSONElement<LENGTH>::ARRAY);
    }


    /**
     * Default constructor, make stable instance.  Optionally allows indication of IsOpen
     *
     * \param[in] is_open IsOpen boolean, when true(default) indicates array opening (start)
     */
    JSONArray(const bool is_open) : JSONElement<LENGTH>(NULL), is_open_(is_open) {
        JSONElement<LENGTH>::Type(JSONElement<LENGTH>::ARRAY);
    }


    /**
     * Query isOpen property of array element
     *
     * \return Boolean
     */
    bool IsOpen() const {
        return is_open_;
    }


    /**
     * Set isOpen property of array element
     *
     * \param[in] is_open IsOpen boolean, when true indicates array opening (start) or closing (end)
     * \return Boolean, see \ref IsOpen
     */
    bool Open(const bool is_open) {
        is_open_ = is_open;

        return IsOpen();
    }

protected:
    bool            is_open_;               ///< Is Open
}; // class JSONArray


/**
 * A class representing a JSON in-memory map element
 *
 * \tparam LENGTH Maximum length (including NULL terminator) in characters of a string attribute/property
 */
template<uint8_t LENGTH>
class JSONMap : public JSONArray<LENGTH> {
public:
    JSONMap(JSONElement<LENGTH>* e=NULL, const bool is_open=true) : JSONArray<LENGTH>(e, is_open) {    // e before isOpen because close will always refer to an e instance
        JSONArray<LENGTH>::Type(JSONElement<LENGTH>::MAP);
    }

    JSONMap(const bool is_open) : JSONArray<LENGTH>(is_open) {
        JSONArray<LENGTH>::Type(JSONElement<LENGTH>::MAP);
    }
}; // class JSONMap

}; // namespace gjson

#endif // GJSON_ELEMENTS_H
