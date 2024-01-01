/**
 * \file
 * Embedded JSON builder and parser - Support
 * PROJECT: GJSON library
 * TARGET SYSTEM: Arduino, STM32, Atmel AT91SAM, portable
 */

#ifndef GJSON_SUPPORT_H
#define GJSON_SUPPORT_H

namespace gjson {

/**
 * JSON support methods, mostly reusable string helpers
 */
class JSONSupport {
public:
    /**
     * Query is string a JSON literal null
     *
     * \param[in] s Pointer, null terminated check string
     * \param[in] l Length (characters) not including null
     * \return String query result
     */
    static bool IsNull(const char *s, const uint16_t l) {
        bool b = false;

        if ((4 == l) && !JSONSupport::StrNCmp(s, "null", 4)) {
            b = true;
        }

        return b;
    }


    /**
     * Query is string a JSON literal boolean
     *
     * \param[in] s Pointer, null terminated check string
     * \param[in] l Length (characters) not including null
     * \return String query result
     */
    static bool IsBoolean(const char *s, const uint16_t l) {
        bool b = false;

        if ((4 == l) && !JSONSupport::StrNCmp(s, "true", 4)) {
            b = true;
        }else if ((5 == l) && !JSONSupport::StrNCmp(s, "false", 5)) {
            b = true;
        }

        return b;
    }


    /**
     * Query is string numeric integer with optional leading zero control
     *
     * IsInt("00.12")      = 0,   IsInt(..., true)    = 0
     * IsInt("-00.12")     = 0,   IsInt(..., true)    = 0
     * IsInt("-012")       = 0,   IsInt(..., true)    = 0
     * IsInt("-00")        = 0,   IsInt(..., true)    = 0
     * IsInt("0.0")        = 0,   IsInt(..., true)    = 0
     * IsInt("-0.0")       = 0,   IsInt(..., true)    = 0
     * IsInt("0.1234")     = 0,   IsInt(..., true)    = 0
     * IsInt("1.455")      = 0,   IsInt(..., true)    = 0
     * IsInt("3455.455")   = 0,   IsInt(..., true)    = 0
     * IsInt("-0.1234")    = 0,   IsInt(..., true)    = 0
     * IsInt("-1.455")     = 0,   IsInt(..., true)    = 0
     * IsInt("-3455.455")  = 0,   IsInt(..., true)    = 0
     * IsInt("-1.2")       = 0,   IsInt(..., true)    = 0
     * IsInt("-012000009") = 0,   IsInt(..., true)    = 0
     * IsInt("000455")     = 0,   IsInt(..., true)    = 1
     * IsInt("00")         = 0,   IsInt(..., true)    = 1
     * IsInt("00000")      = 0,   IsInt(..., true)    = 1
     * IsInt("000001")     = 0,   IsInt(..., true)    = 1
     * IsInt("100000")     = 1,   IsInt(..., true)    = 1
     * IsInt("0")          = 1,   IsInt(..., true)    = 1
     * IsInt("-0")         = 1,   IsInt(..., true)    = 1
     * IsInt("-12000000")  = 1,   IsInt(..., true)    = 1
     * IsInt("-12000009")  = 1,   IsInt(..., true)    = 1
     *
     * \param[in] s Pointer, null terminated check string
     * \param[in] l Length (characters) not including null
     * \param[in] allow_leading Leading zero check control flag, default false(not allowed)
     * \return String query result
     */
    static bool IsInt(const char *s, const uint16_t l, bool allow_leading=false) {
        bool f = true;
        uint16_t    st = 0;

        if (l>1 && (s[0]=='-')) {
            st++;
            allow_leading = false;
        }
        for(uint16_t i=st; i<l; i++) {
            if (s[i]>='0' && s[i]<='9') {
                if (!allow_leading && i==st && '0'==s[i] && l>(1+st)) {
                    // leading zero
                    f = false;
                    break;
                }
            }else {
                f = false;
                break;
            }
        }

        return f;
    } // IsInt(...)


