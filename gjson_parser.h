/**
 * \file
 * Embedded JSON builder and parser - Parser
 * PROJECT: GJSON library
 * TARGET SYSTEM: Arduino, STM32, Atmel AT91SAM, portable
 */

#ifndef GJSON_PARSER_H
#define GJSON_PARSER_H

namespace gjson {

/**
 * A base structure containing JSON parser type and enum information
 */
struct JSONConstant {
    /**
     * Enum of possible interface related errors
     */
    typedef enum {
        ERR_NONE,                       /// No error
        ERR_UNEXPECTED_END_OF_ARRAY,    /// Parsing or building met unexpected end of an array
        ERR_UNEXPECTED_END_OF_MAP,      /// Parsing or building met unexpected end of a map
        ERR_BAD_ATTRIBUTE,              /// Parsing or building had trouble with a Map attribute/property
        ERR_BAD_VALUE,                  /// Parsing or building had trouble with a Map value
        ERR_UNTERMINATED_STRING,        /// Parsing or building had trouble with an unterminated string
		ERR_UNEXPECTED_DATA,            /// Parsing or building unexpected data
        ERR_NOT_FOUND,                  /// Parsing or searching, expected element not found
        ERR_OBJECT_MEMORY_LIMIT,        /// Building, object memory limit reached
        ERR_CHAR_MEMORY_LIMIT,          /// Building, character (I/O) memory limit reached
        ERR_CALLBACK_STOP,              /// Parsing or building, a callback stop condition notified
    }ERR;


    /**
     * Enum of possible JSON operation, debug aid
     */
    typedef enum {
        OP_NONE,                        /// Operation none
        OP_FROM_STRING,                 /// Operation JSON from string
        OP_FROM_OBJECT,                 /// Operation JSON from object
        OP_FIND_OBJECT,                 /// Operation find JSON from object
        OP_RELEASE_OBJECT,              /// Operation release object
    }OP;
}; // struct JSONConstant


/**
 * JSON base parser tokenizer handler abstract class.  Implement methods for callbacks on parsed items both string or object
 *
 * \tparam LENGTH Maximum length (including NULL terminator) in characters of a string attribute/property
 */
template<uint8_t LENGTH>
class JSONTokenizer {
public:
    /**
     * Callback for JSON array element handling.  Invoked from Parsing JSON strings,
     * objects and when searching.
     *
     * The return value mean is dependant on current operation, \ref JSONParse::GetLastOperation
     * If OP_FROM_STRING
     *        true -> stop parsing
     * If OP_FROM_OBJECT
     *        true -> does nothing
     * If OP_FIND_OBJECT
     *        true -> take object, found
     *
     * When implementing functionality in a subclass, setting an error condition 
     * will also terminate current operation, see last_error_ and \ref JSONParse::GetLastError
     *
     * \note You may update source string but operation on data should be terminated if 
     * you are likely to affect output with change.
     * \note Do not release element instance
     *
     * \param[in] e Array element pointer.  Updates can be made to element so long 
     * as the symantics of JSON are not broken.
     * \param[in] s Source JSON string pointer
     * \param[in] i Current parse character index into source JSON string
     * \retval true Operation dependant meaning, see above
     * \retval false Continue operation
     */
    virtual bool TokenArray(const JSONArray<LENGTH>* e, char *s=NULL, uint16_t i=0) = 0;


    /**
     * Callback for JSON map element handling.  Invoked from Parsing JSON strings,
     * objects and when searching.
     *
     * The return value mean is dependant on current operation, \ref JSONParse::GetLastOperation
     * If OP_FROM_STRING
     *        true -> stop parsing
     * If OP_FROM_OBJECT
     *        true -> does nothing
     * If OP_FIND_OBJECT
     *        true -> take object, found
     *
     * When implementing functionality in a subclass, setting an error condition 
     * will also terminate current operation, see last_error_ and \ref JSONParse::GetLastError
     *
     * \note You may update source string but operation on data should be terminated if 
     * you are likely to affect output with change.
     * \note Do not release element instance
     *
     * \param[in] e Array element pointer.  Updates can be made to element so long 
     * as the symantics of JSON are not broken.
     * \param[in] s Source JSON string pointer
     * \param[in] i Current parse character index into source JSON string
     * \retval true Operation dependant meaning, see above
     * \retval false Continue operation
     */
    virtual bool TokenMap(const JSONArray<LENGTH>* e, char *s=NULL, uint16_t i=0) = 0;


    /**
     * Callback for JSON string element handling.  A string could be a property/attribute 
     * or value.  Invoked from Parsing JSON strings, objects and when searching.
     *
     * The return value mean is dependant on current operation, \ref JSONParse::GetLastOperation
     * If OP_FROM_STRING
     *        true -> stop parsing
     * If OP_FROM_OBJECT
     *        true -> does nothing
     * If OP_FIND_OBJECT
     *        true -> take object, found
     *
     * When implementing functionality in a subclass, setting an error condition 
     * will also terminate current operation, see last_error_ and \ref JSONParse::GetLastError.
     *
     * \note You may update source string but operation on data should be terminated if 
     * you are likely to affect output with change.
     * \note Do not release element instance
     *
     * \param[in] e Array element pointer.  Updates can be made to element so long 
     * as the symantics of JSON are not broken.
     * \param[in] s Source JSON string pointer
     * \param[in] si Start character index into source JSON string.  -1 not setup
     * \param[in] ei End character index into source JSON string.  -1 not setup
     * \retval true Operation dependant meaning, see above
     * \retval false Continue operation
     */
    virtual bool TokenString(const JSONString<LENGTH>* e, char *s=NULL, int16_t si=0, int16_t ei=0) = 0;
}; // class JSONTokenizer


/**
 * JSON base non-recursive parser offering string parsing using user supplied I/O.  Features include parsing string with tokenised callbacks for basic JSON elements,
 * object chain creation, searching objects, moving around object chain and converting objects back into JSON.
 *
 * Implementation is pretty skinny to keep footprint down to meet where possible, memory requirements of a resource limited micro controller.
 *
 * \tparam LENGTH Maximum length (including NULL terminator) in characters of a string attribute/property
 */
template<uint8_t LENGTH>
class JSONParse : public JSONConstant, public JSONTokenizer<LENGTH> {
public:
    /**
     * Constructor, make stable instance with I/O and object memory limit specifiers
     *
     * \param[in,out] io Pointer to I/O instance
     * \param[in] memory_limit Size (Bytes) of object memory if any.  Defaults to 256 Bytes
     */
    JSONParse(IO* io, uint16_t memory_limit=256) : binary_size_(0), io_(io), last_error_(ERR_NONE), \
                                operation_(OP_NONE), like_(NULL), limit_node_level_(true), callbacks_(false), \
                                case_sensitive_(true), last_e_(NULL), limit_memory_(memory_limit) { }


    virtual bool TokenArray(const JSONArray<LENGTH>* e, char *s=NULL, uint16_t i=0) {
        return false;
    }


    virtual bool TokenMap(const JSONArray<LENGTH>* e, char *s=NULL, uint16_t i=0) {
        return false;
    }


    virtual bool TokenString(const JSONString<LENGTH>* e, char *s=NULL, int16_t si=0, int16_t ei=0) {
        return false;
    }


    /**
     * Parse JSON string stored in I/O object, optionally build JSON object chain
     *
     * \attention Caller should check \ref GetLastError upon completion for any error
     * condition that may have risen during parse
     *
     * \attention Make sure I/O contains a JSON string prior to invoke
     *
     * \attention Caller should release any object chain via \ref Release
     *
     * \note If JSON string part way through then node alignments will not be met, i.e.
     * there maybe more Array or Map closes than opens.  In this situation caller should 
     * disable via \ref LimitNodeLevel
     *
     * \note If JSON object chain to large, consider increasing via \ref MemoryLimit
     *
     * \note After use, caller can assess what would be the object chain memory footprint via
     * \ref GetBinarySize.  This is calculated even if no objects are generated
     *
     * \todo Refactor object chain elements to release themself
     *
     * \param[in] psi Parse start character index, default 0
     * \param[in] h Pointer to pointer of JSON element to take object chain updates.  Default 
     * is NULL (do not create object chain, just parse).
     * \return Last character index parsed.  Debug aid upon error conditions
     */
    uint16_t FromString(const uint16_t psi=0, JSONElement<LENGTH>** h=NULL) {
        uint16_t ci = Length();

        // Anything?
        if (ci && psi<ci) {
            ci = FromString(const_cast<char*>(io_->Get()), h, psi);
        }

        return ci;
    }