    /**
     * Query is string numeric decimal with optional exponent check
     *
     * IsDecimal("1.e-10",...)       = 0,  IsDecimal(...,false) = 0
     * IsDecimal("01.0e-10",...)     = 0,  IsDecimal(...,false) = 0
     * IsDecimal("0.000000e",...)    = 0,  IsDecimal(...,false) = 0
     * IsDecimal("1.0e-",...)        = 0,  IsDecimal(...,false) = 0
     * IsDecimal("1.0e+",...)        = 0,  IsDecimal(...,false) = 0
     * IsDecimal("1e10",...)         = 1,  IsDecimal(...,false) = 0
     * IsDecimal("0.000000e0",...)   = 1,  IsDecimal(...,false) = 0
     * IsDecimal("-0.000000e-0",...) = 1,  IsDecimal(...,false) = 0
     * IsDecimal("1.0e-10",...)      = 1,  IsDecimal(...,false) = 0
     * IsDecimal("1.0e+10",...)      = 1,  IsDecimal(...,false) = 0
     * IsDecimal("1.0e0",...)        = 1,  IsDecimal(...,false) = 0
     * IsDecimal("1.0e+0",...)       = 1,  IsDecimal(...,false) = 0
     * IsDecimal("0.0e-1",...)       = 1,  IsDecimal(...,false) = 0
     * IsDecimal("0.0e+1",...)       = 1,  IsDecimal(...,false) = 0
     * IsDecimal("0.0e0",...)        = 1,  IsDecimal(...,false) = 0
     * IsDecimal("0.0e+0",...)       = 1,  IsDecimal(...,false) = 0
     * IsDecimal("0.1e345",...)      = 1,  IsDecimal(...,false) = 0
     * IsDecimal("0.1e+345",...)     = 1,  IsDecimal(...,false) = 0
     * IsDecimal("0.1e-345",...)     = 1,  IsDecimal(...,false) = 0
     * IsDecimal("0",...)            = 1,  IsDecimal(...,false) = 1
     * IsDecimal("-0",...)           = 1,  IsDecimal(...,false) = 1
     * IsDecimal("0.0",...)          = 1,  IsDecimal(...,false) = 1
     * IsDecimal("-0.0",...)         = 1,  IsDecimal(...,false) = 1
     * IsDecimal("0.1234",...)       = 1,  IsDecimal(...,false) = 1
     * IsDecimal("-0.1234",...)      = 1,  IsDecimal(...,false) = 1
     * IsDecimal("1.455",...)        = 1,  IsDecimal(...,false) = 1
     * IsDecimal("-1.455",...)       = 1,  IsDecimal(...,false) = 1
     * IsDecimal("3455.455",...)     = 1,  IsDecimal(...,false) = 1
     * IsDecimal("-3455.455",...)    = 1,  IsDecimal(...,false) = 1
     * IsDecimal("3455",...)         = 1,  IsDecimal(...,false) = 1
     * IsDecimal("-3455",...)        = 1,  IsDecimal(...,false) = 1
     * IsDecimal("-1.2",...)         = 1,  IsDecimal(...,false) = 1
     * IsDecimal("1.2",...)          = 1,  IsDecimal(...,false) = 1
     * IsDecimal("-0.012",...)       = 1,  IsDecimal(...,false) = 1
     * IsDecimal("-0.000000",...)    = 1,  IsDecimal(...,false) = 1
     * IsDecimal("0.000000",...)     = 1,  IsDecimal(...,false) = 1
     *
     * \param[in] s Pointer, null terminated check string
     * \param[in] l Length (characters) not including null
     * \param[in] allow_exponent Exponent check control flag, default true(allow)
     * \return String query result
     */
    static bool IsDecimal(const char *s, const uint16_t l, const bool allow_exponent=true) {
        bool        f = true;
        bool        en = false;
        bool        dpnl = false, dpnr = false;
        uint16_t    st = 0, e = 0;    // e cannot be 0 if present
        uint16_t    dp = 0;            // dp cannot be 0 if present

        // format: [s]<n>[[dp]<N>][[e|E][S]<n>]
        //        s   = sign(-)
        //        S   = sign(+|-)
        //        n   = integer no leading zeros
        //        N   = integer with optional leading zeros
        //        dp  = decimal point
        //        e|E = exponent 'e'
        if (l>1 && (s[0]=='-')) {
            st++;
        }
        for(uint16_t i=st; i<l; i++) {
            if (!e) {
                if (s[i]>='0' && s[i]<='9') {
                    if (!dp && i>st && '0'==s[st]) {
                        // leading zero
                        f = false;
                        break;
                    }
                    if (!dp) {
                        dpnl = true;
                    }else {
                        dpnr = true;
                    }
                }else {
                    if (('.' == s[i]) && !dp) {
                        dp = i;
                    }else if (('E' == (s[i] & 223)) && ((dp==0 && dpnl) || (dp>0 && dpnr))) {   // nEn or n.nEn
                        e = i;
                    }else {
                        f = false;
                        break;
                    }
                }
            }else {
                if (s[i]>='0' && s[i]<='9') {
                    en=true;
                }else if ('-' == s[i] && i==(e+1)) {
                }else if ('+' == s[i] && i==(e+1)) {
                }else {
                    f = false;
                    break;
                }
            }
        }
        if (allow_exponent) {
            if (e && !en) {
                f = false;
            }
        }else {
            if (e) {
                f = false;
            }
        }

        return f;
    } // IsDecimal()


    /**
     * Query is string numeric hexadecimal with optional inclusion of prefix
     *
     * IsHex("Hello")      = 0,    IsHex(..., false)  = 0
     * IsHex("true")       = 0,    IsHex(..., false)  = 0
     * IsHex("false")      = 0,    IsHex(..., false)  = 0
     * IsHex("1.456")      = 0,    IsHex(..., false)  = 0
     * IsHex("-4.991")     = 0,    IsHex(..., false)  = 0
     * IsHex("-3456")      = 0,    IsHex(..., false)  = 0
     * IsHex("-04544")     = 0,    IsHex(..., false)  = 0
     * IsHex("0x23j330")   = 0,    IsHex(..., false)  = 0
     * IsHex("-00")        = 0,    IsHex(..., false)  = 0
     * IsHex("-0")         = 0,    IsHex(..., false)  = 0
     * IsHex("-12000000")  = 0,    IsHex(..., false)  = 0
     * IsHex("0x000000e")  = 1,    IsHex(..., false)  = 0
     * IsHex("0x002344ff") = 1,    IsHex(..., false)  = 0
     * IsHex("0xab203efe") = 1,    IsHex(..., false)  = 0
     * IsHex("0xdeadbeef") = 1,    IsHex(..., false)  = 0
     * IsHex("fffffff")    = 0,    IsHex(..., false)  = 1
     * IsHex("000000e")    = 0,    IsHex(..., false)  = 1
     * IsHex("9345")       = 0,    IsHex(..., false)  = 1
     * IsHex("000455")     = 0,    IsHex(..., false)  = 1
     * IsHex("100000")     = 0,    IsHex(..., false)  = 1
     * IsHex("00")         = 0,    IsHex(..., false)  = 1
     * IsHex("00000")      = 0,    IsHex(..., false)  = 1
     * IsHex("000001")     = 0,    IsHex(..., false)  = 1
     * IsHex("cafe")       = 0,    IsHex(..., false)  = 1
     * IsHex("0")          = 0,    IsHex(..., false)  = 1
     *
     * \param[in] s Pointer, null terminated check string
     * \param[in] l Length (characters) not including null
     * \param[in] prefix_0x Prefix check control flag, default true(allow "0x" as prefix)
     * \return String query result
     */
    static bool IsHex(const char *s, const uint16_t l, const bool prefix_0x=true) {
        bool f = true;
        uint16_t st = 0;

        if (prefix_0x) {
            if (l>2 && s[0] == '0' && s[1] == 'x') {
                st = 2;
            }else {
                f = false;
            }
        }
        if (f) {
            for(uint16_t i=st; i<l; i++) {
                if ((s[i]>='0' && s[i]<='9') || (s[i]>='A' && s[i]<='F')  || (s[i]>='a' && s[i]<='f')) {
                }else {
                    f = false;
                    break;
                }
            }
        }

        return f;
    } // IsHex(...)