    /**
     * Parse given JSON string, optionally build JSON object chain
     *
     * \attention Caller should check \ref GetLastError upon completion for any error
     * condition that may have risen during parse
     *
     * \attention Caller should release any object chain via \ref Release
     *
     * \note If JSON string part way through then node alignments will not be met, i.e.
     * there maybe more Array or Map closes than opens.  In this situation caller should 
     * disable via \ref LimitNodeLevel
     *
     * \note If JSON object chain to large, consider increasing via \ref MemoryLimit
     *
     * \note After use, caller can assess what would be the object chain memory footprint via
     * \ref GetBinarySize.  This is calculated even if no objects are generated
     *
     * \todo Refactor object chain elements to release themself
     *
     * \param[in] s JSON source string, NULL terminated
     * \param[in] h Pointer to pointer of JSON element to take object chain updates.  Default 
     * is NULL (do not create object chain, just parse)
     * \param[in] psi Parse start character index, default 0
     * \return Last character index parsed.  Debug aid upon error conditions
     */
    uint16_t FromString(char *s, JSONElement<LENGTH>** h=NULL, uint16_t psi=0) {
        uint8_t    is_literal = 1; // Could be quoted or literal
        uint16_t l = strlen(s), i;
        int16_t ac = 0, mc = 0;
        int16_t str_start = -1, str_end = -1;
        JSONElement<LENGTH>* e = NULL;
        bool expect_value = false;
        bool stop = false;
        bool quoted_processing = false;
        bool escape = false;

        last_e_ = NULL;
        like_ = NULL;
        last_error_ = ERR_NONE;
        operation_ = OP_FROM_STRING;
        binary_size_ = 0;
        for(i=psi; i<l; i++) {
            if (quoted_processing) { // Quoted string
                if (!escape) {
                    if ('\\' == s[i] && !escape) {
                        escape = true;
                    }else if ('\"' == s[i]) {
                        // Data enclose
                        if ((-1 == str_start) || (-1 != str_end)) {
                            if (expect_value) {
                                last_error_ = ERR_BAD_VALUE;
                            }else {
                                last_error_ = ERR_BAD_ATTRIBUTE;
                            }
                        }else if (-1 == str_end) {
                            // String between str_start and i
                            str_end = i;
                            quoted_processing = false;
                        }
                    }
                }else {
                    escape = false;
                }
            }
            
            if (ERR_NONE == last_error_ && !quoted_processing) {
                char cc = s[i];

                // Assume undetermined
                switch(cc) {
                    case '[' : {    // Array open
                        if (!h || limit_memory_ >= (binary_size_ + sizeof(JSONArray<LENGTH>))) {
                            auto a = GJSON_NEW_OBJECT( JSONArray<LENGTH>(e), JSONArray<LENGTH> );

                            // No objects wanted, release
                            if (!h && last_e_) {
                                GJSON_RELEASE_OBJECT( last_e_ );
                            }
                            last_e_ = e;
                            e = a;
                            if (callbacks_) {
                                stop = TokenArray(a, s, i);
                            }
                            ac++;
                            binary_size_ += sizeof(JSONArray<LENGTH>);
                        }else {
                            last_error_ = ERR_OBJECT_MEMORY_LIMIT;
                        }
                    }
                    break;

                    case ']' : {    // Array close
                        uint16_t bsz = binary_size_ + ((-1 < str_start) ? sizeof(JSONString<LENGTH>) + sizeof(JSONArray<LENGTH>) : sizeof(JSONArray<LENGTH>));

                        if (!h || limit_memory_ >= bsz) {
                            if (-1 < str_start) {
                                // Quoted string value.  We arn't using constructor because we want to know of there was an error setting data and we arn't using Exceptions
                                auto a = GJSON_NEW_OBJECT( JSONString<LENGTH>(true, e), JSONString<LENGTH> );

                                if (!a->Data(&s[str_start], ((2 == is_literal)?true:false), static_cast<uint8_t>(str_end - str_start))) {
                                    last_error_ = ERR_BAD_VALUE;
                                }else {
                                    is_literal = 1; // Undetermined as yet
                                    expect_value = false;
                                }

                                // No objects wanted, release
                                if (!h && last_e_) {
                                    GJSON_RELEASE_OBJECT( last_e_ );
                                }
                                last_e_ = e;
                                e = a;
                                if (callbacks_) {
                                    stop = TokenString(a, s, str_start, str_end);
                                    if (stop) {
                                        i = str_start-1;    // -1 to include opening quote
                                    }
                                }
                                str_start = str_end = -1;
                                binary_size_ += sizeof(JSONString<LENGTH>);
                            }
                            auto a = GJSON_NEW_OBJECT( JSONArray<LENGTH>(e, false), JSONArray<LENGTH> );

                            // No objects wanted, release
                            if (!h && last_e_) {
                                GJSON_RELEASE_OBJECT( last_e_ );
                            }
                            last_e_ = e;
                            e = a;
                            if (!stop && callbacks_) {
                                stop = TokenArray(a, s, i);
                            }
                            ac--;
                            binary_size_ += sizeof(JSONArray<LENGTH>);
                        }else {
                            last_error_ = ERR_OBJECT_MEMORY_LIMIT;
                        }
                    }
                    break;

                    case '{' : {    // Map open
                        if (!h || limit_memory_ >= (binary_size_ + sizeof(JSONMap<LENGTH>))) {
                            auto a = GJSON_NEW_OBJECT( JSONMap<LENGTH>(e), JSONMap<LENGTH> );

                            // No objects wanted, release
                            if (!h && last_e_) {
                                GJSON_RELEASE_OBJECT( last_e_ );
                            }
                            last_e_ = e;
                            e = a;
                            if (callbacks_) {
                                stop = TokenMap(a, s, i);
                            }
                            mc++;
                            binary_size_ += sizeof(JSONMap<LENGTH>);
                        }else {
                            last_error_ = ERR_OBJECT_MEMORY_LIMIT;
                        }
                    }
                    break;

                    case '}' : {    // Map close
                        uint16_t bsz = binary_size_ + ((-1 < str_start) ? sizeof(JSONString<LENGTH>) + sizeof(JSONMap<LENGTH>) : sizeof(JSONMap<LENGTH>));

                        if (!h || limit_memory_ >= bsz) {
                            if (-1 < str_start) {
                                // Quoted string value.  We arn't using constructor because we want to know of there was an error setting data and we arn't using Exceptions
                                auto a = GJSON_NEW_OBJECT( JSONString<LENGTH>(true, e), JSONString<LENGTH> );

                                if (!a->Data(&s[str_start], ((2 == is_literal)?true:false), static_cast<uint8_t>(str_end - str_start))) {
                                    last_error_ = ERR_BAD_VALUE;
                                }else {
                                    is_literal = 1; // Undetermined as yet
                                    expect_value = false;
                                }

                                // No objects wanted, release
                                if (!h && last_e_) {
                                    GJSON_RELEASE_OBJECT( last_e_ );
                                }
                                last_e_ = e;
                                e = a;
                                if (callbacks_) {
                                    stop = TokenString(a, s, str_start, str_end);
                                    if (stop) {
                                        i = str_start-1;    // -1 to include opening quote
                                    }
                                }
                                str_start = str_end = -1;
                                binary_size_ += sizeof(JSONString<LENGTH>);
                            }
                            auto a = GJSON_NEW_OBJECT( JSONMap<LENGTH>(e, false), JSONMap<LENGTH> );

                            // No objects wanted, release
                            if (!h && last_e_) {
                                GJSON_RELEASE_OBJECT( last_e_ );
                            }
                            last_e_ = e;
                            e = a;
                            if (!stop && callbacks_) {
                                stop = TokenMap(a, s, i);
                            }
                            mc--;
                            binary_size_ += sizeof(JSONMap<LENGTH>);
                        }else {
                            last_error_ = ERR_OBJECT_MEMORY_LIMIT;
                        }
                    }
                    break;

                    case '\"' :    // Data enclose
                        // Were we already sampling a literal?
                        if (is_literal == 2) {
                            if (expect_value) {
                                last_error_ = ERR_BAD_VALUE;
                                break;
                            }else {
                                last_error_ = ERR_BAD_ATTRIBUTE;
                                break;
                            }
                        }
                        if (-1 == str_start) {
                            str_start = i + 1;
                            is_literal = 0;        // Quoted string
                            quoted_processing = true;
                        }
                    break;

                    case ':' : {    // Attribute/Property
                        if (!h || limit_memory_ >= (binary_size_ + sizeof(JSONString<LENGTH>))) {
                            if (is_literal || (-1 == str_start) || (-1 == str_end)) {
                                last_error_ = ERR_BAD_ATTRIBUTE;
                            }else {
                                // Attribute/Property is always quoted
                                auto a = GJSON_NEW_OBJECT( JSONString<LENGTH>(&s[str_start], static_cast<uint8_t>(str_end - str_start), e, false), JSONString<LENGTH>);

                                // No objects wanted, release
                                if (!h && last_e_) {
                                    GJSON_RELEASE_OBJECT( last_e_ );
                                }
                                last_e_ = e;
                                e = a;
                                if (callbacks_) {
                                    stop = TokenString(a, s, str_start, str_end);
                                    if (stop) {
                                        i = str_start-1;
                                    }
                                }
                                expect_value = true;
                                is_literal = 1;    // Could be literal OR quoted string, not sure yet
                                str_start = str_end = -1;
                                binary_size_ += sizeof(JSONString<LENGTH>);
                            }
                        }else {
                            last_error_ = ERR_OBJECT_MEMORY_LIMIT;
                        }
                    }
                    break;

                    case ',' : {    // Value
                        // Situation where a map or array ends the last string would have be dealt with and this comma is after the container so no data
                        if (-1 < str_start) {
                            if (!h || limit_memory_ >= (binary_size_ + sizeof(JSONString<LENGTH>))) {
                                if (is_literal==1 || (-1 == str_start) || (-1 == str_end)) {
                                    last_error_ = ERR_BAD_VALUE;
                                }else {
                                    // Kind of bug in the way literals are checked in main character switch(...), in quoted string handling end is current i which is ending quote
                                    // but for literals without quotes the end is actually the end character so we have to move it on 1.

                                    // String value (maybe quoted).  We arn't using constructor because we want to know of there was an error setting data and we arn't using Exceptions
                                    auto a = GJSON_NEW_OBJECT( JSONString<LENGTH>(true, e), JSONString<LENGTH> );

                                    if (!a->Data(&s[str_start], ((2 == is_literal)?true:false), static_cast<uint8_t>(str_end - str_start))) {
                                        last_error_ = ERR_BAD_VALUE;
                                    }

                                    is_literal = 1; // Undetermined

                                    // No objects wanted, release
                                    if (!h && last_e_) {
                                        GJSON_RELEASE_OBJECT( last_e_ );
                                    }
                                    last_e_ = e;
                                    e = a;
                                    if (callbacks_) {
                                        stop = TokenString(a, s, str_start, str_end);
                                        if (stop) {
                                            i = str_start;
                                        }
                                        if (!a->IsValueLiteral()) {
                                            i--; // Include opening quote
                                        }
                                    }
                                    str_start = str_end = -1;
                                    binary_size_ += sizeof(JSONString<LENGTH>);
                                }
                            }else {
                                last_error_ = ERR_OBJECT_MEMORY_LIMIT;
                            }
                        }
                    }
                    break;
                    default:
		                if (mc==0 && ac==0) {
							last_error_ = ERR_UNEXPECTED_DATA;
						}else {
							if ((1 <= is_literal) /*&& expect_value*/) {
								if (-1 == str_start) {
									if (s[i] == '\t' || s[i] == ' ') {
										break;    // Ignore white space
									}else {
										if (IsLiteralCharater(s[i])) {
											is_literal = 2;    // We are sampling a literal, ignore quoted string processing
											str_start = i;
											str_end = i + 1;    // Literals cannot be 0 length
										}else {
											last_error_ = ERR_BAD_VALUE;
										}
									}
								}else {
									// End of literal?
									if (s[i] == '\t' || s[i] == ' ') {
									}else {
										// Literal continues
										if (IsLiteralCharater(s[i])) {
											str_end = i + 1;    // Literals cannot be 0 length
										}else {
											last_error_ = ERR_BAD_VALUE;
										}
									}
								}
							}
						}
                } // switch(s[i])
            } // if (ERR_NONE == last_error_ && !quoted_processing)

            if (h && e && !*h) {
                *h = e;
            }

            // A callback has handled something and wishes to complete
            if (ERR_NONE == last_error_ && stop) {
                last_error_ = ERR_CALLBACK_STOP;
            }
            if (limit_node_level_) {
                // Relative open/close sanity check
                if (ac<0) {
                    last_error_ = ERR_UNEXPECTED_END_OF_ARRAY;
                }
                if (mc<0) {
                    last_error_ = ERR_UNEXPECTED_END_OF_MAP;
                }
                // Have we reached the base of where we started parsing?
                if (mc==0 && ac==0) {
                    break;
                }
            }
            if (ERR_NONE != last_error_) {
                break;
            }
        } // for(i=0; ...)
        if (!h) {
            // No objects wanted, release
            if (last_e_) {
                GJSON_RELEASE_OBJECT( last_e_ );
            }
            if (e) {
                GJSON_RELEASE_OBJECT( e );
            }
        }
        if (!last_error_ && (-1 != str_start) && (-1 == str_end)) {
            last_error_ = ERR_UNTERMINATED_STRING;
        }
        if (!last_error_ && limit_node_level_) {
            // Final relative open/close sanity check
            if (0!=ac) {
                last_error_ = ERR_UNEXPECTED_END_OF_ARRAY;
            }
            if (0!=mc) {
                last_error_ = ERR_UNEXPECTED_END_OF_MAP;
            }
        }

        last_e_ = NULL;

        return i;
    } // FromString(...)


    /**
     * Parse given JSON object chain, build JSON string in I/O object, optionally build JSON 
     * string.
     *
     * \attention If callbacks employed via \ref Callbacks set to true then NO output string 
     * is created.  It is assumed the callbacks will handle JSON string generation.
     *
     * \note If I/O object is not a buffer of some kind you may not have any resulting 
     * JSON string.
     *
     * \note If object chain not start then node alignments will not be met, i.e.
     * there maybe more Array or Map closes than opens.  In this situation caller should 
     * disable via \ref LimitNodeLevel
     *
     * \param[in] e Pointer to first JSON element in object chain
     * \retval true Success
     * \retval false Failure, check \ref GetLastError
     */
    bool FromObject(JSONElement<LENGTH>* e) {
        bool wr_err = false;
        bool stop = false;
        int16_t ac = 0, mc = 0;

        last_e_ = NULL;
        like_ = NULL;
        last_error_ = ERR_NONE;
        operation_ = OP_FROM_OBJECT;
        io_->Length(0);
        while(e) {
            switch(e->Type()) {
                case JSONElement<LENGTH>::ARRAY : {
                    auto s = reinterpret_cast<JSONArray<LENGTH>*>(e);

                    if (callbacks_) {
                        stop = TokenArray(s);
                    }
                    if (s->IsOpen()) {
                        if (!callbacks_) {
                            if (last_e_ && JSONElement<LENGTH>::STRING == last_e_->Type()) {
                                auto ls = reinterpret_cast<JSONString<LENGTH>*>(last_e_);

                                if (ls->IsValue()) {
                                    wr_err |= io_->Append(", ");
                                }
                            }
                            wr_err |= io_->Append("[ ");
                        }

                        ac++;
                    }else {
                        if (!callbacks_) {
                            auto nsa = reinterpret_cast<JSONArray<LENGTH>*>(s->Next());    // Array or Map, same properties

                            // If next and (next is map or array and open) or next is string
                            if (s->Next() && (((JSONElement<LENGTH>::ARRAY == s->Next()->Type() || JSONElement<LENGTH>::MAP == s->Next()->Type())
                                                    && nsa->IsOpen()) || JSONElement<LENGTH>::STRING == s->Next()->Type())) {
                                wr_err |= io_->Append(" ], ");
                            }else {
                                wr_err |= io_->Append(" ]");
                            }
                        }

                        ac--;
                    }
                }
                break;

                case JSONElement<LENGTH>::MAP : {
                    auto s = reinterpret_cast<JSONMap<LENGTH>*>(e);

                    if (callbacks_) {
                        stop = TokenMap(s);
                    }
                    if (s->IsOpen()) {

                        if (!callbacks_) {
                            if (last_e_ && JSONElement<LENGTH>::STRING == last_e_->Type()) {
                                auto ls = reinterpret_cast<JSONString<LENGTH>*>(last_e_);

                                if (ls->IsValue()) {
                                    wr_err |= io_->Append(", ");
                                }
                            }
                            wr_err |= io_->Append("{ ");
                        }

                        mc++;
                    }else {
                        if (!callbacks_) {
                            auto nsa = reinterpret_cast<JSONArray<LENGTH>*>(s->Next());    // Array or Map, same properties

                            // If next and (next is map or array and open) or next is string
                            if (s->Next() && (((JSONElement<LENGTH>::ARRAY == s->Next()->Type() || JSONElement<LENGTH>::MAP == s->Next()->Type())
                                                    && nsa->IsOpen()) || JSONElement<LENGTH>::STRING == s->Next()->Type())) {
                                wr_err |= io_->Append(" }, ");
                            }else {
                                wr_err |= io_->Append(" }");
                            }
                        }

                        mc--;
                    }
                }
                break;

                case JSONElement<LENGTH>::STRING : {
                    auto s = reinterpret_cast<JSONString<LENGTH>*>(e);

                    if (callbacks_) {
                        stop = TokenString(s);
                    }else {
                        if (last_e_ && JSONElement<LENGTH>::STRING == last_e_->Type()) {
                            auto ls = reinterpret_cast<JSONString<LENGTH>*>(last_e_);
                    
                            if (ls->IsValue()) {
                                wr_err |= io_->Append(", ");
                            }
                        }

                        if (s->Length() > 0) {
                            // Any data
                            if (!s->IsValue()) {
                                // Has data
                                wr_err |= io_->Append("\"");
                                wr_err |= io_->Append(s->Data());
                                if (s->IsValue()) {
                                    wr_err |= io_->Append("\", ");
                                }else {
                                    wr_err |= io_->Append("\":");
                                }
                            }else {
								printf("v: %s\n", s->Data());
                                if (s->IsValueLiteral()) {
                                    wr_err |= io_->Append(s->Data());
                                }else {
                                    wr_err |= io_->Append("\"");
                                    wr_err |= io_->Append(s->Data());
                                    wr_err |= io_->Append("\"");
                                }
                            }
                        }
                    }
                }
                break;
            } // switch(e->Type())
            last_e_ = e;

            // A callback has handled something and wishes to complete
            if (stop) {
                last_error_ = ERR_CALLBACK_STOP;
            }

            if (ERR_NONE != last_error_) {
                break;
            }else {
                if (wr_err) {
                    last_error_ = ERR_CHAR_MEMORY_LIMIT;
                    break;
                }
            }

            if (limit_node_level_) {
                if (ac<0) {
                    last_error_ = ERR_UNEXPECTED_END_OF_ARRAY;
                    break;
                }
                if (mc<0) {
                    last_error_ = ERR_UNEXPECTED_END_OF_MAP;
                    break;
                }
                // Have we reached the base of where we started parsing?
                if (mc==0 && ac==0) {
                    break;
                }
            }

            e = e->Next();
        } // while(e)
        last_e_ = NULL;
        io_->Append('\0');    // Ignore write error


        return (ERR_NONE == last_error_) ? true : false;
    } // FromObjects(...)

    
    /**
     * Parse given JSON object chain looking for an object previously setup via \ref Like
     *
     * \note If object chain not start then node alignments will not be met, i.e.
     * there maybe more Array or Map closes than opens.  In this situation caller should 
     * disable via \ref LimitNodeLevel
     *
     * \param[in] e Pointer to first JSON element in object chain
     * \return Found element pointer or NULL (not found)
     */
    JSONElement<LENGTH>* FindObject(JSONElement<LENGTH>* e) {
        int16_t ac = 0, mc = 0;
        JSONElement<LENGTH>* f = NULL;

        last_e_ = NULL;
        last_error_ = ERR_NONE;
        operation_ = OP_FIND_OBJECT;
        while(e) {
            switch(e->Type()) {
                case JSONElement<LENGTH>::ARRAY : {
                    auto s = reinterpret_cast<JSONArray<LENGTH>*>(e);

                    if (s->IsOpen()) {
                        ac++;
                    }else {
                        ac--;
                    }
                    if (callbacks_) {
                        if (TokenArray(s)) {
                            f = e;
                            break;
                        }
                    }else {
                        if (like_ && e->Type() == like_->Type()) {
                            auto l = reinterpret_cast<JSONArray<LENGTH>*>(like_);

                            if (s->IsOpen() == l->IsOpen()) {
                                f = e;
                                break;
                            }
                        }
                    }
                }
                break;

                case JSONElement<LENGTH>::MAP : {
                    auto s = reinterpret_cast<JSONMap<LENGTH>*>(e);

                    if (s->IsOpen()) {
                        mc++;
                    }else {
                        mc--;
                    }
                    if (callbacks_) {

                        if (TokenMap(s)) {
                            f = e;
                            break;
                        }
                    }else {
                        if (like_ && e->Type() == like_->Type()) {
                            auto l = reinterpret_cast<JSONMap<LENGTH>*>(like_);

                            if (s->IsOpen() == l->IsOpen()) {
                                f = e;
                                break;
                            }
                        }
                    }
                }
                break;

                case JSONElement<LENGTH>::STRING : {
                    auto s = reinterpret_cast<JSONString<LENGTH>*>(e);

                    if (callbacks_) {
                        if (TokenString(s)) {
                            f = e;
                            break;
                        }
                    }else {
                        if (like_ && e->Type() == like_->Type()) {
                            auto l = reinterpret_cast<JSONString<LENGTH>*>(like_);

                            if (s->IsValue() == l->IsValue()) {
                                // Any string to compare?
                                if (!l->Length()) {
                                    // Accept any string
                                    f = e;
                                    break;
                                }
                                if (case_sensitive_) {
                                    if (GJSON_REPLACE_STRNCMP(l->Data(), s->Data(), l->Length())>=0) {
                                        f = e;
                                        break;
                                    }
                                }else {
                                    if (GJSON_REPLACE_STRNCMPI(l->Data(), s->Data(), l->Length())>=0) {
                                        f = e;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            } // switch(e->Type())
            last_e_ = e;

            if (f) {
                break;    // Found
            }
            if (limit_node_level_) {
                if (ac<0) {
                    last_error_ = ERR_UNEXPECTED_END_OF_ARRAY;
                    break;
                }
                if (mc<0) {
                    last_error_ = ERR_UNEXPECTED_END_OF_MAP;
                    break;
                }
            }

            e = e->Next();
        } // while(e)
        last_e_ = NULL;
        like_ = NULL;

        if (!f && ERR_NONE == last_error_) {
            last_error_ = ERR_NOT_FOUND;
        }

        return f;
    } // FindObject(...)


    /**
     * Parse given JSON object chain looking for next Array object (from given chain entry)
     * Routine uses \ref FindObject, see for further details.
     *
     * \param[in] e Pointer to first JSON element in object chain
     * \param[in] is_open Array must be an open(true, default) or close type(false)
     * \return Found element pointer or NULL (not found)
     */
    JSONArray<LENGTH>* NextArray(JSONElement<LENGTH>* e, const bool is_open=true) {
        JSONArray<LENGTH>* fe = NULL;

        operation_ = OP_FIND_OBJECT;
        last_error_ = ERR_NOT_FOUND;
        if (e) {
            auto m = JSONArray<LENGTH>(is_open);
            Like(&m);
            fe = reinterpret_cast<JSONArray<LENGTH>*>(FindObject(e->Next()));
        }

        return fe;
    }


    /**
     * Parse given JSON object chain looking for next Map object (from given chain entry).
     * Routine uses \ref FindObject, see for further details.
     *
     * \param[in] e Pointer to first JSON element in object chain
     * \param[in] is_open Map must be an open(true, default) or close type(false)
     * \return Found element pointer or NULL (not found)
     */
    JSONMap<LENGTH>* NextMap(JSONElement<LENGTH>* e, bool is_open=true) {
        JSONMap<LENGTH>* fe = NULL;

        operation_ = OP_FIND_OBJECT;
        last_error_ = ERR_NOT_FOUND;
        if (e) {
            auto m = JSONMap<LENGTH>(is_open);
            Like(&m);
            fe = reinterpret_cast<JSONMap<LENGTH>*>(FindObject(e->Next()));
        }

        return fe;
    }


    /**
     * Parse given JSON object chain looking for next String object (from given chain entry).
     * Routine uses \ref FindObject, see for further details.
     *
     * \param[in] e Pointer to first JSON element in object chain
     * \param[in] is_value String must be a value(true, default) or property/attribute 
     * type(false)
     * \return Found element pointer or NULL (not found)
     */
    JSONString<LENGTH>* NextString(JSONElement<LENGTH>* e, bool is_value=true) {
        JSONString<LENGTH>* fe = NULL;

        operation_ = OP_FIND_OBJECT;
        last_error_ = ERR_NOT_FOUND;
        if (e) {
            auto m = JSONString<LENGTH>(is_value);
            Like(&m);
            fe = reinterpret_cast<JSONString<LENGTH>*>(FindObject(e->Next()));
        }

        return fe;
    }


    /**
     * Parse given JSON object chain looking for a String object (from given chain entry)
     * matching string characteristics.  Routine uses \ref FindObject, see for further 
     * details.
     *
     * \param[in] e Pointer to first JSON element in object chain
     * \param[in] s Pointer to NULL terminated search string
     * \param[in] is_value String must be a value(true, default) or property/attribute 
     * type(false)
     * \param[in] case_sensitive Flag indicating search should be case sensitive, default
     * false
     * \return Found element pointer or NULL (not found)
     */
    JSONString<LENGTH>* FindString(JSONElement<LENGTH>* e, char *s, bool is_value=true, const bool case_sensitive=false) {
        JSONString<LENGTH>* fe = NULL;

        operation_ = OP_FIND_OBJECT;
        last_error_ = ERR_NOT_FOUND;
        if (e) {
            auto m = JSONString<LENGTH>(s, is_value);
            Like(&m, case_sensitive);
            fe = reinterpret_cast<JSONString<LENGTH>*>(FindObject(e));
        }

        return fe;
    }


    /**
     * Get or set object chain creation memory limit (Bytes)
     *
     * \note Read required memory via \ref GetBinarySize
     *
     * \param[in] limit_memory Bytes.  Default 0 implies read only, otherwise set and read
     * \return Memory limit, Bytes.  If not updated then new value not accepted
     */
    uint16_t MemoryLimit(const uint16_t limit_memory=0) {
        if (limit_memory) {
            limit_memory_ = limit_memory;
        }

        return limit_memory_;
    }


    /**
     * Setup JSON element to find by \ref FindObject
     *
     * \param[in] like Pointer to element containing Like information for JSON search
     * \param[in] case_sensitive For string comparisons, case sensitive flag, default 
     * true
     */
    void Like(JSONElement<LENGTH>* like, const bool case_sensitive=true) {
        like_ = like;
        case_sensitive_ = case_sensitive;
    }


    /**
     * Set limit node level flag.  This affects parsing where Array and Map open 
     * and closes are checked and matched
     *
     * \param[in] limit_node_level Flag state, default true
     */
    void LimitNodeLevel(const bool limit_node_level=true) {
        limit_node_level_ = limit_node_level;
    }


    /**
     * Set Callbacks flag indicating use of callbacks.  This is used by parsing both 
     * object and character JSON.  By default callbacks are not implemented within this class
     * but subclasses will likely implement.
     *
     * \attention Callbacks should only be employed if custom functionality required by 
     * subclasses
     *
     * \param[in] callbacks Flag state
     */
    void Callbacks(bool callbacks) {
        callbacks_ = callbacks;
    }


	/**
	 * Get Callbacks flag indicating use of callbacks
	 *
	 * \retval true Callbacks used
	 * \retval false Callbacks not used
	 */
	bool Callbacks() {
		return callbacks_;
	}


    /**
     * Get last JSON parse or find error condition (if any).  For list see enum \ref ERR
     *
     * \return ERR numeric
     */
    ERR GetLastError() const {
        return last_error_;
    }


    /**
     * Get last or current JSON parse or find operation (if any).  For list see enum \ref OP
     *
     * \return OP numeric
     */
    OP GetLastOperation() const {
        return operation_;
    }


    /**
     * Get object chain binary size.  This will be the result of a character string parse, 
     * see \ref FromString
     *
     * \note Increase via \ref MemoryLimit
     *
     * \return Bytes
     */
    uint32_t GetBinarySize() const {
        return binary_size_;
    }


    /**
     * Get I/O object data length, see \ref gjson::IO::GetLength
     *
     * \return Characters
     */
    uint16_t Length() {
        return io_->GetLength();
    }


    /**
     * Get I/O object data pointer
     *
     * \return Pointer to NULL terminated string
     */
    const char* Get() {
        return io_->Get();
    }


    /**
     * Release object chain
     *
     * \attention Caller should make sure no pointers in other objects exist to elements 
     * of the released chain prior to invoke.
     *
     * \todo Attempt to remove this method
     *
     * \param[in] h Pointer to pointer of first JSON element in object chain. This will 
     * be NULLed upon completion
     */
    void Release(JSONElement<LENGTH>** h) {
        // deref head_ref to get the real head
        JSONElement<LENGTH>* c = *h;
        JSONElement<LENGTH>* n;

        // Deref any internals
        Like(NULL);
        last_e_ = NULL;

        operation_ = OP_RELEASE_OBJECT;
        uint16_t rt = 0;
        while(c != NULL) {
            n = c->Next();  
            GJSON_RELEASE_OBJECT( c );
            c = n;
        }
      
        // Deref head_ref to affect the real head back in the caller
        *h = NULL;  
    }

protected:
    /**
     * Check if character exists within JSON literal alphabet, if all characters of a string 
     * were within alphabet then that string could be a literal.
     *
     * \return Literal character state
     */
    bool IsLiteralCharater(char c) {
        const char literals[] = "0123456789.-+eEnultrfas";    // Digits, signs, decimal point and alphas for true, false and null
        uint16_t i;
        bool lc = false;

        for(i=0; i<sizeof(literals); i++) {
            if (c == literals[i]) {
                lc = true;
                break;
            }
        }

        return lc;
    } // IsLiteralCharater(...)


    /**
     * Helper to set case sensitive searching
     *
     * \param[in] case_sensitive For string comparisons, case sensitive flag, default 
     * true
     */
    void CaseSensitive(const bool case_sensitive) {
        case_sensitive_ = case_sensitive;
    }


    /**
     * Helper to set error condition
     *
     * \param[in] error Error condition code, \ref ERR
     */
    void Error(const ERR error) {
        last_error_ = error;
    }


protected:
    ERR         last_error_;                ///< Last error
    OP          operation_;                 ///< Current operation
    uint16_t    binary_size_;               ///< Size of binary, Bytes
    uint16_t    limit_memory_;              ///< Limit of memory, Bytes
    IO*         io_;                        ///< IO instance pointer
    bool        limit_node_level_;          ///< Limit node level flag
    bool        callbacks_;                 ///< Callbacks employed flag
    bool        case_sensitive_;            ///< Case sensitive checks flag
    JSONElement<LENGTH>* last_e_;           ///< Last element processed pointer
    JSONElement<LENGTH>* like_;             ///< Search like element pointer
}; // class JSONParse


/**
 * JSON non-recursive string parser by subclass for searching and replacing JSON string attributes/properties in Maps.  Uses user supplied
 * I/O as source of JSON string.
 *
 * \tparam LENGTH Maximum length (including NULL terminator) in characters of a string attribute/property
 */
template<uint8_t LENGTH>
class JSONSimpleStringParseAttr : protected JSONParse<LENGTH> {
public:
    /**
     * \copydoc JSONParse::ERR
     */
    using JSONParse<LENGTH>::ERR;


    /**
     * \copydoc JSONParse::GetLastError
     */
    using JSONParse<LENGTH>::GetLastError;


    /**
     * \copydoc JSONParse::LimitNodeLevel
     */
    using JSONParse<LENGTH>::LimitNodeLevel;


    /**
     * \copydoc JSONParse::Length
     */
    using JSONParse<LENGTH>::Length;

    /**
     * Constructor, make stable instance with given I/O instance
     *
     * \param[in,out] io Pointer to I/O instance, source of JSON string for parsing
     */
    JSONSimpleStringParseAttr(IO* io) : take_next_(false), replace_(false), rename_(false), attr_(NULL), adj_i_(0), attr_length_(0), JSONParse<LENGTH>(io, 0U) {
        JSONParse<LENGTH>::Callbacks(true);
        value_[0] = '\0';
    }


    /**
     * Parse given JSON string looking for specific attribute/property, providing the 
     * value
     *
     * \attention Caller should check \ref GetLastError upon completion for any error
     * condition that may have risen during parse
     *
     * \attention Get value string of found attribute/property by \ref GetAttrValue
     *
     * \note If JSON string part way through then node alignments will not be met, i.e.
     * there maybe more Array or Map closes than opens.  In this situation caller should 
     * disable via \ref LimitNodeLevel
     *
     * \param[in] a Attribute/property string (NULL terminated), search string
     * \param[in] case_sensitive Flag indicating search should be case sensitive, default true
     * \return Last character index parsed
     */
    uint16_t FromStringSearch(char *a, const bool case_sensitive=true) {
        return FromStringSearch(0, a, case_sensitive);
    }


    /**
     * Parse given JSON string looking for specific attribute/property, providing the 
     * value
     *
     * \attention Caller should check \ref GetLastError upon completion for any error
     * condition that may have risen during parse
     *
     * \attention Get value string of found attribute/property by \ref GetAttrValue
     *
     * \note If JSON string part way through then node alignments will not be met, i.e.
     * there maybe more Array or Map closes than opens.  In this situation caller should 
     * disable via \ref LimitNodeLevel
     *
     * \param[in] psi Parse start character >=0 index < \ref Length
     * \param[in] a Attribute/property string (NULL terminated), search string
     * \param[in] case_sensitive Flag indicating search should be case sensitive, default true
     * \return Last character index parsed
     */
    uint16_t FromStringSearch(const uint16_t psi, char *a, const bool case_sensitive=true) {
        uint16_t pi;

        take_next_ = false;
        attr_ = a;
        attr_length_ = strlen(a);
        if (attr_length_>=LENGTH) {
            attr_length_ = LENGTH-1;
        }
        value_[0]='\0';
        JSONParse<LENGTH>::CaseSensitive(case_sensitive);
        rename_ = false;
        replace_ = false;

        pi = JSONParse<LENGTH>::FromString(psi);
        if (JSONParse<LENGTH>::ERR_NONE == GetLastError()) {
            JSONParse<LENGTH>::Error(JSONParse<LENGTH>::ERR_NOT_FOUND);
        }else if (JSONParse<LENGTH>::ERR_CALLBACK_STOP == GetLastError()) {
            JSONParse<LENGTH>::Error(JSONParse<LENGTH>::ERR_NONE);
        }

        return pi;
    }


    /**
     * Parse given JSON string looking for specific attribute/property then replace value
     *
     * \attention Caller should check \ref GetLastError upon completion for any error
     * condition that may have risen during parse
     *
     * \note If JSON string incomplete then node alignments will not be met, i.e. there maybe 
     * more Array or Map closes than opens.  In this situation caller should disable via 
     * \ref LimitNodeLevel
     *
     * \param[in] a Attribute/property string (NULL terminated), search string
     * \param[in] v Replacement value string (NULL terminated)
     * \param[in] case_sensitive Flag indicating search should be case sensitive, default true
     * \return Last character index parsed
     */
    uint16_t FromStringReplace(char *a, char *v, const bool case_sensitive=true) {
        return FromStringReplace(0, a, v, case_sensitive);
    }


    /**
     * Parse given JSON string looking for specific attribute/property with replace of 
     * value
     *
     * \attention Caller should check \ref GetLastError upon completion for any error
     * condition that may have risen during parse
     *
     * \note If JSON string part way through (psi!=0) then node alignments will not be met, i.e.
     * there maybe more Array or Map closes than opens.  In this situation caller should 
     * disable via \ref LimitNodeLevel
     *
     * \param[in] psi Parse start character >=0 index < \ref Length
     * \param[in] a Attribute/property string (NULL terminated), search string
     * \param[in] v Replacement value string (NULL terminated)
     * \param[in] case_sensitive Flag indicating search should be case sensitive, default true
     * \return Last character index parsed
     */
    uint16_t FromStringReplace(const uint16_t psi, char *a, char *v, const bool case_sensitive=true) {
        uint16_t l = strlen(v), pi;

        take_next_ = false;
        attr_ = a;
        attr_length_ = strlen(a);
        if (attr_length_>=LENGTH) {
            attr_length_ = LENGTH-1;
        }
        value_[0]='\0';
        if (l>=LENGTH) {
            l = LENGTH-1;
        }
        strncpy(value_, v, l);
        value_[l]='\0';
        JSONParse<LENGTH>::CaseSensitive(case_sensitive);
        rename_ = false;
        replace_ = true;
        adj_i_ = 0;

        pi = JSONParse<LENGTH>::FromString(psi);
        uint32_t err = GetLastError();

        if (JSONParse<LENGTH>::ERR_NONE == GetLastError()) {
            JSONParse<LENGTH>::Error(JSONParse<LENGTH>::ERR_NOT_FOUND);
        }else if (JSONParse<LENGTH>::ERR_CALLBACK_STOP == GetLastError()) {
            JSONParse<LENGTH>::Error(JSONParse<LENGTH>::ERR_NONE);
            //pi += adj_i_;
        }

        return pi;
    } // FromStringReplace(...)


    /**
     * Parse given JSON string looking for specific attribute/property with rename of 
     * attribute/propery
     *
     * \attention Caller should check \ref GetLastError upon completion for any error
     * condition that may have risen during parse
     *
     * \note If JSON string incomplete then node alignments will not be met, i.e. there maybe 
     * more Array or Map closes than opens.  In this situation caller should disable via 
     * \ref LimitNodeLevel
     *
     * \param[in] a Attribute/property string (NULL terminated), search string
     * \param[in] ra Replacement Attribute/property string (NULL terminated)
     * \param[in] case_sensitive Flag indicating search should be case sensitive, default true
     * \return Last character index parsed
     */
    uint16_t FromStringRename(char *a, char *ra, const bool case_sensitive=true) {
        return FromStringRename(0, a, ra, case_sensitive);
    }


    /**
     * Parse given JSON string looking for specific attribute/property with rename of 
     * attribute/propery
     *
     * \attention Caller should check \ref GetLastError upon completion for any error
     * condition that may have risen during parse
     *
     * \note If JSON string part way through (psi!=0) then node alignments will not be met, i.e.
     * there maybe more Array or Map closes than opens.  In this situation caller should 
     * disable via \ref LimitNodeLevel
     *
     * \param[in] psi Parse start character >=0 index < \ref Length
     * \param[in] a Attribute/property string (NULL terminated), search string
     * \param[in] ra Replacement Attribute/property string (NULL terminated)
     * \param[in] case_sensitive Flag indicating search should be case sensitive, default true
     * \return Last character index parsed
     */
    uint16_t FromStringRename(const uint16_t psi, char *a, char *ra, const bool case_sensitive=true) {
        uint16_t l = strlen(ra), pi;

        take_next_ = false;
        attr_ = a;
        attr_length_ = strlen(a);
        if (attr_length_>=LENGTH) {
            attr_length_ = LENGTH-1;
        }
        value_[0]='\0';
        if (l>=LENGTH) {
            l = LENGTH-1;
        }
        strncpy(value_, ra, l);
        value_[l]='\0';
        JSONParse<LENGTH>::CaseSensitive(case_sensitive);
        rename_ = true;
        replace_ = false;
        adj_i_ = 0;

        pi = JSONParse<LENGTH>::FromString(psi);
        uint32_t err = GetLastError();

        if (JSONParse<LENGTH>::ERR_NONE == GetLastError()) {
            JSONParse<LENGTH>::Error(JSONParse<LENGTH>::ERR_NOT_FOUND);
        }else if (JSONParse<LENGTH>::ERR_CALLBACK_STOP == GetLastError()) {
            JSONParse<LENGTH>::Error(JSONParse<LENGTH>::ERR_NONE);
            pi += adj_i_;
        }

        return pi;
    } // FromStringRename(...)


    /**
     * Get attribute/property value string
     *
     * \return String pointer, NULL terminated
     */
    const char* GetAttrValue() const {
        return value_;
    }


    bool TokenArray(const JSONArray<LENGTH>* e, char *s=NULL, uint16_t i=0) {
        // String parsing?
        if (JSONParse<LENGTH>::OP_FROM_STRING == JSONParse<LENGTH>::GetLastOperation()) {
            if (take_next_) {
                // Shouldn't get here for string properties, likely an array or map was value not a string
                take_next_ = false;
            }
        }

        return false;
    } // TokenArray(...)


    bool TokenMap(const JSONArray<LENGTH>* e, char *s=NULL, uint16_t i=0) {
        // String parsing?
        if (JSONParse<LENGTH>::OP_FROM_STRING == JSONParse<LENGTH>::GetLastOperation()) {
            if (take_next_) {
                // Shouldn't get here for string properties, likely an array or map was value not a string
                take_next_ = false;
            }
        }

        return false;
    } // TokenMap(...)


    bool TokenString(const JSONString<LENGTH>* e, char *s=NULL, int16_t si=0, int16_t ei=0) {
        bool found = false;

        // String parsing?
        if (JSONParse<LENGTH>::OP_FROM_STRING == JSONParse<LENGTH>::GetLastOperation()) {
            if (e->IsValue()) {
                if (take_next_) {
                    if (replace_ && si!=ei && si<ei) {
                        uint16_t val_length = strlen(value_);
                        // Replace attribute value
                        uint16_t adj_length = JSONSupport::Replace(s, JSONParse<LENGTH>::io_->GetLength(), JSONParse<LENGTH>::io_->GetLength(true), si, ei, value_, val_length);
                        if (!adj_length) {
                            JSONParse<LENGTH>::io_->Length(adj_length);
                        }
                        // Figure out adjust character index i to make sure the ending character is still the same after replace
                        adj_i_ = val_length - e->Length();
                    }else {
                        strcpy(value_, e->Data());    // Take
                    }
                    found = true;
                    take_next_ = false;
                }
            }else {
                if (!take_next_ && attr_length_) {
                    if (JSONParse<LENGTH>::case_sensitive_) {
                        if (!GJSON_REPLACE_STRNCMP(e->Data(), attr_, attr_length_)) {
                            take_next_ = true;
                        }
                    }else {
                        if (!GJSON_REPLACE_STRNCMPI(e->Data(), attr_, attr_length_)) {
                            take_next_ = true;
                        }
                    }
                    if (rename_ && take_next_ && si!=ei && si<ei) {
                        uint16_t val_length = strlen(value_);
                        // Replace attribute name
                        uint16_t adj_length = JSONSupport::Replace(s, JSONParse<LENGTH>::io_->GetLength(), JSONParse<LENGTH>::io_->GetLength(true), si, ei, value_, val_length);
                        if (!adj_length) {
                            JSONParse<LENGTH>::io_->Length(adj_length);
                        }
                        // Figure out adjust character index i to make sure the ending character is still the same after replace
                        adj_i_ = val_length - e->Length();
                        found = true;
                        take_next_ = false;
                    }
                }
            }
        }

        return found;
    } // TokenString(...)

protected:
    char        value_[LENGTH];             ///< Value string
    char*       attr_;                      ///< Attibute/Property string pointer
    uint16_t    attr_length_;               ///< Attribute/Property string length, Characters
    int16_t     adj_i_;                     ///< Adjust index i numeric
    bool        take_next_;                 ///< Take next element flag
    bool        replace_;                   ///< Replace flag
    bool        rename_;                    ///< Rename flag
}; // class JSONSimpleStringParseAttr


/**
 * JSON non-recursive string parser by subclass for searching and replacing JSON strings, both in Arrays and Maps; with the latter 
 * there is know knowledge of value or attribute/property.  Uses user supplied I/O as source of JSON string.
 *
 * \tparam LENGTH Maximum length (including NULL terminator) in characters of a string attribute/property
 */
template<uint8_t LENGTH>
class JSONSimpleStringParse : protected JSONParse<LENGTH> {
public:
    /**
     * \copydoc JSONParse::ERR
     */
    using JSONParse<LENGTH>::ERR;


    /**
     * \copydoc JSONParse::GetLastError
     */
    using JSONParse<LENGTH>::GetLastError;


    /**
     * \copydoc JSONParse::LimitNodeLevel
     */
    using JSONParse<LENGTH>::LimitNodeLevel;


    /**
     * \copydoc JSONParse::Length
     */
    using JSONParse<LENGTH>::Length;

    /**
     * Constructor, make stable instance with given I/O instance
     *
     * \param[in,out] io Pointer to I/O instance, source of JSON string for parsing
     */
    JSONSimpleStringParse(IO* io) : replace_(false), str_(NULL), str_length_(0), adj_i_(0), JSONParse<LENGTH>(io, 0U) {
        JSONParse<LENGTH>::Callbacks(true);
        value_[0] = '\0';
    }


    /**
     * Parse given JSON string looking for specific string.  Provides character index
     *
     * \attention Caller should check \ref GetLastError upon completion for any error
     * condition that may have risen during parse
     *
     * \note If JSON string part way through then node alignments will not be met, i.e.
     * there maybe more Array or Map closes than opens.  In this situation caller should 
     * disable via \ref LimitNodeLevel
     *
     * \param[in] s String (NULL terminated), search string
     * \param[in] case_sensitive Flag indicating search should be case sensitive, default true
     * \return Last character index parsed
     */
    uint16_t FromStringSearch(char *s, const bool case_sensitive=true) {
        return FromStringSearch(0, s, case_sensitive);
    }


    /**
     * Parse given JSON string looking for specific string.  Provides character index
     *
     * \attention Caller should check \ref GetLastError upon completion for any error
     * condition that may have risen during parse
     *
     * \note If JSON string part way through then node alignments will not be met, i.e.
     * there maybe more Array or Map closes than opens.  In this situation caller should 
     * disable via \ref LimitNodeLevel
     *
     * \param[in] psi Parse start character >=0 index < \ref Length
     * \param[in] s String (NULL terminated), search string
     * \param[in] case_sensitive Flag indicating search should be case sensitive, default true
     * \return Last character index parsed
     */
    uint16_t FromStringSearch(const uint16_t psi, char *s, const bool case_sensitive=true) {
        uint16_t pi;

        str_ = s;
        str_length_ = strlen(s);
        if (str_length_>=LENGTH) {
            str_length_ = LENGTH-1;
        }
        value_[0]='\0';
        replace_ = false;
        JSONParse<LENGTH>::CaseSensitive(case_sensitive);
        adj_i_ = 0;

        pi = JSONParse<LENGTH>::FromString(psi);
        if (JSONParse<LENGTH>::ERR_NONE == GetLastError()) {
            JSONParse<LENGTH>::Error(JSONParse<LENGTH>::ERR_NOT_FOUND);
        }else if (JSONParse<LENGTH>::ERR_CALLBACK_STOP == GetLastError()) {
            JSONParse<LENGTH>::Error(JSONParse<LENGTH>::ERR_NONE);
        }

        return pi;
    }


    /**
     * Parse given JSON string looking for specific string then replace it.  Provides 
     * character index of replacement
     *
     * \attention Caller should check \ref GetLastError upon completion for any error
     * condition that may have risen during parse
     *
     * \note If JSON string incomplete then node alignments will not be met, i.e. there maybe 
     * more Array or Map closes than opens.  In this situation caller should disable via 
     * \ref LimitNodeLevel
     *
     * \param[in] s String (NULL terminated), search string
     * \param[in] rs Replacement value string (NULL terminated)
     * \param[in] case_sensitive Flag indicating search should be case sensitive, default true
     * \return Last character index parsed
     */
    uint16_t FromStringReplace(char *s, char *rs, const bool case_sensitive=true) {
        return FromStringReplace(0, s, rs, case_sensitive);
    }


    /**
     * Parse given JSON string looking for specific string then replace it.  Provides 
     * character index of replacement
     *
     * \attention Caller should check \ref GetLastError upon completion for any error
     * condition that may have risen during parse
     *
     * \note If JSON string part way through (psi!=0) then node alignments will not be met, i.e.
     * there maybe more Array or Map closes than opens.  In this situation caller should 
     * disable via \ref LimitNodeLevel
     *
     * \param[in] psi Parse start character >=0 index < \ref Length
     * \param[in] s String (NULL terminated), search string
     * \param[in] rs Replacement value string (NULL terminated)
     * \param[in] case_sensitive Flag indicating search should be case sensitive, default true
     * \return Last character index parsed
     */
    uint16_t FromStringReplace(const uint16_t psi, char *s, char *rs, const bool case_sensitive=true) {
        uint16_t l = strlen(rs), pi;

        str_ = s;
        str_length_ = strlen(s);
        if (str_length_>=LENGTH) {
            str_length_ = LENGTH-1;
        }
        value_[0]='\0';
        replace_ = true;
        if (l>=LENGTH) {
            l = LENGTH-1;
        }
        strncpy(value_, rs, l);
        value_[l]='\0';
        JSONParse<LENGTH>::CaseSensitive(case_sensitive);
        adj_i_ = 0;

        pi = JSONParse<LENGTH>::FromString(psi);
        uint32_t err = GetLastError();

        if (JSONParse<LENGTH>::ERR_NONE == GetLastError()) {
            JSONParse<LENGTH>::Error(JSONParse<LENGTH>::ERR_NOT_FOUND);
        }else if (JSONParse<LENGTH>::ERR_CALLBACK_STOP == GetLastError()) {
            JSONParse<LENGTH>::Error(JSONParse<LENGTH>::ERR_NONE);
            //pi += adj_i_;
        }

        return pi;
    } // FromStringReplace(...)


    bool TokenString(const JSONString<LENGTH>* e, char *s=NULL, int16_t si=0, int16_t ei=0) {
        bool found = false;

        // String parsing?
        if (JSONParse<LENGTH>::OP_FROM_STRING == JSONParse<LENGTH>::GetLastOperation()) {
            if (str_length_) {
                if (JSONParse<LENGTH>::case_sensitive_) {
                    if (!GJSON_REPLACE_STRNCMP(e->Data(), str_, str_length_)) {
                        found = true;
                    }
                }else {
                   if (!GJSON_REPLACE_STRNCMPI(e->Data(), str_, str_length_)) {
                        found = true;
                    }
                }
                if (found) {
                    // Replace string?
                    if (replace_ && si!=ei && si<ei) {
                        uint16_t val_length = strlen(value_);
                        uint16_t adj_length = JSONSupport::Replace(s, JSONParse<LENGTH>::io_->GetLength(), JSONParse<LENGTH>::io_->GetLength(true), si, ei, value_, val_length);
                        if (!adj_length) {
                            JSONParse<LENGTH>::io_->Length(adj_length);
                        }
                        // Figure out adjust character index i to make sure the ending character is still the same after replace
                        adj_i_ = val_length - e->Length();
                    }
                }
            }
        }

        return found;
    } // TokenString(...)

protected:
    char        value_[LENGTH];             ///< Value string
    char*       str_;                       ///< Value string pointer
    uint16_t    str_length_;                ///< Value string length, Characters
    int16_t     adj_i_;                     ///< Adjust flag
    bool        replace_;                   ///< Replace flag
}; // class JSONSimpleStringParse



#if !defined(GJSON_NO_EXAMPLES)

/**
 * JSON non-recursive simple string parser by subclass example using no buffering.  Parse user supplied string with callbacks
 * where user can dictate what is done with parsed data, here it is simply printed to console.
 *
 * \tparam LENGTH Maximum length (including NULL terminator) in characters of a string attribute/property
 */
template<uint8_t LENGTH>
class JSONExampleStringParse : protected JSONParse<LENGTH> {
public:
    /**
     * \copydoc JSONParse::ERR
     */
    using JSONParse<LENGTH>::ERR;


    /**
     * \copydoc JSONParse::GetLastError
     */
    using JSONParse<LENGTH>::GetLastError;


    /**
     * \copydoc JSONParse::LimitNodeLevel
     */
    using JSONParse<LENGTH>::LimitNodeLevel;


    /**
     * Default constructor, make stable instance.  Setup NULL I/O, base parser and enable callbacks
     */
    JSONExampleStringParse() : jio_(), JSONParse<LENGTH>(&jio_, 0U) {
        JSONParse<LENGTH>::Callbacks(true);
    }


    /**
     * JSON Parse user supplied string
     *
     * \attention Caller should check \ref GetLastError upon completion for any error
     * condition that may have risen during parse
     *
     * \note If JSON string part way through then node alignments will not be met, i.e.
     * there maybe more Array or Map closes than opens.  In this situation caller should 
     * disable via \ref LimitNodeLevel
     *
     * param[in] s JSON NULL terminated string
     * \return Last character index parsed.  Debug aid upon error conditions
     */
    uint16_t FromString(char *s) {
        return JSONParse<LENGTH>::FromString(s, NULL);
    }


    /**
     * \copydoc JSONTokenizer::TokenArray
     */
    bool TokenArray(const JSONArray<LENGTH>* e, char *s=NULL, uint16_t i=0) {
        // String parsing?
        if (JSONParse<LENGTH>::OP_FROM_STRING == JSONParse<LENGTH>::GetLastOperation()) {
            if (e->IsOpen()) {
                GJSON_PRINTLINE("[");
            }else {
                GJSON_PRINTLINE("]");
            }
        }

        return false;
    }


    /**
     * \copydoc JSONTokenizer::TokenMap
     */
    bool TokenMap(const JSONArray<LENGTH>* e, char *s=NULL, uint16_t i=0) {
        // String parsing?
        if (JSONParse<LENGTH>::OP_FROM_STRING == JSONParse<LENGTH>::GetLastOperation()) {
            if (e->IsOpen()) {
                GJSON_PRINTLINE("{");
            }else {
                GJSON_PRINTLINE("}");
            }
        }

        return false;
    }


    /**
     * \copydoc JSONTokenizer::TokenString
     */
    bool TokenString(const JSONString<LENGTH>* e, char *s=NULL, int16_t si=0, int16_t ei=0) {
        // String parsing?
        if (JSONParse<LENGTH>::OP_FROM_STRING == JSONParse<LENGTH>::GetLastOperation()) {
            if (e->IsValue()) {
                if (e->IsValueLiteral()) {
                    GJSON_PRINT("LVal(");
                    GJSON_PRINTSTR(e->Data());
                    GJSON_PRINTLINE(")");
                }else {
                    GJSON_PRINT("Val(");
                    GJSON_PRINTSTR(e->Data());
                    GJSON_PRINTLINE(")");
                }
            }else {
                GJSON_PRINT("Prop(");
                GJSON_PRINTSTR(e->Data());
                GJSON_PRINTLINE(")");
            }
        }

        return false;
    }

protected:
    NullBuffer  jio_;                       ///< NULL buffer instance
}; // class JSONExampleStringParse


/**
 * JSON non-recursive simple object parser by subclass example.  Parse user supplied object chain with callbacks
 * where user can dictate what is done with parsed data, here it is simply written to Arduino debug Serial and 
 * there is no I/O because in this example no string is created.
 *
 * \tparam LENGTH Maximum length (including NULL terminator) in characters of a string attribute/property
 */
template<uint8_t LENGTH>
class JSONExampleObjectParse : protected JSONParse<LENGTH> {
public:
    /**
     * \copydoc JSONParse::ERR
     */
    using JSONParse<LENGTH>::ERR;


    /**
     * \copydoc JSONParse::GetLastError
     */
    using JSONParse<LENGTH>::GetLastError;


    /**
     * \copydoc JSONParse::LimitNodeLevel
     */
    using JSONParse<LENGTH>::LimitNodeLevel;


    /**
     * Default constructor, make stable instance.  Setup NULL I/O, base parser and enable callbacks
     */
    JSONExampleObjectParse() : jio_(), JSONParse<LENGTH>(&jio_, 0U) {
        JSONParse<LENGTH>::Callbacks(true);
    }


    /**
     * Parse given JSON object chain, parsed JSON objects will invoke the callback interface
     *
     * \note No JSON is built because I/O buffer doesn't exist (NULL)
     *
     * \note If object chain not start then node alignments will not be met, i.e.
     * there maybe more Array or Map closes than opens.  In this situation caller should 
     * disable via \ref LimitNodeLevel
     *
     * \param[in] e Pointer to first JSON element in object chain
     * \retval true Success
     * \retval false Failure, check \ref GetLastError
     */
    bool FromObject(JSONElement<LENGTH>* e) {
        return JSONParse<LENGTH>::FromObject(e);
    }


    /**
     * \copydoc JSONTokenizer::TokenArray
     */
    bool TokenArray(const JSONArray<LENGTH>* e, char *s=NULL, uint16_t i=0) {
        // Object parsing?
        if (JSONParse<LENGTH>::OP_FROM_OBJECT == JSONParse<LENGTH>::GetLastOperation()) {
            if (e->IsOpen()) {
                GJSON_PRINTLINE("[");
            }else {
                GJSON_PRINTLINE("]");
            }
        }

        return false;
    }


    /**
     * \copydoc JSONTokenizer::TokenMap
     */
    bool TokenMap(const JSONArray<LENGTH>* e, char *s=NULL, uint16_t i=0) {
        // Object parsing?
        if (JSONParse<LENGTH>::OP_FROM_OBJECT == JSONParse<LENGTH>::GetLastOperation()) {
            if (e->IsOpen()) {
                GJSON_PRINTLINE("{");
            }else {
                GJSON_PRINTLINE("}");
            }
        }

        return false;
    }


    /**
     * \copydoc JSONTokenizer::TokenString
     */
    bool TokenString(const JSONString<LENGTH>* e, char *s=NULL, int16_t si=0, int16_t ei=0) {
        // Object parsing?
        if (JSONParse<LENGTH>::OP_FROM_OBJECT == JSONParse<LENGTH>::GetLastOperation()) {
            if (e->IsValue()) {
                if (e->IsValueLiteral()) {
                    GJSON_PRINT("LVal(");
                    GJSON_PRINTSTR(e->Data());
                    GJSON_PRINTLINE(")");
                }else {
                    GJSON_PRINT("Val(");
                    GJSON_PRINTSTR(e->Data());
                    GJSON_PRINTLINE(")");
                }
            }else {
                GJSON_PRINT("Prop(");
                GJSON_PRINTSTR(e->Data());
                GJSON_PRINTLINE(")");
            }
        }

        return false;
    }

protected:
    NullBuffer  jio_;                       ///< NULL buffer instance
}; // class JSONExampleObjectParse

#endif // !defined(GJSON_NO_EXAMPLES)


}; // namespace gjson

#endif // GJSON_PARSER_H