    /**
     * Query is string JSON literal with optional exponent check.  Used during parsing to ensure 
     * value data is correctly represented as literals.
     *
     * \param[in] s Pointer, null terminated check string
     * \param[in] l Length (characters) not including null
     * \param[in] allow_exponent Exponent check control flag, default true(allow)
     * \return String query result
     */
    static bool IsLiteral(const char *s, const uint16_t l, const bool allow_exponent=true) {
        bool li = false;

        if (IsNull(s,l) || IsBoolean(s,l) || IsDecimal(s,l,allow_exponent)) {
            li = true;
        }

        return li;
    } // IsLiteral(...)


    /**
     * Query is string JSON literal decimal with exponent specified
     *
     * \param[in] s Pointer, null terminated check string
     * \param[in] l Length (characters) not including null
     * \return String query result
     */
    static bool HasLiteralExponent(const char* s, uint16_t l) {
        bool he = false;
        uint16_t i;

        // <s><n>[dp][e<S><n>]
        //           /|\
        //            Exponent
        if (JSONSupport::IsDecimal(s, l)) {
            for(i=0; i<l; i++) {
                if ('e' == s[i]) {
                    he = true;
                    break;
                }
            }
        }

        return he;
    } // HasLiteralExponent(...)


    /**
     * Character string d contains substring between given character indexes that should be replaced
     * by a null terminated string.
     *
     * \param[in] d Pointer, destination null terminated string
     * \param[in] d_length Actual length of destination string
     * \param[in] d_max Maximum possible length including null terminator of destination string
     * \param[in] sp Start position or character index into destionation string for replace
     * \param[in] ep End position or character index into destionation string for replace
     * \param[in] s Pointer, null terminated replacement string
     * \param[in] sl Length (characters) of replacement string not including null
     * \return New destination length (characters).  An update of d_length
     */
    static uint16_t Replace(char *d, uint16_t d_length, uint16_t d_max, int16_t sp, int16_t ep, const char* s, uint16_t sl) {
        uint16_t dl = ep - sp;
        uint16_t diff;

        // Valid start + end points?
        if (sp>=0 && ep>=0 && sp<d_length && ep<=d_length) {
            // Destination space same size as source (new string)
            if (dl == sl) {
                memcpy(static_cast<char*>(&d[sp]), s, sl);
            }else if (dl>sl) {
                // Destination space larger than source (new string)
                diff = dl - sl;
                memmove(static_cast<char*>(&d[ep - diff]),  static_cast<char*>(&d[ep]), d_length - (ep - diff));
                d_length -= diff;
                memcpy(static_cast<char*>(&d[sp]), s, sl);

            }else {
                // Destination space smaller than source (new string)
                
                // Will fit?
                diff = sl - dl;
                if (d_max >= (d_length + diff)) {
                    memmove(static_cast<char*>(&d[ep + diff]),  static_cast<char*>(&d[ep]), d_length - (ep - diff));
                    d_length += diff;
                    memcpy(static_cast<char*>(&d[sp]), s, sl);
                }
            }
        }

        return d_length;
    } // Replace(...)


    /**
     * Alternative from strncmpi and strncmp.  Some standard library implementations don't include
     *
     * \param[in] s1 String, NULL terminated
     * \param[in] s2 String, NULL terminated
     * \param[in] n Characters (length excluding NULL)
     * \param[in] case_sensitive Case sensitive compare flag (default true)
     * \return int Difference.  s1 == s2 => 0, s1 < s2 => -n, s2 > s2 => +n
     */
    static int StrNCmp(const char *s1, const char *s2, const int n, const bool case_sensitive=true) {
        int r = 0;

        for(int i=0; i<n; i++) {
            if ((case_sensitive && s1[i] != s2[i]) || (!case_sensitive && (s1[i] & 223) != (s2[i] & 223))) {
                r = -1;
                break;
            }
        }
        if (r==0) {
            if ((int)strlen(s1) > n) {
                r=1;
            }
        }

        return r;
    }

}; // class JSONSupport

}; // namespace gjson

#endif // GJSON_SUPPORT_H
