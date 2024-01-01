// json.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#define GJSON_NEW_OBJECT(o,t)               (new o); printf("n\n");
#define GJSON_RELEASE_OBJECT(o)             (delete o); printf("d\n");

#include "../gjson.h"


/**
 * Dump buffer as hexidecimal bytes or ASCII characters (where possible)
 *
 * \param[in] b Octet buffer
 * \param[in] l Buffer length (including any NULL terminator)
 * \param[in] ll Line length, characters, default 16
 */
uint32_t dump_buffer(const char *b, uint32_t l, uint8_t ll=16) {
    uint32_t cs = 0;

    for(uint16_t i=0, c=0; i<=l; i++, c++) {
        if (i==0 || c==16) {
            std::cout << std::endl << "[" << std::hex << std::setw(4) << std::setfill('0') << i << "]  ";
            c=0;
        }
        if (b[i]>=32 && b[i]<192) {
            std::cout << " " << b[i] << " ";
        }else {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << b[i] << " ";
        }
        cs += b[i];
    }
    std::cout << std::endl << std::dec;

    return cs;
}


using namespace gjson;


/**
 * Temporary use, global I/O buffer for JSON Parsing and building
 */
Buffer<256> jio;

struct CommandBar {
public:
	CommandBar() { Clear(); }

	/**
	 * Enum of possible LED (logical OR) bit states
	 *
	 * \note Value matters and must match set routine bit indexs
	 */
	typedef enum {
		LED_Y2	= 1,
		LED_R2	= 2,
		LED_G2	= 4,
		LED_G1	= 8,
	}LED_e;


	void Clear() {
		valid = false;
		boot = false;
		value = 0; 
	}


	template<uint8_t LENGTH>
	void Process(const gjson::JSONString<LENGTH>* last_property, const gjson::JSONString<LENGTH>* value, const uint8_t square_count, const uint8_t curly_count) {
		auto *str = value->Data();

		if (2 == curly_count) {
			if (value->IsBoolean() && *last_property == "boot") {
				// boot by itself doesnt make for valid
				boot = *value == "true" ? true : false;
			}else if (value->IsInt(true) && '-' != str[0]) {
				if (*last_property == "value") {
					valid = true;
					this->value = static_cast<uint8_t>(atoi(str));
				}else if (*last_property == "g1") {
					if ('0' == str[0]) {
						valid = true;
						this->value &= ~LED_G1;
					}else if ('1' == str[0]) {
						valid = true;
						this->value |= LED_G1;
					}
				}else if (*last_property == "g2") {
					if ('0' == str[0]) {
						valid = true;
						this->value &= ~LED_G2;
					}else if ('1' == str[0]) {
						valid = true;
						this->value |= LED_G2;
					}
				}else if (*last_property == "y2") {
					if ('0' == str[0]) {
						valid = true;
						this->value &= ~LED_Y2;
					}else if ('1' == str[0]) {
						valid = true;
						this->value |= LED_Y2;
					}
				}else if (*last_property == "r2") {
					if ('0' == str[0]) {
						valid = true;
						this->value &= ~LED_R2;
					}else if ('1' == str[0]) {
						valid = true;
						this->value |= LED_R2;
					}
				}else if (*last_property == "boot") {
					// boot by itself doesnt make for valid
					boot = '1' == str[0] ? true : false;
				}
			}
		}
	}


	bool valid;
	bool boot;
	uint8_t value;
}; // struct CommandBar


struct CommandVisible {
public:
	CommandVisible() { Clear(); }


	void Clear() {
		valid = false;
		state = true;
	}


	template<uint8_t LENGTH>
	void Process(const gjson::JSONString<LENGTH>* last_property, const gjson::JSONString<LENGTH>* value, const uint8_t square_count, const uint8_t curly_count) {
		auto *str = value->Data();

		if (1 == curly_count) {
			if (*last_property == "visible") {
				if (value->IsBoolean()) {
					valid = true;
					state = *value == "true" ? true : false;
				}else if (value->IsInt(true) && '-' != str[0]) {
					// UInt
					valid = true;
					state = '1' == str[0] ? true : false;
				}
			}
		}
	}


	bool valid;
	bool state;
}; // struct CommandVisible


struct CommandCfg {
public:
	CommandCfg() { Clear(); }


	void Clear() {
		valid = false;
		refresh.value = 0;
		refresh.set = false;
		m.value = 0.0f;
		m.set = false;
		x.value = 0.0f;
		x.set = false;
		c.value = 0.0f;
		c.set = false;
		reset = false;
	}


	template<uint8_t LENGTH>
	void Process(const gjson::JSONString<LENGTH>* last_property, const gjson::JSONString<LENGTH>* value, const uint8_t square_count, const uint8_t curly_count) {
		auto *str = value->Data();

		if (2 == curly_count) {
			if (*last_property == "refresh") {
				// UInt
				if (value->IsInt(true) && '-' != str[0]) {
					refresh.value = static_cast<uint8_t>(atoi(str));
					valid = refresh.set = true;
				}
			}else if (*last_property == "reset") {
				if (value->IsBoolean()) {
					reset = *value == "true" ? true : false;
				}else if (value->IsInt(true) && '-' != str[0]) {
					// UInt
					reset = '1' == str[0] ? true : false;
				}
			}else if (*last_property == "baud") {
				// UInt
				if (value->IsInt(true) && '-' != str[0]) {
					baud.value = static_cast<uint16_t>(atoi(str));
					valid = baud.set = true;
				}
			}else if (value->IsDecimal()) {
				// Float
				if (*last_property == "m") {
					m.value = static_cast<float>(strtod(str, NULL));
					valid = m.set = true;
				}else if (*last_property == "x") {
					x.value = static_cast<float>(strtod(str, NULL));
					valid = x.set = true;
				}else if (*last_property == "c") {
					c.value = static_cast<float>(strtod(str, NULL));
					valid = c.set = true;
				}
			}
		}
	}


	bool valid;
	struct {
		uint8_t value;
		bool set;
	}refresh;
	struct {
		uint16_t value;
		bool set;
	}baud;
	struct {
		float value;		///< temp = mx + c (calibration value subtract m from raw ADC sample)
		bool set;
	}m;
	struct {
		float value;		///< calibration value x
		bool set;
	}x;
	struct {
		float value;		///< calibration value c
		bool set;
	}c;
	bool reset;
}; // struct CommandCfg


struct CommandDOP {
public:
	CommandDOP() { Clear(); }


	void Clear() {
		valid = false;
		boot = false;
		op_idx = 0;
		op[0] = 0;
		op[1] = 0;
	}


	template<uint8_t LENGTH>
	void Process(const gjson::JSONString<LENGTH>* last_property, const gjson::JSONString<LENGTH>* value, const uint8_t square_count, const uint8_t curly_count) {
		auto *str = value->Data();

		if (square_count) {
			if (value->IsInt(true) && '-' != str[0]) {
				// UInt

				// Write next output.  These array elements have no index so internally we have to maintain where the next value should be written
				if (op_idx<sizeof(op)) {
					// Property name once at first index, check it and buffer data
					if (op_idx || (!op_idx && *last_property == "dop")) {
						op[op_idx++] = '1' == str[0] ? 1 : 0;
						if (2 == op_idx) {
							valid = true;
						}
					}
				}else {
					// To much data, invalid
					valid = false;
				}
			}
		}else {
			if (*last_property == "boot") {
				if (value->IsBoolean()) {
					boot = *value == "true" ? true : false;
				}else if (value->IsInt(true) && '-' != str[0]) {
					// UInt
					boot = '1' == str[0] ? true : false;
				}
			}
		}
	}


	/**
	 * Count of op[op_idx]
	 *
	 * \return Next index into this->op[], expect 0..2
	 */
	uint8_t GetOPCount() const {
		return op_idx;
	}


	bool valid;
	uint8_t op[2];
	bool boot;

protected:
	uint8_t op_idx;
}; // struct CommandDOP


struct CommandGet {
public:
	/**
	 * Enum of possible Get properties
	 */
	typedef enum {
		DATA_UNSET,
		DATA_DIP,
		DATA_DOP,
		DATA_TEMP,
		DATA_ALOG,			///< Both analogue inputs
		DATA_ALOG1,
		DATA_ALOG2,
	}DATA_e;


	CommandGet() { Clear(); }


	void Clear() {
		valid = false;
		degc = true;
		dp.valid = false;
		dp.value = 0;
		data = DATA_UNSET;
		// these calibration values are the defaults to InternalTemperature::GetBuffered
		m = 352.0f;
		x = 1.0f/(165.0f/128.0f);
		c = 25.0f;
		scale = 1.0;
		volt = 5.0f;
	}


	template<uint8_t LENGTH>
	void Process(const gjson::JSONString<LENGTH>* last_property, const gjson::JSONString<LENGTH>* value, const uint8_t square_count, const uint8_t curly_count) {
		auto *str = value->Data();

		if (!square_count) {
			if (*last_property == "degc") {
				if (value->IsBoolean()) {
					degc = *value == "true" ? true : false;
				}else if (value->IsInt(true) && '-' != str[0]) {
					// UInt
					degc = '1' == str[0] ? true : false;
				}
			}else if (*last_property == "m") {
				if (value->IsDecimal()) {
					m = static_cast<float>(strtod(str, NULL));
				}
			}else if (*last_property == "x") {
				if (value->IsDecimal()) {
					x = static_cast<float>(strtod(str, NULL));
				}
			}else if (*last_property == "c") {
				if (value->IsDecimal()) {
					c = static_cast<float>(strtod(str, NULL));
				}
			}else if (*last_property == "volt") {
				if (value->IsDecimal() && '-' != str[0] && value->Length() <= 4) {
					volt = static_cast<float>(atof(str));
				}
			}else if (*last_property == "scale") {
				if (value->IsDecimal() && '-' != str[0] && value->Length() <= 4) {
					scale = static_cast<float>(atof(str));
				}
			}else if (*last_property == "dp") {
				if (value->IsInt(true) && '-' != str[0] && value->Length() == 1) {
					// UInt, 1digit so must be 0-9, limit to 3
					dp.valid = true;
					dp.value = str[0] - '0';
					if (dp.value>3) {
						dp.value = 3;
					}
				}
			}else if (*last_property == "data") {
				if (*value == "dip") {
					data = DATA_DIP;
					valid = true;
				}else if (*value == "dop") {
					data = DATA_DOP;
					valid = true;
				}else if (*value == "temp") {
					data = DATA_TEMP;
					valid = true;
				}else if (*value == "alog") {
					data = DATA_ALOG;
					valid = true;
				}else if (*value == "alog1") {
					data = DATA_ALOG1;
					valid = true;
				}else if (*value == "alog2") {
					data = DATA_ALOG2;
					valid = true;
				}
			}
		}
	} // Process(...)


	bool valid;
	bool degc;				///< used only when data == DATA_TEMP
	uint8_t data;			///< DATA_e
	struct {
		uint8_t	value;		///< used only when data == DATA_ALOG or DATA_ALOG1 or DATA_ALOG2
		bool valid;			///< dp flag, true when dp specified so no dp and dp=0 can be detected
	}dp;
	float m;				///< temperature = mx + c (calibration value subtract m from raw ADC sample)
	float x;				///< calibration value x for temperature
	float c;				///< calibration value c for temperature
	float scale;			///< scale for adc
	float volt;				///< voltage for adc
}; // struct CommandGet


struct CommandEE {
public:
	CommandEE() { Clear(); }


	void Clear() {
		valid = false;
		offset = 0;
		size = 1;
		memset(data, 0, sizeof(data));
		set_idx = 0;
	}


	bool IsGetOperation() const {
		bool get = false;

		// Did they specify size but gave no data => a GET otherwise SET operation
		if (size && !set_idx) {
			get = true;
		}

		return get;
	}


	template<uint8_t LENGTH>
	void Process(const gjson::JSONString<LENGTH>* last_property, const gjson::JSONString<LENGTH>* value, const uint8_t square_count, const uint8_t curly_count) {
		auto *str = value->Data();

		if (square_count) {
			if (value->IsInt(true) && '-' != str[0]) {
				// UInt

				// Write next data.  These array elements have no index so internally we have to maintain where the next value should be written
				if (set_idx<sizeof(data)) {
					// Property name once at first index, check it and buffer data
					if (set_idx || (!set_idx && *last_property == "data")) {
						data[set_idx++] = static_cast<uint8_t>(atoi(str));
						size = set_idx;
					}
				}else {
					valid = false;
					size = 0; // To much data (will be invalid)
				}
			}
		}else {
			if (*last_property == "size") {
				// Only accept if set_idx=0 => GET operation
				if (value->IsInt(true) && '-' != str[0] && value->Length() == 1 && !set_idx) {
					// UInt, 1digit. Must be 1-8
					size = str[0] - '0';
					if (!size || size>8) {
						size = 1;
					}
				}
			}else if (*last_property == "offset") {
				if (value->Length() && value->Length() <= 3 && value->IsInt() && '-' != str[0] && size) {
					valid = true;
					offset = static_cast<uint8_t>(atoi(str));
				}
			}
		}
	} // Process(...)


	bool valid;
	uint8_t size;			///< 1>= Size (Bytes) <= 8
	uint8_t data[8];		///< Octets to write
	uint8_t	offset;			///< Offset

protected:
	uint8_t set_idx;
}; // struct CommandEE


/**
 * Set display segments
 */
struct CommandSegment {
public:
	CommandSegment() { Clear(); }


	void Clear() {
		valid = false;
		set_idx = 0;
		boot = false;
		memset(data, 0, sizeof(data));
	}


	template<uint8_t LENGTH>
	void Process(const gjson::JSONString<LENGTH>* last_property, const gjson::JSONString<LENGTH>* value, const uint8_t square_count, const uint8_t curly_count) {
		auto *str = value->Data();

		if (square_count) {
			if (value->IsInt(true) && '-' != str[0]) {
				// UInt

				// Write next data.  These array elements have no index so internally we have to maintain where the next value should be written
				if (set_idx<sizeof(data)) {
					// Property name once at first index, check it and buffer data
					if (set_idx || (!set_idx && *last_property == "data")) {
						data[set_idx++] = static_cast<uint8_t>(atoi(str));
						if (set_idx==sizeof(data)) {
							valid = true;
						}
					}
				}else {
					valid = false; // To much data
				}
			}
		}else {
			if (*last_property == "boot") {
				if (value->IsBoolean()) {
					boot = *value == "true" ? true : false;
				}else if (value->IsInt(true) && '-' != str[0]) {
					// UInt
					boot = '1' == str[0] ? true : false;
				}
			}
		}
	}


	bool valid;
	bool boot;
	uint8_t data[7];

protected:
	uint8_t set_idx;
}; // struct CommandSegment


/**
 * Set display segments, pattern
 */
/**
 * A structure used to maintain parsed JSON specific to pattern data
 *
 * \tparam PATTERN_MAX Pattern symbols maximum available used for buffer and control
 */
template<uint8_t PATTERN_MAX>
struct CommandPattern {
public:
	CommandPattern() { Clear(); }


	void Clear() {
		valid = false;
		set_idx = 0;
		memset(data, 0, sizeof(data));
		boot = false;
	}


	template<uint8_t LENGTH>
	void Process(const gjson::JSONString<LENGTH>* last_property, const gjson::JSONString<LENGTH>* value, const uint8_t square_count, const uint8_t curly_count) {
		auto *str = value->Data();

		if (square_count) {
			if (value->IsInt(true) && '-' != str[0]) {
				// UInt

				// Write next data.  These array elements have no index so internally we have to maintain where the next value should be written
				if (set_idx<sizeof(data)) {
					// Property name once at first index, check it and buffer data
					if (set_idx || (!set_idx && *last_property == "data")) {
						data[set_idx++] = static_cast<uint8_t>(atoi(str));
						valid = true;
					}
				}else {
					valid = false; // To much data
				}
			}
		}else {
			if (*last_property == "boot") {
				if (value->IsBoolean()) {
					boot = *value == "true" ? true : false;
				}else if (value->IsInt(true) && '-' != str[0]) {
					// UInt
					boot = '1' == str[0] ? true : false;
				}
			}
		}
	}


	/**
	 * Count of data[set_idx]
	 *
	 * \return Next index into this->data[]
	 */
	uint8_t GetDataCount() const {
		return set_idx;
	}


	bool valid;
	uint8_t data[PATTERN_MAX];
	bool boot;

protected:
	uint8_t set_idx;
}; // struct CommandPattern


/**
 * A structure used to maintain parsed JSON specific to LED display shown data
 *
 * \tparam DIGITS Display N digits
 */
template<uint8_t DIGITS>
struct CommandShow {
public:
	CommandShow() { Clear(); }


	/**
	 * Enum of possible types
	 */
	typedef enum {
		TYPE_UNSET,			///< Must be 0 (first in list)
		TYPE_RAW,
		TYPE_NUMERIC,
		TYPE_PATTERN,
		TYPE_TEMP,
		TYPE_ALOG0,
		TYPE_ALOG1,
	}TYPE_e;


	void Clear() {
		valid = false;
		boot = false;
		degc = true;
		type = TYPE_UNSET;
		digits = 0;
		dp = 0;
		volt = 0.0f;
		scale = 0.0f;
		memset(data, 0, sizeof(data));
		scroll.valid = false;
		scroll.left = false;
		scroll.delay = 0;
		pattern.valid = false;
		pattern.inc = false;
		pattern.digit_inc = false;
		pattern.delay = 0;
		pattern.final_delay = 0;
		set_idx = 0;
	}


	template<uint8_t LENGTH>
	void Process(const gjson::JSONString<LENGTH>* last_property, const gjson::JSONString<LENGTH>* value, const uint8_t square_count, const uint8_t curly_count) {
		auto *str = value->Data();

		if (square_count) {
			if (value->IsInt(true) && '-' != str[0]) {
				// UInt

				// Write next data.  These array elements have no index so internally we have to maintain where the next value should be written
				if (set_idx<sizeof(data)-1) {
					// Property name once at first index, check it and buffer data
					if (set_idx || (!set_idx && *last_property == "data")) {
						uint8_t chr = static_cast<uint8_t>(atoi(str));
						
						if (!chr) {
							chr=' ';	// The low level driver converts ASCII and spaces get coverted into 0's but allow further characters as strings used for animation buffer
						}
						data[set_idx++] = chr;
						valid = true;
						type = TYPE_RAW;
					}
				}else {
					valid = false; // To much data
				}
			}
		}else {
			bool flag;

			// Handle bootleans
			if (value->IsBoolean()) {
				flag = *value == "true" ? true : false;
			}else if (value->IsInt(true) && '-' != str[0]) {
				// UInt
				flag = '1' == str[0] ? true : false;
			}

			if (2 == curly_count) {
				if (*last_property == "boot") {
					boot = flag;
				}else if (*last_property == "degc") {
					degc = flag;
				}else if (*last_property == "digits") {
					if (value->IsInt(true) && '-' != str[0] && value->Length() == 1) {
						// UInt, 1digit so must be 0-DIGITS
						digits = str[0] - '0';
						if (digits > DIGITS) {
							digits = DIGITS;
						}
					}
				}else if (*last_property == "dp") {
					if (value->IsInt(true) && '-' != str[0] && value->Length() == 1) {
						// UInt, 1digit so must be 0-9, limit to 3
						dp = str[0] - '0';
						if (dp>3) {
							dp = 3;
						}
					}
				}else if (*last_property == "volt") {
					if (value->IsDecimal() && '-' != str[0] && value->Length() <= 4) {
						volt = static_cast<float>(atof(str));
					}
				}else if (*last_property == "scale") {
					if (value->IsDecimal() && '-' != str[0] && value->Length() <= 4) {
						scale = static_cast<float>(atof(str));
					}
				}else if (*last_property == "data") {
					if (*value == "pattern") {
						// Show pattern
						type = TYPE_PATTERN;
						valid = true;
					}else if (*value == "temp") {
						// Show internal temperature sensor
						type = TYPE_TEMP;
						valid = true;
					}else if (*value == "alog0") {
						// Show external analogue voltage (ADC0) 
						type = TYPE_ALOG0;
						valid = true;
					}else if (*value == "alog1") {
						// Show external analogue voltage (ADC1) 
						type = TYPE_ALOG1;
						valid = true;
					}else if (value->IsInt(true) || value->IsHex(false)) {
						// Show numeric data to set as string
						if (value->Length() < sizeof(data)) {
							set_idx = static_cast<uint8_t>(value->Length());
							memcpy(data, str, 1 + set_idx); // 1+ to include NULL
							type = TYPE_NUMERIC;
							valid = true;
						}
					}
				}
			}else if (3 == curly_count) {
				if (!scroll.valid && *last_property == "left") {
					scroll.valid = true;
					scroll.left = flag;
				}else if (*last_property == "digitinc") {
					pattern.digit_inc = flag;
				}else if (*last_property == "interval") {
					// The very simplistic implementation of parsing here means our keys have to be different so scroll keys cannot duplicate cycle keys - doh!
					if (value->Length() && value->Length() <= 4 && value->IsInt(true) && '-' != str[0]) {
						scroll.valid = true;
						scroll.delay = static_cast<uint16_t>(atoi(str));
						// Larger values make little sense, even 1000ms is overly generous
						if (scroll.delay > 1000) {
							scroll.delay = 1000;
						}
					}
				}else if (!pattern.valid && *last_property == "inc") {
					pattern.valid = true;
					pattern.inc = flag;
				}else if (*last_property == "final") {
					// The very simplistic implementation of parsing here means our keys have to be different so scroll keys cannot duplicate cycle keys - doh!
					// Final delay is a strange side affect of animation buffer, it is kind of part of scroll, doesn't mean much to pattern
					if (value->Length() && value->Length() <= 5 && value->IsInt(true) && '-' != str[0]) {
						pattern.final_delay = static_cast<uint16_t>(atoi(str));
						if (pattern.final_delay > 10000) {
							pattern.final_delay = 10000;
						}
					}
				}else if (*last_property == "delay") {
					// The very simplistic implementation of parsing here means our keys have to be different so scroll keys cannot duplicate cycle keys - doh!
					if (value->Length() && value->Length() <= 3 && value->IsInt(true) && '-' != str[0]) {
						uint16_t v = static_cast<uint16_t>(atoi(str));
						// Must fit our type, again though larger vales make little sense
						if (v>0 && v<=255) {
							pattern.delay = static_cast<uint8_t>(v);
							pattern.valid = true;
						}
					}
				}
			}
		}
	} /* Process(...) */


	/**
	 * Count of data[set_idx]
	 *
	 * \return Next index into this->data[]
	 */
	uint8_t GetDataCount() const {
		return set_idx;
	}


	bool valid;
	bool boot;
	bool degc;
	uint8_t type;
	uint8_t digits;
	uint8_t dp;
	float volt;
	float scale;
	char data[DIGITS+1];		///< Display data +1 for NULL so can be handled as string
	struct {
		bool valid;				///< Indicates scroll included in message
		bool left;				///< Left or right flag
		uint16_t delay;			///< Milliseconds
	}scroll;
	struct {
		bool valid;				///< Indicates pattern included in message
		bool inc;				///< Patterns direction.  True implies pattern[0..n] otherwise [n...0]
		bool digit_inc;			///< Pattern change per digit following 'inc' direction
		uint8_t delay;			///< Milliseconds
		uint16_t final_delay;	///< Final (last frame) delay, Milliseconds (optional)
	}pattern;

protected:
	uint8_t set_idx;
}; // struct CommandShow





/*
{visible:<true/false>}
{"bar": {"g1":<0/1>, "g2":<0/1>, "y2":<0/1>, "r2":<0/1>}, boot:<true/false>}
{"bar": n, boot:<true/false>}
{show: { data:[n,...], boot:<true/false>, scroll: { "left": "true|false", timer: 40 }, pattern: { "inc": "true|false, delay: 5}}
{show: { data:"pattern", digits:n, boot:<true/false>, scroll: { "left": "true|false", timer: 40}, pattern: { "inc": "true|false, delay: 5}}
{show: { data:"temp", degc:<true/false>, boot:<true/false>, scroll: { "left": "true|false", timer: 40}, pattern: { "inc": "true|false, delay: 5}}
{show: { data:"alog0", volt: 5, scale: x, dp:0, boot:<true/false>, scroll: { "left": "true|false"}, pattern: { "inc": "true|false, delay: 5}}
{show: { data:"alog1", volt: 5, scale: x, dp:2, boot:<true/false>, scroll: { "left": "true|false"}, pattern: { "inc": "true|false, delay: 5}}
{show: { data:"01234567", boot:<true/false>, scroll: { "from": "left|right"}, pattern: { "inc": "true|false, delay: 5}}
{pattern: { "data": [n,...] } }
{segment: { "data":[n,...], boot:<true/false>}
{"get": { "data":"temp", degc:<true/false>}}
{"get": { "data":"dip" }}
{"get": { "data":"dop" }}
{"get": { "data":"alog<n>", "format": ["int"|"float"|"raw", ...]}}
{"dop": [op0, op1], boot:<true/false>}
{"cfg": {"refresh":"<rate>", "tempcal":n}}

bool digit_use;
char digit[7];

bool bar_use;
uint8_t bar;

bool show_use;
bool show_mode;		// custom, temp, adc, adcx
bool show_state;	// show:bool default true

#define JSON_STRLEN		12
*/

#if defined(GJSON_PACKING)
#pragma pack(push, GJSON_PACKING)
#endif


/**
 * A custom parser class based of gjson::JSONParse for parsing and building LED display JSON
 *
 * \tparam LENGTH JSON string length maximum characters including NULL
 * \tparam DIGITS Display N digits used for buffering
 * \tparam PATTERN_MAX Pattern symbols maximum available used for buffer and control
 */
template<uint8_t LENGTH, uint8_t DIGITS, uint8_t PATTERN_MAX>
class JSONDisplayStringParse : protected gjson::JSONParse<LENGTH> {
public:
    using gjson::JSONParse<LENGTH>::ERR;
    using gjson::JSONParse<LENGTH>::GetLastError;
    using gjson::JSONParse<LENGTH>::LimitNodeLevel;
    using gjson::JSONParse<LENGTH>::FromObject;
	using gjson::JSONParse<LENGTH>::Callbacks;

	/* JSON root information */
	typedef enum {
		ROOT_TYPE_UNKNOWN,
		ROOT_TYPE_VISIBLE,
		ROOT_TYPE_SHOW,
		ROOT_TYPE_BAR,
		ROOT_TYPE_PATTERN,
		ROOT_TYPE_GET,
		ROOT_TYPE_DOP,
		ROOT_TYPE_CFG,
		ROOT_TYPE_EE,
	}ROOT_TYPE_e;

	ROOT_TYPE_e root;
	char root_string[LENGTH];

	/**
	 * Commands handled
	 */
	CommandBar bar;
	CommandVisible visible;
	CommandCfg cfg;
	CommandDOP dop;
	CommandGet get;
	CommandPattern<PATTERN_MAX> pattern;
	CommandShow<DIGITS> show;
	CommandEE ee;

    /**
     * Default constructor, make stable instance.  Setup NULL I/O, base parser and enable callbacks
     */
	JSONDisplayStringParse(gjson::IO* io) : gjson::JSONParse<LENGTH>(io, 0U) {
		Callbacks(true);
		Clear();
    }


	/**
	 * Clear JSON decode
	 */
	void Clear() {
		root = ROOT_TYPE_UNKNOWN;
		strcpy(root_string, "unknown");
		last_property_.Length(0);
		bar.Clear();
		visible.Clear();
		cfg.Clear();
		dop.Clear();
		get.Clear();
		pattern.Clear();
		show.Clear();
		ee.Clear();
		bracket.square = bracket.curly = 0;
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
		return gjson::JSONParse<LENGTH>::FromString(s, NULL);
    }


    /*
     * See JSONTokenizer::TokenArray.  Here we simply bracket count and clear last property on close
     */
    bool TokenArray(const gjson::JSONArray<LENGTH>* e, char *s=NULL, uint16_t i=0) {
        // String parsing?
        if (gjson::JSONConstant::OP_FROM_STRING == gjson::JSONParse<LENGTH>::GetLastOperation()) {
            if (e->IsOpen()) {
				bracket.square++;
            }else {
				bracket.square--;
				last_property_.Length(0);
            }
        }

        return false;
    }


    /*
     * \copydoc JSONTokenizer::TokenMap.  Here we simply bracket count and clear last property on close
     */
    bool TokenMap(const gjson::JSONArray<LENGTH>* e, char *s=NULL, uint16_t i=0) {
        // String parsing?
        if (gjson::JSONConstant::OP_FROM_STRING == gjson::JSONParse<LENGTH>::GetLastOperation()) {
            if (e->IsOpen()) {
				bracket.curly++;
            }else {
				bracket.curly--;
				last_property_.Length(0);
            }
        }

        return false;
    }


    /*
     * \copydoc JSONTokenizer::TokenString.  Store property and decode on values
     */
    bool TokenString(const gjson::JSONString<LENGTH>* e, char *s=NULL, int16_t si=0, int16_t ei=0) {
        // String parsing?
        if (gjson::JSONConstant::OP_FROM_STRING == gjson::JSONParse<LENGTH>::GetLastOperation()) {
			if (e->IsValue()) {
				// Not required but safer to check

				// By not checking bracket.curly or bracket.square we risk collecting data from some other source but this isnt to important for this application
				if (ROOT_TYPE_VISIBLE == root) {
					visible.Process<LENGTH>(&last_property_, e, bracket.square, bracket.curly);
				}else if (ROOT_TYPE_BAR == root) {
					bar.Process<LENGTH>(&last_property_, e, bracket.square, bracket.curly);
				}else if (ROOT_TYPE_CFG == root) {
					cfg.Process<LENGTH>(&last_property_, e, bracket.square, bracket.curly);
				}else if (ROOT_TYPE_DOP == root) {
					// We take {<key>:<data>} or [data, data, data] hence the length + bracket.square check
					dop.Process<LENGTH>(&last_property_, e, bracket.square, bracket.curly);
				}else if (ROOT_TYPE_GET == root) {
					// We take {<key>:<data>} or [data, data, data] hence the length + bracket.square check
					get.Process<LENGTH>(&last_property_, e, bracket.square, bracket.curly);
				}else if (ROOT_TYPE_PATTERN == root) {
					// We take {<key>:<data>} or [data, data, data] hence the length + bracket.square check
					pattern.template Process<LENGTH>(&last_property_, e, bracket.square, bracket.curly);
				}else if (ROOT_TYPE_SHOW == root) {
					// We take {<key>:<data>} or [data, data, data] hence the length + bracket.square check
					show.template Process<LENGTH>(&last_property_, e, bracket.square, bracket.curly);
				}else if (ROOT_TYPE_EE == root) {
					// We take {<key>:<data>} or [data, data, data] hence the length + bracket.square check
					ee.Process<LENGTH>(&last_property_, e, bracket.square, bracket.curly);
				}

				last_property_.Length(0);
            }else {
				// Root?
				if (ROOT_TYPE_UNKNOWN == root) {
					if (*e == "visible") {
						root = ROOT_TYPE_VISIBLE;
					}else if (*e == "show") {
						root = ROOT_TYPE_SHOW;
					}else if (*e == "bar") {
						root = ROOT_TYPE_BAR;
					}else if (*e == "pattern") {
						root = ROOT_TYPE_PATTERN;
					}else if (*e == "get") {
						root = ROOT_TYPE_GET;
					}else if (*e == "dop") {
						root = ROOT_TYPE_DOP;
					}else if (*e == "cfg") {
						root = ROOT_TYPE_CFG;
					}else if (*e == "ee") {
						root = ROOT_TYPE_EE;
					}
					if (ROOT_TYPE_UNKNOWN != root) {
						strcpy(root_string, e->Data());
					}
				}else {
					// Acceptable nested
				}
				last_property_ = *e;
            }
        }

        return false;
    } /* TokenString(...) */


protected:
	struct {
		uint8_t		square;		///< Square count
		uint8_t		curly;		///< Curly count
	}bracket;
	gjson::JSONString<LENGTH>	last_property_;
}; // class JSONDisplayStringParse


#if defined(GJSON_PACKING)
#pragma pack(pop)
#endif


/**
 * Application specific buffered JSON serial IO command handler
 *
 * \tparam BL Maximum buffering length in characters (including any NULL terminator)
 * \tparam LENGTH Maximum length (including NULL terminator) in characters of a JSON string attribute/property
 * \tparam DIGITS Display N digits used for buffering
 * \tparam PATTERN_MAX Pattern symbols maximum available used for buffer and control
 */
template<uint16_t BL, uint8_t LENGTH, uint8_t DIGITS, uint8_t PATTERN_MAX>
class JSONBufferedSerialIO {
public:
	char buffer[BL];											///< Raw JSON string I/O buffer
	JSONDisplayStringParse<LENGTH, DIGITS, PATTERN_MAX> json;	///< Received, parsed JSON complete with decoded command class instance(s).  Use json.root


	JSONBufferedSerialIO() : usr_buffer_(buffer, BL), json(&usr_buffer_) { }


	bool IsError() const {
		return error_;
	}


	bool IsResponse() const {
		return response_;
	}


	/**
	 * Parse input JSON within buffer.  May produce response JSON error string
	 */
	bool Parse() {
		error_ = response_ = false;
		usr_buffer_.Length(strlen(buffer));
		json.Clear();
		json.FromString(buffer);
		if (json.GetLastError() != gjson::JSONConstant::ERR_NONE) {

			// Build JSON parse error response.  Temporarily  use response JSON buffer to convert error numeric
			auto a = gjson::JSONMap<LENGTH>();                                     // Map(o) >
			auto b = gjson::JSONMapTupleMap<LENGTH>(json.root_string, &a);      // String("<json_root_str>") > Map(o)
			auto c = gjson::JSONMapTuple<LENGTH>("error", static_cast<uint8_t>(json.GetLastError()), "%u", &b, true);             // String("error") > String("<error_str>") >
			auto d = gjson::JSONMap<LENGTH>(&c,false);                             // Map(c) >
			auto e = gjson::JSONMap<LENGTH>(&d,false);                             // Map(c)

			json.Callbacks(false); // Required for Object to string, not sure why??  but the callbacks are used for input parsing
			error_ = json.FromObject(reinterpret_cast<gjson::JSONElement<LENGTH>*>(&a));
			json.Callbacks(true);
			if (!error_) {
				BuildStringErrorResponse(json.GetLastError());
			}
			error_ = response_ = true;
		}

		return !error_;
	}


	/* Responses:
	 *	bar			(1) or no response
	 *	visible		(1) or no response
	 *	cfg			(1) or no response
	 *	dop			(1) or no response
	 *	pattern		(1) or no response
	 *	get			(1) or {"get":{ "data": [x] } } or {"get":{ "data": [x, y] } }
	 *	show		(1) or {"show": { "state": "busy" } } unless force specified or able to show
	 *
	 * (1) Parse error.  {"<root">: { "error": "n"} } where n is parse error numeric
	 *     Parameter error.  {"<root">: { "error": "<param name>"} } 
	 *     Build error.  {"<root">: { "error": "n", "build":true } } 
	 */
	void BuildShowResponse() {
		// Build JSON response
		auto a = gjson::JSONMap<LENGTH>();											// Map(o) >
		auto b = gjson::JSONMapTupleMap<LENGTH>(json.root_string, &a);			// String("<json_root_str>") > Map(o)
		auto c = gjson::JSONMapTuple<LENGTH>("state", "busy", &b);					// String("state") > String("busy") >
		auto d = gjson::JSONMap<LENGTH>(&c,false);									// Map(c) >
		auto e = gjson::JSONMap<LENGTH>(&d,false);									// Map(c)

		json.Callbacks(false); // Required for Object to string, not sure why??  but the callbacks are used for input parsing
		error_ = json.FromObject(reinterpret_cast<gjson::JSONElement<LENGTH>*>(&a));
		json.Callbacks(true);
		if (!error_) {
			BuildStringErrorResponse(json.GetLastError());
			error_ = true;
		}
		response_ = true;
	}


// "{"get": {"data":"alog1", value:[0]"} }
// "{"get": {"data":"alog2", value:[0]"} }
// "{"get": {"data":"alog", value:[0, 0]"} }
// "{"get": {"data":"temp", value:[1.455]"} }
// "{"get": {"data":"temp", value:[1023]"} }
// "{"get": {"data":"dip", value:[0, 0]"} }
// "{"get": {"data":"dop", value:[0, 0]"} }
//	template<typename T>
	void BuildGetResponse(const char* data_type, gjson::JSONElement<LENGTH>* data_numeric1, gjson::JSONElement<LENGTH>* data_numeric2=NULL) {
		auto a = gjson::JSONMap<LENGTH>();											// Map(o) >
		auto b = gjson::JSONMapTupleMap<LENGTH>(json.root_string, &a);				// String(<json.root_string>) > Map(o)
		auto c = gjson::JSONMapTuple<LENGTH>("data", data_type, &b);				// String("data") > String("<data_type>") >
		auto d = gjson::JSONMapTupleArray<LENGTH>("value", &c);						// String("value") > Array(o)

		auto e = gjson::JSONArray<LENGTH>(false);									// Map(c) >
		auto f = gjson::JSONMap<LENGTH>(&e,false);									// Map(c) >
		auto g = gjson::JSONMap<LENGTH>(&f,false);									// Map(c)

		// Either [data_numeric1] or [data_numeric1, data_numeric2]
		d.Next(data_numeric1);
		if (!data_numeric2) {
			data_numeric1->Next(&e);
		}else {
			data_numeric1->Next(data_numeric2);
			data_numeric2->Next(&e);
		}
		bool cbu = json.Callbacks();
		json.Callbacks(false);	// Callbacks are not required for object to string, not sure why??  but the callbacks are used for string parsing
		error_ = json.FromObject(reinterpret_cast<gjson::JSONElement<LENGTH>*>(&a));
		json.Callbacks(cbu);	// Restore
		if (!error_) {
			BuildStringErrorResponse(json.GetLastError());
			error_ = true;
		}
		response_ = true;
	}
	

	// data[8] even if length < 8
	void BuildEEResponse(const uint8_t offset, const uint8_t *data, const uint8_t length) {
		// Build JSON response
		auto a = gjson::JSONMap<LENGTH>();											// Map(o) >
		auto b = gjson::JSONMapTupleMap<LENGTH>(json.root_string, &a);				// String("<json_root_str>") > Map(o)
		auto c = gjson::JSONMapTuple<LENGTH, uint8_t>("offset", offset, "%u", &b);	// String("offset") > String(offset) 
		auto d = gjson::JSONMapTupleArray<LENGTH>("data", &c);						// String("<json_root_str>") > Array(o)

		// data e[]

		auto f = gjson::JSONArray<LENGTH>(false);									// Array(NULL, c) >
		auto g = gjson::JSONMap<LENGTH>(&f,false);									// Map(c) >
		auto h = gjson::JSONMap<LENGTH>(&g,false);									// Map(c)

		gjson::JSONString<LENGTH,uint8_t> e[] = {
			gjson::JSONString<LENGTH,uint8_t>(data[0], "%u", &d, true, true),
			gjson::JSONString<LENGTH,uint8_t>(data[1], "%u", &e[0], true, true),
			gjson::JSONString<LENGTH,uint8_t>(data[2], "%u", &e[1], true, true),
			gjson::JSONString<LENGTH,uint8_t>(data[3], "%u", &e[2], true, true),
			gjson::JSONString<LENGTH,uint8_t>(data[4], "%u", &e[3], true, true),
			gjson::JSONString<LENGTH,uint8_t>(data[5], "%u", &e[4], true, true),
			gjson::JSONString<LENGTH,uint8_t>(data[6], "%u", &e[5], true, true),
			gjson::JSONString<LENGTH,uint8_t>(data[7], "%u", &e[6], true, true)
		};
		e[length-1].Next(&f);

		bool cbu = json.Callbacks();
		json.Callbacks(false);	// Callbacks are not required for object to string, not sure why??  but the callbacks are used for string parsing
		error_ = json.FromObject(reinterpret_cast<gjson::JSONElement<LENGTH>*>(&a));
		json.Callbacks(cbu);	// Restore
		response_ = true;
	}

protected:
	void BuildStringErrorResponse(gjson::JSONConstant::ERR error) {
		char error_str[8];

		if (strlen(json.root_string)) {
			strcpy(buffer, "{\"");
			strcat(buffer, json.root_string);
		}else {
			strcpy(buffer, "{\"unknown");
		}
		strcat(buffer, "\":{ \"error\":\"");
		itoa(static_cast<uint8_t>(error), error_str, 10);
		strcat(buffer, error_str);
		strcat(buffer, "\", \"build\":true } }");
		usr_buffer_.Length(strlen(buffer));
	}

protected:
	bool	error_;					///< Parse error flag.  This covers both received errors and transmit (both are handled outside of this class)
	bool	response_;				///< Response flag, set when JSON buffer contains response
	gjson::UserBuffer usr_buffer_;	///< JSON string I/O buffer
}; // class JSONBufferedSerialIO



/**
 * Main entry point into test harness application
 *
 * \param[in] argc Shell argument count
 * \param[in] argv Shell argument array of string pointers
 */
int _tmain(int argc, _TCHAR* argv[])
{
    uint32_t test = 1;
#if 0
   {
        auto a = JSONArray<16>();                        // Array(o) >
        auto b = JSONString<16,float>(1.260f, "%.2f", &a);            // String("name") >
        auto c = JSONString<16,float>(1.260f, "%.2f", &b, true, false);            // String("name") >
        auto d = JSONString<16>("pineapple", &c);        // String("name") >
        auto e = JSONString<16>("pear", &d);             // String("james") >
        auto f = JSONArray<16>(&e,false);                // Array(c)
        char *check_js = "[ \"apple\", \"orange\", \"pineapple\", \"pear\" ]";

		printf("result = %d\n", d.Data<uint8_t>(100,"0x%x", false));

        std::cout << "Test: " << test++ << ", Object to string" << std::endl;
        JSONParse<16> pr(&jio);
        bool err = pr.FromObject(reinterpret_cast<JSONElement<16>*>(&a));
        if (!err) {
            std::cout << "Last error: " << pr.GetLastError() << std::endl;
        }else {
            std::cout << "Length: " << pr.Length() << std::endl;
            std::cout << "Output: " << pr.Get() << std::endl;
            if (!strcmp(check_js, pr.Get())) {
                std::cout << "Test passed" << std::endl << std::endl;
            }else {
                std::cout << "Test failed" << std::endl << std::endl;
                return 1;
            }
        }

        std::cout << "Test: " << test++ << ", Custom object parse" << std::endl;
        JSONExampleObjectParse<16> cpr;
        err = cpr.FromObject(reinterpret_cast<gjson::JSONElement<16>*>(&a));
        if (!err) {
            std::cout << "Last error: " << pr.GetLastError() << std::endl;
            return 1;
        }
        std::cout << "Test passed" << std::endl << std::endl;
		return 0;
    }
#endif// 0
	{
//		char js[] = "{\"bar\": { \"g2\":\"1\", \"g1\":1, \"r2\":1, \"y2\":1, \"boot\":1}}";
//		char js[] = "{\"bar\": { \"g2\":\"1\" }}";
//		char js[] = "{\"bar\": { \"y2\":1 }}";
//		char js[] = "{\"bar\": { \"g1\":1, \"r2\":1 }}";
//		char js[] = "{\"bar\": { \"value\":15, \"boot\":1} }";
//		char js[] = "{\"bar\": { \"value\":0} }";
//		char js[] = "{\"bar\": { \"value\":5} }";
//		char js[] = "{\"visible\":true}";
//		char js[] = "{\"visible\":1}";
//		char js[] = "{\"visible\":\"1\"}";
//		char js[] = "{\"visible\":\"0\"}";
//		char js[] = "{\"cfg\": {\"refresh\":\"20\"}}";
//		char js[] = "{\"cfg\": {\"refresh\":\"20\", \"x\":0.766467}}";
//		char js[] = "{\"cfg\": {\"refresh\":\"20\", \"x\":0.766467, \"reset\":true}}";
//		char js[] = "{\"cfg\": {\"refresh\":\"20\", \"x\":0.766467, \"reset\":1}}";
//		char js[] = "{\"cfg\": {}}";
//		char js[] = "{\"cfg\": {\"ignore\":null}}";
//		char js[] = "{\"cfg\": {\"x\":26}}";
//		char js[] = "{\"cfg\": {\"m\":355.0, \"x\":0.766467, \"c\":27.1}}";
//		char js[] = "{\"cfg\": {\"tempcal\":100}}";
//		char js[] = "{\"cfg\": {\"tempcal\":100, \"baud\":9600}}\r\n";
//		char js[] = "{\"dop\": [\"0\", \"1\"], \"boot\":true}";
//		char js[] = "{\"dop\": [1, 0], \"boot\":false}";
//		char js[] = "{\"dop\": [1, 1]}";
//		char js[] = "{\"dop\": [0, 0], \"boot\":\"1\"}";
//		char js[] = "{\"get\": {\"data\":\"alog\", \"format\":[\"raw\",\"int\"]}}";
//		char js[] = "{\"get\": {\"data\":\"alog\"}}";
//		char js[] = "{\"get\": {\"data\":\"alog\", \"dp\":0}}";
//		char js[] = "{\"get\": {\"data\":\"alog\", \"dp\":1}}";
//		char js[] = "{\"get\": {\"data\":\"alog\", \"dp\":1, \"volt\":3.3, \"scale\":0.8}}";
//		char js[] = "{\"get\": {\"data\":\"temp\", \"degc\":true, \"m\":355.0, \"x\":0.766467, \"c\":25.0}}";
//		char js[] = "{\"get\": {\"data\":\"temp\", \"degc\":\"0\"}}";
//		char js[] = "{\"get\": {\"data\":\"dip\"}}";
//		char js[] = "{\"get\": {\"data\":\"dop\"}}";
//		char js[] = "{\"get\": {\"data\":\"alog1\"}}";
//		char js[] = "{\"get\": {\"data\":\"alog1\", \"format\":\"raw\"}}";
//		char js[] = "{\"get\": {\"data\":\"alog2\", \"format\":\"int\"}}";
//		char js[] = "{\"pattern\": {\"data\":[1]}}";
//		char js[] = "{\"pattern\": {\"data\":[1,2,3,4,5,6,7,8,9,10]}}";
//		char js[] = "{\"show\": {\"data\":[1,2,3,4]}}";
//		char js[] = "{\"show\": {\"data\":[1,2,3,4,5,6,7]}}";
//		char js[] = "{\"show\": {\"data\":\"1234567\"}}";
//		char js[] = "{\"show\": {\"data\":\"-123456\"}}";
//		char js[] = "{\"show\": {\"data\":\"234\", \"boot\":true}}";
//		char js[] = "{\"show\": {\"data\":\"1234567\"}}";
//		char js[] = "{\"show\": {\"data\":\"234\", \"boot\":true, \"scroll\": { \"left\":false, \"timer\":40}, \"pattern\": { \"inc\":1, \"delay\":4}}}";
//		char js[] = "{\"show\": {\"data\":\"temp\", \"degc\":false, \"dp\":1, \"boot\":true, \"scroll\": { \"left\":false, \"timer\":40}, \"pattern\": { \"inc\":1, \"delay\":4}}}";
//		char js[] = "{\"show\": {\"data\":\"temp\", \"degc\":false, \"dp\":1, \"boot\":true, \"scroll\": { \"left\":false, \"timer\":40}, \"pattern\": { \"inc\":1, \"delay\":4}}}";
//		char js[] = "{\"show\": {\"data\":\"alog0\", \"scale\":1.0, \"volt\": 5.0, \"dp\":1, \"boot\":true, \"scroll\": { \"left\":false, \"timer\":40}, \"pattern\": { \"inc\":1, \"delay\":4}}}";
//		char js[] = "{\"show\": {\"data\":\"pattern\", \"digits\":4, \"boot\":true, \"scroll\": { \"left\":false, \"timer\":40}, \"pattern\": { \"inc\":1, \"delay\":4}}}";
//		char js[] = "{\"show\": {\"data\":\"pattern\", \"digits\":4, \"boot\":true, \"scroll\": { \"left\":false, \"timer\":40}, \"pattern\": { \"inc\":1, \"delay\":4, \"digitinc\":true, \"final\":500 } } }";
//		char js[] = "{\"ee\": {\"data\":[1,2,3,4,5,6]}}";
//		char js[] = "{\"ee\": {\"offset\": 0, \"data\":[1,2,3,4,5,6,7,8]}}";
//		char js[] = "{\"ee\": {\"offset\": 0, \"data\":[255]}}";
//		char js[] = "{\"ee\": {\"offset\": 0, \"data\":[]}}";
//		char js[] = "{\"ee\": {\"offset\": 0, \"size\": 6, \"data\":[1,2,3]}}";
//		char js[] = "{\"ee\": {\"offset\": 0, \"data\":[1,2,3], \"size\": 1}}";
//		char js[] = "{\"ee\": {\"offset\": 511}}";
//		char js[] = "{\"ee\": {\"offset\": 0, \"size\":4}}";
//		char js[] = "{\"ee\": {\"offset\": 64, \"size\": 5}}";

/*
{show: { data:[n,...], boot:<true/false>, scroll: { "left": "true|false", timer: 40 }, pattern: { "inc": "true|false, delay: 5}}
{show: { data:"pattern", digits:n, boot:<true/false>, scroll: { "left": "true|false", timer: 40}, pattern: { "inc": "true|false, delay: 5}}
{show: { data:"temp", degc:<true/false>, boot:<true/false>, scroll: { "left": "true|false", timer: 40}, pattern: { "inc": "true|false, delay: 5}}
{show: { data:"alog0", volt: 5, scale: x, dp:0, boot:<true/false>, scroll: { "left": "true|false"}, pattern: { "inc": "true|false, delay: 5}}
{show: { data:"alog1", volt: 5, scale: x, dp:2, boot:<true/false>, scroll: { "left": "true|false"}, pattern: { "inc": "true|false, delay: 5}}
{show: { data:"01234567", boot:<true/false>, scroll: { "from": "left|right"}, pattern: { "inc": "true|false, delay: 5}}
*/

//		std::cout << js << std::endl << std::endl;

#if 0
	    JSONDisplayStringParse<12> dpr(&jio);
		uint16_t fci = dpr.FromString(js);
		if (dpr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << dpr.GetLastError() << std::endl;
		    return 1;
		}
#endif//0

		JSONBufferedSerialIO<256, 12, 7, 16> jserial;

		for(uint8_t cmds=0; cmds<3; cmds++) {
			char js[256];

			if (cmds==0) {
//				strcpy(js, "{\"ee\": {\"thiswillbreakmyscriptswontit?\": 64, \"size\": 5}}");
//				strcpy(js, "{\"get\":{\"data\":\"\"}}\n");
				strcpy(js, "{\"bar\":{\"value\":15, \"boot\":1}}");
		//		strcpy(js,"{\"show\": {\"data\":\"temp\", \"degc\":false, \"dp\":1, \"boot\":true, \"scroll\": { \"left\":false, \"timer\":40}, \"pattern\": { \"inc\":1, \"delay\":4}}}");
			}else if (cmds==1) {
				strcpy(js,"{\"ee\": {\"offset\": 0, \"size\":4}}");
			}else if (cmds==2) {
//				strcpy(js,"{\"get\": {\"data\":\"alog\"}}");
				strcpy(js,"{\"cfg\":{\"reset\":1}}");
//				strcpy(js,"{\"get\":{\"data"\:\"temp\", \"degc\":true}}");
			}

			std::cout << js << std::endl << std::endl;

			strcpy(jserial.buffer, js);
		jserial.Parse();

		if (jserial.IsError() && jserial.IsResponse()) {
			std::cout << "Response:" << std::endl;
			std::cout << jserial.buffer << std::endl;
			return 1;
		}

		std::cout << "Root: " << jserial.json.root_string << std::endl;

		switch(jserial.json.root) {
			case jserial.json.ROOT_TYPE_UNKNOWN:
				std::cout << "show.type ROOT_TYPE_UNKNOWN" << std::endl;
				break;
			case jserial.json.ROOT_TYPE_VISIBLE:
				std::cout << "show.type ROOT_TYPE_VISIBLE" << std::endl;
				break;
			case jserial.json.ROOT_TYPE_SHOW:
				std::cout << "show.type ROOT_TYPE_SHOW" << std::endl;
				break;
			case jserial.json.ROOT_TYPE_BAR:
				std::cout << "show.type ROOT_TYPE_BAR" << std::endl;
				break;
			case jserial.json.ROOT_TYPE_PATTERN:
				std::cout << "show.type ROOT_TYPE_PATTERN" << std::endl;
				break;
			case jserial.json.ROOT_TYPE_GET:
				std::cout << "show.type ROOT_TYPE_GET" << std::endl;
				break;
			case jserial.json.ROOT_TYPE_DOP:
				std::cout << "show.type ROOT_TYPE_DOP" << std::endl;
				break;
			case jserial.json.ROOT_TYPE_CFG:
				std::cout << "show.type ROOT_TYPE_CFG" << std::endl;
				break;
			case jserial.json.ROOT_TYPE_EE:
				std::cout << "show.type ROOT_TYPE_EE" << std::endl;
				break;
		}

		if (jserial.json.show.valid) {
			std::cout << "show.valid " << jserial.json.show.valid << std::endl;
			std::cout << "show.boot " << jserial.json.show.boot << std::endl;
			switch(jserial.json.show.type) {
				case jserial.json.show.TYPE_UNSET:
					std::cout << "show.type TYPE_UNSET" << std::endl;
					break;
				case jserial.json.show.TYPE_RAW:
					std::cout << "show.type TYPE_RAW" << std::endl;
					for(uint8_t x=0; x<jserial.json.show.GetDataCount(); x++) {
						std::cout << "show.data[" << (uint32_t)x << "] " << (uint32_t)jserial.json.show.data[x] << std::endl;
					}
					break;
				case jserial.json.show.TYPE_NUMERIC:
					std::cout << "show.type TYPE_NUMERIC" << std::endl;
					std::cout << "show.data " << jserial.json.show.data << std::endl;
					break;
				case jserial.json.show.TYPE_TEMP:
					std::cout << "show.type TYPE_TEMP" << std::endl;
					std::cout << "show.degc " << jserial.json.show.degc << std::endl;
					std::cout << "show.digits " << (uint32_t)jserial.json.show.digits << std::endl;
					std::cout << "show.dp " << (uint32_t)jserial.json.show.dp << std::endl;
					break;
				case jserial.json.show.TYPE_PATTERN:
					std::cout << "show.type TYPE_PATTERN" << std::endl;
					break;
				case jserial.json.show.TYPE_ALOG0:
					std::cout << "show.type TYPE_ALOG0" << std::endl;
					std::cout << "show.digits " << (uint32_t)jserial.json.show.digits << std::endl;
					std::cout << "show.dp " << (uint32_t)jserial.json.show.dp << std::endl;
					std::cout << "show.scale " << jserial.json.show.scale << std::endl;
					std::cout << "show.volt " << jserial.json.show.volt << std::endl;
				break;
				case jserial.json.show.TYPE_ALOG1:
					std::cout << "show.type TYPE_ALOG1" << std::endl;
					std::cout << "show.digits " << (uint32_t)jserial.json.show.digits << std::endl;
					std::cout << "show.dp " << (uint32_t)jserial.json.show.dp << std::endl;
					std::cout << "show.scale " << jserial.json.show.scale << std::endl;
					std::cout << "show.volt " << jserial.json.show.volt << std::endl;
					break;
			}
			if (jserial.json.show.pattern.valid && jserial.json.show.pattern.delay) {
				std::cout << "show.pattern.inc " << jserial.json.show.pattern.inc << std::endl;
				std::cout << "show.pattern.digitinc " << jserial.json.show.pattern.digit_inc << std::endl;
				std::cout << "show.pattern.timer " << (uint32_t)jserial.json.show.pattern.delay << std::endl;
				if (jserial.json.show.pattern.final_delay) {
					std::cout << "show.pattern.final_timer " << jserial.json.show.pattern.final_delay << std::endl;
				}
			}
			if (jserial.json.show.scroll.valid) {
				std::cout << "show.scroll.left " << jserial.json.show.scroll.left << std::endl;
				std::cout << "show.scroll.timer " << (uint32_t)jserial.json.show.scroll.delay << std::endl;
			}
		}
		if (jserial.json.bar.valid) {
			std::cout << "bar.valid " << jserial.json.bar.valid << std::endl;
			std::cout << "bar.value " << (uint32_t)jserial.json.bar.value << std::endl;
			std::cout << "bar.boot " << jserial.json.bar.boot << std::endl;
		}
		if (jserial.json.visible.valid) {
			std::cout << "visible.valid " << jserial.json.visible.valid << std::endl;
			std::cout << "visible.state " << jserial.json.visible.state << std::endl;
		}
		if (jserial.json.cfg.valid) {
			std::cout << "cfg.valid " << jserial.json.cfg.valid << std::endl;
			if (jserial.json.cfg.refresh.set) {
				std::cout << "cfg.refresh " << (uint32_t)jserial.json.cfg.refresh.value << std::endl;
			}
			if (jserial.json.cfg.baud.set) {
				std::cout << "cfg.baud " << (uint32_t)jserial.json.cfg.baud.value << std::endl;
			}
			if (jserial.json.cfg.m.set) {
				std::cout << "cfg.m " << jserial.json.cfg.m.value << std::endl;
			}
			if (jserial.json.cfg.x.set) {
				std::cout << "cfg.x " << jserial.json.cfg.x.value << std::endl;
			}
			if (jserial.json.cfg.c.set) {
				std::cout << "cfg.c " << jserial.json.cfg.c.value << std::endl;
			}
			std::cout << "cfg.reset " << jserial.json.cfg.reset << std::endl;

//			jserial.BuildCfgResponse(true);
		}
		if (jserial.json.dop.valid) {
			std::cout << "dop.valid " << jserial.json.dop.valid << std::endl;
			std::cout << "dop.op[0] " << (uint32_t)jserial.json.dop.op[0] << std::endl;
			std::cout << "dop.op[1] " << (uint32_t)jserial.json.dop.op[1] << std::endl;
			std::cout << "dop.boot " << jserial.json.dop.boot << std::endl;
		}
		if (jserial.json.get.valid) {
			std::cout << "get.valid " << jserial.json.get.valid << std::endl;
			std::cout << "get.data " << (uint32_t)jserial.json.get.data << std::endl;
			if (jserial.json.get.DATA_ALOG == jserial.json.get.data || jserial.json.get.DATA_ALOG1 == jserial.json.get.data || jserial.json.get.DATA_ALOG2 == jserial.json.get.data) {
				if (jserial.json.get.dp.valid) {
					std::cout << "get converted volts" << std::endl;
					std::cout << "get.dp " << (uint32_t)jserial.json.get.dp.value << std::endl;
					std::cout << "get.scale " << jserial.json.get.scale << std::endl;
					std::cout << "get.volt " << jserial.json.get.volt << std::endl;
				}else {
					std::cout << "get raw adc" << std::endl;
				}
			}else if (jserial.json.get.DATA_TEMP == jserial.json.get.data) {
				if (jserial.json.get.dp.valid) {
					std::cout << "get converted temp." << std::endl;
					std::cout << "get.degc " << jserial.json.get.degc << std::endl;
					std::cout << "get.dp " << (uint32_t)jserial.json.get.dp.value << std::endl;
				}else {
					std::cout << "get raw temp." << std::endl;
				}
				std::cout << "get.m " << jserial.json.get.m << std::endl;
				std::cout << "get.x " << jserial.json.get.x << std::endl;
				std::cout << "get.c " << jserial.json.get.c << std::endl;
			}

			auto a0 = JSONString<12,uint32_t>(-1025, "%d", true, true);
			auto a1 = JSONString<12,float>(2.610f, "%.1f", true, true);

			printf("%%.%uf\n",2);

			jserial.BuildGetResponse("alog", &a0, &a1);
			//jserial.BuildGetResponse("22.234", "28.7");
		}
		if (jserial.json.pattern.valid) {
			std::cout << "pattern.valid " << jserial.json.pattern.valid << std::endl;
			for(uint8_t x=0; x<jserial.json.pattern.GetDataCount(); x++) {
				std::cout << "pattern.data[" << (uint32_t)x << "] " << (uint32_t)jserial.json.pattern.data[x] << std::endl;
			}
		}

		if (jserial.json.ee.valid) {
			std::cout << "ee.valid " << jserial.json.ee.valid << std::endl;
			std::cout << "ee.offset " << (uint32_t)jserial.json.ee.offset << std::endl;
			std::cout << "ee.size " << (uint32_t)jserial.json.ee.size << std::endl;
			if (!jserial.json.ee.IsGetOperation()) {
				std::cout << "ee => Is SET" << std::endl;
				for(uint8_t x=0; x<jserial.json.ee.size; x++) {
					std::cout << "ee.data[" << (uint32_t)x << "] " << (uint32_t)jserial.json.ee.data[x] << std::endl;
				}
			}else {
				std::cout << "ee => Is GET" << std::endl;

				for(uint8_t x=0; x<jserial.json.ee.size; x++) {
					jserial.json.ee.data[x] = x + 1;
				}
				jserial.BuildEEResponse(jserial.json.ee.offset, jserial.json.ee.data, jserial.json.ee.size);
			}
		}

		if (jserial.IsResponse()) {
			std::cout << "Response:" << std::endl;
			std::cout << jserial.buffer << std::endl;
		}else {
			std::cout << "No response" << std::endl;
		}

		}
		return 1;
	}

    // [ "apple", "orange", "pineapple", "pear" ]
    {
        auto a = JSONArray<16>();                        // Array(o) >
        auto b = JSONString<16>("apple", &a);            // String("name") >
        auto c = JSONString<16>("orange", &b);           // String("dave") >
        auto d = JSONString<16>("pineapple", &c);        // String("name") >
        auto e = JSONString<16>("pear", &d);             // String("james") >
        auto f = JSONArray<16>(&e,false);                // Array(c)
        char *check_js = "[ \"apple\", \"orange\", \"pineapple\", \"pear\" ]";

        std::cout << "Test: " << test++ << ", Object to string" << std::endl;
        JSONParse<16> pr(&jio);
        bool err = pr.FromObject(reinterpret_cast<JSONElement<16>*>(&a));
        if (!err) {
            std::cout << "Last error: " << pr.GetLastError() << std::endl;
        }else {
            std::cout << "Length: " << pr.Length() << std::endl;
            std::cout << "Output: " << pr.Get() << std::endl;
            if (!strcmp(check_js, pr.Get())) {
                std::cout << "Test passed" << std::endl << std::endl;
            }else {
                std::cout << "Test failed" << std::endl << std::endl;
                return 1;
            }
        }

        std::cout << "Test: " << test++ << ", Custom object parse" << std::endl;
        JSONExampleObjectParse<16> cpr;
        err = cpr.FromObject(reinterpret_cast<gjson::JSONElement<16>*>(&a));
        if (!err) {
            std::cout << "Last error: " << pr.GetLastError() << std::endl;
            return 1;
        }
        std::cout << "Test passed" << std::endl << std::endl;
    }

    // { "name":"dave", "id":345 }
    {
        auto a = JSONMap<16>();                          // Map(o) >
        auto b = JSONMapTuple<16>("name", "dave", &a);   // String("name") > String("dave") >
        auto c = JSONMapTuple<16>("id", "345", &b, true);// String("id") > String("345", literal) >
        auto d = JSONMap<16>(&c,false);                  // Map(c)
        char *check_js = "{ \"name\":\"dave\", \"id\":345 }";

        std::cout << "Test: " << test++ << ", Object to string" << std::endl;
        JSONParse<16> pr(&jio);
        bool err = pr.FromObject(reinterpret_cast<JSONElement<16>*>(&a));
        if (!err) {
            std::cout << "Last error: " << pr.GetLastError() << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Length: " << pr.Length() << std::endl;
        std::cout << "Output: " << pr.Get() << std::endl;
        if (!strcmp(check_js, pr.Get())) {
            std::cout << "Test passed" << std::endl << std::endl;
        }else {
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
    }

    // [ { "aaa":"bbb", "ccc":"ddd" } ]
    {
        auto a = JSONArray<6>();                         // Array(o) >
        auto b = JSONMap<6>(&a);                         // Map(o) >
        auto c = JSONMapTuple<6>("aaa", "bbb", &b);      // String("aaa") > String("bbb") >
        auto d = JSONMapTuple<6>("ccc", "ddd", &c);      // String("ccc") > String("ddd") >
        auto e = JSONMap<6>(&d,false);                   // Map(c) >
        auto f = JSONArray<6>(&e,false);                 // Array(c)
        char *check_js = "[ { \"aaa\":\"bbb\", \"ccc\":\"ddd\" } ]";

        std::cout << "Test: " << test++ << ", Object to string" << std::endl;
        JSONParse<6> pr(&jio);
        bool err = pr.FromObject(reinterpret_cast<JSONElement<6>*>(&a));
        if (!err) {
            std::cout << "Last error: " << pr.GetLastError() << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Length: " << pr.Length() << std::endl;
        std::cout << "Output: " << pr.Get() << std::endl;
        if (!strcmp(check_js, pr.Get())) {
            std::cout << "Test passed" << std::endl << std::endl;
        }else {
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
    }

    // { "a":{ "b":"c" } }
    {
        auto a = JSONMap<10>();                          // Map(o) >
        auto b = JSONMapTupleMap<10>("a", &a);           // String("a") > Map(o)
        auto c = JSONMapTuple<10>("b", "c", &b);         // String("b") > String("c") >
        auto d = JSONMap<10>(&c,false);                  // Map(c) >
        auto e = JSONMap<10>(&d,false);                  // Map(c)
        char *check_js = "{ \"a\":{ \"b\":\"c\" } }";

        std::cout << "Test: " << test++ << ", Object to string" << std::endl;
        JSONParse<10> pr(&jio);
        bool err = pr.FromObject(reinterpret_cast<JSONElement<10>*>(&a));
        if (!err) {
            std::cout << "Last error: " << pr.GetLastError() << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Length: " << pr.Length() << std::endl;
        std::cout << "Output: " << pr.Get() << std::endl;
        if (!strcmp(check_js, pr.Get())) {
            std::cout << "Test passed" << std::endl << std::endl;
        }else {
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
    }

    // { "item":"computer", "id":1234 }
    {
        char js[48];
        UserBuffer ubjio(js, sizeof(js));
        gjson::JSONParse<12> pr(&ubjio);    // Maximum 0B object chain can be generated - Not used in this example

        auto a = JSONMap<12>();                          // Map(o) >
        auto b = JSONString<12>("item", &a, false);      // String("item", attr) >
        auto c = JSONString<12>("computer", &b);         // String("computer", value) >
        auto d = JSONString<12>("id", &c, false);        // String("id", attr) >
        auto e = JSONString<12>("1234", &d, true, true); // String("1234", value, literal) >
        auto f = JSONMap<12>(&e,false);                  // Map(c)

        std::cout << "Test: " << test++ << ", Object to string" << std::endl;
        bool err = pr.FromObject(&a);
        if (!err) {
            std::cout << "Last error: " << pr.GetLastError() << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Length: " << pr.Length() << std::endl;
            std::cout << "Output: " << pr.Get() << std::endl;
        }
        std::cout << "Test passed" << std::endl << std::endl;
    }

    // { "a":{ "b":"c", "d":"e" }, "f":"g" }
    {
        auto a = JSONMap<10>();                          // Map(o) >
        auto b = JSONMapTupleMap<10>("a", &a);           // String("a") > Map(o) >
        auto c = JSONMapTuple<10>("b", "c", &b);         // String("b") > String("c") >
        auto d = JSONMapTuple<10>("d", "e", &c);         // String("d") > String("e") >
        auto e = JSONMap<10>(&d,false);                  // Map(c) >
        auto f = JSONMapTuple<10>("f", "g", &e);         // String("f") > String("g") >
        auto g = JSONMap<10>(&f,false);                  // Map(c)
        char *check_js = "{ \"a\":{ \"b\":\"c\", \"d\":\"e\" }, \"f\":\"g\" }";

        std::cout << "Test: " << test++ << ", Object to string" << std::endl;
        JSONParse<10> pr(&jio);
        bool err = pr.FromObject(reinterpret_cast<JSONElement<10>*>(&a));
        if (!err) {
            std::cout << "Last error: " << pr.GetLastError() << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Length: " << pr.Length() << std::endl;
        std::cout << "Output: " << pr.Get() << std::endl;
        if (!strcmp(check_js, pr.Get())) {
            std::cout << "Test passed" << std::endl << std::endl;
        }else {
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
    }

    // [ { "a":[ [ [ "b", { "c":"d" } ] ] ] } ]
    {
        auto a = JSONArray<5>();                         // Array(o) >
        auto b = JSONMap<5>(&a);                         // Map(o) >
        auto c = JSONMapTupleArray<5>("a", &b);          // String("a") > Array(o) >
        auto d = JSONArray<5>(&c);                       // Array(o) >
        auto e = JSONArray<5>(&d);                       // Array(o) >
        auto f = JSONString<5>("b", &e);                 // String("b") >
        auto g = JSONMap<5>(&f);                         // Map(o) >
        auto h = JSONMapTuple<5>("c", "d", &g);          // String("c") > String("d") >
        auto i = JSONMap<5>(&h,false);                   // Map(c)
        auto j = JSONArray<5>(&i,false);                 // Array(c)
        auto k = JSONArray<5>(&j,false);                 // Array(c)
        auto l = JSONArray<5>(&k,false);                 // Array("a", c)
        auto m = JSONMap<5>(&l,false);                   // Map(c)
        auto n = JSONArray<5>(&m,false);                 // Array(c)
        char *check_js = "[ { \"a\":[ [ [ \"b\", { \"c\":\"d\" } ] ] ] } ]";

        std::cout << "Test: " << test++ << ", Object to string" << std::endl;
        JSONParse<5> pr(&jio);
        bool err = pr.FromObject(reinterpret_cast<JSONElement<5>*>(&a));
        if (!err) {
            std::cout << "Last error: " << pr.GetLastError() << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Length: " << pr.Length() << std::endl;
        std::cout << "Output: " << pr.Get() << std::endl;
        if (!strcmp(check_js, pr.Get())) {
            std::cout << "Test passed" << std::endl << std::endl;
        }else {
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
    }

    // [ { "a" : { "b" : "c", "d" : "e" }, "f" : "g", "h" : { "i" : "j" }, "k" : [ "l" ] } ]
    {
        auto a = JSONArray<5>();                         // Array(o) >
        auto b = JSONMap<5>(&a);                         // Map(o) >
        auto c = JSONMapTupleMap<5>("a", &b);            // String("a") > Map(o) >
        auto d = JSONMapTuple<5>("b", "c", &c);          // String("b") > String("c") >
        auto e = JSONMapTuple<5>("d", "e", &d);          // String("d") > String("e") >
        auto f = JSONMap<5>(&e,false);                   // Map("a", c) >
        auto g = JSONMapTuple<5>("f", "g", &f);          // String("f") > String("g") >
        auto h = JSONMapTupleMap<5>("h", &g);            // String("h") > Map(o) >
        auto i = JSONMapTuple<5>("i", "j", &h);          // String("i") > String("j") >
        auto j = JSONMap<5>(&i,false);                   // Map("h", c) >
        auto k = JSONMapTupleArray<5>("k", &j);          // String("k") > Array(o) >
        auto l = JSONString<5>("l", &k);                 // String("l") >
        auto m = JSONArray<5>(&l,false);                 // Array("k", c) >
        auto n = JSONMap<5>(&m,false);                   // Map("a", c) >
        auto o = JSONArray<5>(&n,false);                 // Array(c)
        char *check_js = "[ { \"a\":{ \"b\":\"c\", \"d\":\"e\" }, \"f\":\"g\", \"h\":{ \"i\":\"j\" }, \"k\":[ \"l\" ] } ]";

        std::cout << "Test: " << test++ << ", Object to string" << std::endl;
        JSONParse<5> pr(&jio);
        bool err = pr.FromObject(reinterpret_cast<JSONElement<5>*>(&a));
        if (!err) {
            std::cout << "Last error: " << pr.GetLastError() << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Length: " << pr.Length() << std::endl;
        std::cout << "Output: " << pr.Get() << std::endl;
        if (!strcmp(check_js, pr.Get())) {
            std::cout << "Test passed" << std::endl << std::endl;
        }else {
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
    }

    {
        // Expect something like: { "jam":[ "raspberry", "strawberry" ], "marmalade" : [ "orange", "lemon", "lime" ] }
        auto a = gjson::JSONMap<12>();                               // Map(o) >
        auto b = gjson::JSONMapTupleArray<12>("jam", &a);            // String("jam") > Map(o) >
        auto c = gjson::JSONString<12>("raspberry", &b);             // String("raspberry") >
        auto d = gjson::JSONString<12>("strawberry", &c);            // String("strawberry") >
        auto e = gjson::JSONArray<12>(&d,false);                     // Array(c)
        auto f = gjson::JSONMapTupleArray<12>("marmalade", &e);      // String("marmalade") > Array(o) >
        auto g = gjson::JSONString<12>("orange", &f);                // String("orange") >
        auto h = gjson::JSONString<12>("lemon", &g);                 // String("orange") >
        auto i = gjson::JSONString<12>("lime", &h);                  // String("lime") >
        auto j = gjson::JSONArray<12>(&i,false);                     // Array(c)
        auto k = gjson::JSONMap<12>(&j,false);                       // Map(c)

        std::cout << "Test: " << test++ << ", Attribute find, iterate over values" << std::endl;
        std::cout << "Find string(s)" << std::endl;

        // Find attribute marmalade
        JSONParse<12> pr(&jio);
        auto mm = pr.FindString(&a, "marmalade", false);
        if (pr.GetLastError() != JSONConstant::ERR_NONE || NULL==mm) {
            std::cout << "Last error: " << pr.GetLastError() << std::endl;
            return 1;
        }
        std::cout << "Attribute string " << mm->Data() << std::endl;

        // Iterate over value strings after above find
        int ct = 0;
        do {
            auto ns = pr.NextString(mm);

            if (ns) {
                std::cout << "Value string: " << ns->Data() << std::endl;

                mm = ns;
                ct++;
            }else {
                break;
            }
        }while(1);

        if (ct != 3) {
            std::cout << "Failed to find all strings" << std::endl;
            std::cout << "Last error: " << pr.GetLastError() << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }

        std::cout << "Test passed" << std::endl << std::endl;
    }

    {
        // Expect something like: [ "apple", "orange", "pineapple", "another" : [ "ORAnge" ], "pear" ]
        auto a = gjson::JSONArray<12>();                            // Array(o) >
        auto b = gjson::JSONString<12>("apple", &a);                // String("name") >
        auto c = gjson::JSONString<12>("orange", &b);               // String("dave") >
        auto d = gjson::JSONString<12>("pineapple", &c);            // String("name") >
        auto e = gjson::JSONMapTupleArray<12>("another", &d);       // String("another") > Map(o) >
        auto f = gjson::JSONString<12>("ORAnge", &e);               // String("dave") >
        auto g = gjson::JSONArray<12>(&f,false);                    // Array(c)
        auto h = gjson::JSONString<12>("pear", &g);                 // String("james") >
        auto i = gjson::JSONArray<12>(&h,false);                    // Array(c)

        std::cout << "Test: " << test++ << ", Object find, twice, same characteristics" << std::endl;
        std::cout << "Find string(s)" << std::endl;
        JSONParse<12> pr(&jio);

        auto dd = pr.FindString(&a, "orange", true, false);
        if (pr.GetLastError() != JSONConstant::ERR_NONE || NULL==dd) {
            std::cout << "Last error: " << pr.GetLastError() << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Find value string " << dd->Data() << std::endl;

        if (dd->Next()) {
            auto ee = pr.FindString(dd->Next(), "orange", true, false);
            if (pr.GetLastError() != JSONConstant::ERR_NONE || NULL==dd) {
                std::cout << "Last error: " << pr.GetLastError() << std::endl;
                std::cout << "Test failed" << std::endl << std::endl;
                return 1;
            }
            std::cout << "Find value string " << ee->Data() << std::endl;

            if (ee->Next()) {
                auto ac = pr.NextArray(dd->Next(), false);
                if (!ac) {
                    std::cout << "Last error: " << pr.GetLastError() << std::endl;
                    std::cout << "Test failed" << std::endl << std::endl;
                    return 1;
                }
            }else {
                std::cout << "Expected ee->Next() != NULL" << std::endl;
                std::cout << "Test failed" << std::endl << std::endl;
                return 1;
            }

        }else {
            std::cout << "Expected dd->Next() != NULL" << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }

        std::cout << "Test passed" << std::endl << std::endl;
    }

    {
        char js[] = "[ { \"a\":{ \"b\":-1.458e-20, \"d\":\"e\" }, \"f\":\"g\", \"h\":{ \"i\":\"j\" }, \"k\":[ \"l\" ] }, { \"ff\":\"g\" } ]";

        std::cout << "Test: " << test++ << ", String to object, global buffer" << std::endl;
        JSONParse<12> pr(reinterpret_cast<Buffer<256>*>(&jio), 512);

        JSONElement<12>* a = NULL;
        std::cout << "Str: " << js << std::endl;
        uint16_t fci = pr.FromString(js, &a);
        std::cout << "Size " << pr.GetBinarySize() << std::endl;
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            pr.Release(&a);
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }

        std::cout << std::endl << "Test: " << test++ << ", Object finding" << std::endl;
        pr.LimitNodeLevel(false);
        auto k = JSONString<12>("F",false);
        pr.Like(&k, false);
        auto b = pr.FindObject(a);
        std::cout << "looked " << k.Data() << ", value " << k.IsValue() << std::endl;
        if (b) {
            std::cout << "first find" << std::endl;
/*            pr.Like(&k, false);
            auto c = pr.FindObject(b->Next());
            if (c) {
                auto s = reinterpret_cast<JSONString<5>*>(c);
                printf("2nd find %s value %u\n", s->Data(), s->IsValue());
                s->Data("tst");
            }*/
            std::cout << "Error: " << pr.GetLastError() << std::endl;
            auto c = pr.NextArray(b,false);
            if (c) {
                auto d = pr.NextString(c, false);
                if (d) {
                    std::cout << "Find " << d->Data() << ", value " << d->IsValue() << std::endl;
                    d->Data("tst");
                }
            }
            std::cout << "Error: " << pr.GetLastError() << std::endl;
        }

        auto dd = pr.FindString(a, "tst", false, false);
        if (dd) {
            std::cout << "Find string " << dd->Data() << ", value " << dd->IsValue() << std::endl;
        }

        pr.LimitNodeLevel(true);

        std::cout << std::endl << "Test: " << test++ << ", Object to string" << std::endl;
        bool err = pr.FromObject(a);
        if (!err) {
            std::cout << "Last error: " << pr.GetLastError() << std::endl;
        }else {
            std::cout << "Length: " << pr.Length() << std::endl;
            std::cout << "Output: " << pr.Get() << std::endl;
            if (!strcmp("[ { \"a\":{ \"b\":-1.458e-20, \"d\":\"e\" }, \"f\":\"g\", \"h\":{ \"i\":\"j\" }, \"k\":[ \"l\" ] }, { \"tst\":\"g\" } ]", pr.Get())) {
                // Pass
            }else {
                std::cout << "Test failed" << std::endl << std::endl;
                return 1;
            }
        }

        pr.Release(&a);
        if (NULL==a) {
            std::cout << "Test passed" << std::endl << std::endl;
        }else {
            std::cout << "Release(a) failed" << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
    }

    {
        char js[] = "[ { \"a\":{ \"b\":-1.458e-20, \"d\":\"e\" }, \"f\":\"g\", \"h\":{ \"i\":\"j\" }, \"k\":[ \"l\" ] }, { \"ff\":\"g\" } ]";

        std::cout << "Test: " << test++ << ", IO put" << std::endl;
        uint16_t l = strlen(js);
        jio.Length(l);
        for(uint16_t i=0; i<l; i++) {
            jio.Put(i,js[i]);
        }
        jio.Put(l,'\0');
        const char *b = jio.Get();
        l = jio.GetLength();
        std::cout << "Length: " << l << " " << strlen(js) << std::endl;
        std::cout << "Str: " << b << std::endl;
        if (strlen(js) == l) {
            std::cout << "Test passed" << std::endl << std::endl;
        }else {
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }

        std::cout << std::endl << "Test: " << test++ << ", IO string replace" << std::endl;
        uint32_t csb = ::dump_buffer(b, l, 16);
        if (5548 == csb) {
        }else {
            std::cout << "csb: " << csb << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }

        jio.Replace("\"cool foo bar\"", 0xe, 0x18);

        l = jio.GetLength();
        printf("Len: %u\nStr: %s\n", l, b);
        uint32_t csa = ::dump_buffer(b, l, 16);
        if (6197 == csa) {
        }else {
            std::cout << "csa: " << csa << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Test passed" << std::endl << std::endl;
    }

    {
        char js[] = "{ \"\":0 }";

        std::cout << "Test: " << test++ << ", Simple string parse, empty attribute" << std::endl;
        JSONParse<12> pr(reinterpret_cast<Buffer<sizeof(js)>*>(&jio), 512);

        JSONElement<12>* a = NULL;
        std::cout << "Str: " << js << std::endl;
        uint16_t fci = pr.FromString(js, &a);
        std::cout << "Size " << pr.GetBinarySize() << std::endl;
        pr.Release(&a);
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Test passed" << std::endl << std::endl;
    }

    {
        char js[128] = "[ { \"a\":{ \"b\":-1.458e-20, \"d\":\"e\" }, \"f\":\"g\", \"h\":{ \"i\":\"j\" }, \"k\":[ \"l\" ] }, { \"ff\":\"g\" } ]";

        UserBuffer ubjio(js, 128, strlen(js));
        JSONParse<12> pr(&ubjio);

        std::cout << "Test: " << test++ << ", String custom parse using own string buffer" << std::endl;
        std::cout << "Str: " << js << std::endl;
        pr.Callbacks(true);
        uint16_t fci = pr.FromString(js);
        std::cout << "Size " << pr.GetBinarySize() << std::endl;
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Test passed" << std::endl << std::endl;
    }

    {
        char js[128] = "[ { \"a\":{ \"b\":-1.458e-20, \"d\":\"e\" }, \"f\":\"g\", \"h\":{ \"i\":\"j\" }, \"k\":[ \"l\" ] }, { \"ff\":\"g\" } ]";

        std::cout << "Test: " << test++ << ", Simple string parse, uses callbacks" << std::endl;
        JSONExampleStringParse<12> pr;

        std::cout << "Str: " << js << std::endl << "Start" << std::endl << std::endl;
        uint16_t fci = pr.FromString(js);
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "End" << std::endl << std::endl;
        std::cout << "Test passed" << std::endl << std::endl;
    }

    {
        char js[128] = "[ { \"a\":{ \"b\":-1.458e-20, \"d\":\"e\" }, \"f\":\"g\", \"h\":{ \"i\":\"j\" }, \"k\":[ \"l\" ] }, { \"ff\":\"g\" } ]";

        std::cout << "Test: " << test++ << ", Simple string parse, uses callbacks" << std::endl;
        JSONExampleStringParse<12> pr;

        std::cout << "Str: " << js << std::endl << "Start" << std::endl << std::endl;
        uint16_t fci = pr.FromString(js);
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "End" << std::endl << std::endl;
        std::cout << "Test passed" << std::endl << std::endl;
    }

    {
        char js[128] = "[ { \"aa\":{ \"bb\":\"\", \"dd\":4 }, \"f\":\"g\", \"h\":{ \"i\":\"j\" }, \"k\":[ \"l\" ] }, { \"ff\":\"g\" } ]";

        std::cout << "Test: " << test++ << ", Simple string parse, uses callbacks" << std::endl;
        JSONExampleStringParse<12> pr;

        std::cout << "Str: " << js << std::endl << "Start" << std::endl << std::endl;
        uint16_t fci = pr.FromString(js);
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "End" << std::endl << std::endl;
        std::cout << "Test passed" << std::endl << std::endl;
    }

    {
        char js[128] = "[ { \"a\":{ \"number\":0, \"test\":\"e\" }, \"f\":\"g\", \"h\":{ \"nothing\":\"something\" }, \"k\":[ \"l\" ] }, { \"ff\":\"g\" } ]";

        std::cout << "Test: " << test++ << ", Attribute parse, uses callbacks and original string as buffer.  " << std::endl;
        UserBuffer ubjio(js, 128, strlen(js));
        JSONSimpleStringParseAttr<12> pr(&ubjio);

        std::cout << "Str: " << js << std::endl;
        uint16_t fci = pr.FromStringSearch("test");
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Look for test.  Found: " << pr.GetAttrValue() << std::endl;
        }

        fci = pr.FromStringSearch("number");
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Look for number.  Found: " << pr.GetAttrValue() << std::endl;
        }

        fci = pr.FromStringSearch("something");
        if (pr.GetLastError() == JSONConstant::ERR_NOT_FOUND) {
            std::cout << "Look for something.  Didn't find anything" << std::endl;
        }else {
            std::cout << "Look for something.  Found: " << pr.GetAttrValue() << std::endl;
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Test passed" << std::endl << std::endl;
    }

    {
        char js[192] = "[{\"start\":{\"number\":0,\"test\":\"string\"},\"fall\":\"good\",\"FALL\":[\"out\",\"item\"]},{\"FALL2\":\"now\"}]";

        std::cout << "Test: " << test++ << ", Attribute parse, uses callbacks and original string as buffer, Replace both value and attribute.  " << std::endl;
        UserBuffer ubjio(js, sizeof(js), strlen(js));
        JSONSimpleStringParseAttr<12> pr(&ubjio);

        std::cout << "Input: " << js << std::endl;
        uint16_t fci = pr.FromStringSearch("test");
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Look for test.  Found: " << pr.GetAttrValue() << std::endl;
        }

        fci = pr.FromStringReplace("fall", "hello");
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Look for fall (value), replaced with " << pr.GetAttrValue() << std::endl;
        }
        std::cout << "idx: " << fci << '\'' << js[fci-1] << js[fci] << js[fci+1] << '\'' << std::endl;

        // We are starting part way through source JSON so brackets will not match
        pr.LimitNodeLevel(false);
        fci = pr.FromStringReplace(fci, "fall2", "hello2", false);
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Look for fall2 (value), replaced with " << pr.GetAttrValue() << std::endl;
        }
        pr.LimitNodeLevel(true);
        std::cout << "Output: " << ubjio.Get() << std::endl;

        // From the beginning
        fci = pr.FromStringSearch("fall");
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Look for fall (value).  Found: " << pr.GetAttrValue() << std::endl;
        }

        // Rename fall (attribute) to stop
        fci = pr.FromStringRename("fall", "stop");
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Look for fall2 (attribute), renamed with " << pr.GetAttrValue() << std::endl;
        }

        std::cout << "Output: " << js << std::endl;
        if (96 != strlen(js)) {
            std::cout << "Error expected resulting string to be length " << strlen(js) << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Test passed" << std::endl << std::endl;
    }

    {
        char js[128] = "[ { \"test\":{ \"routine\":45, \"offset\":\"byte\" }, { \"routine\":\"trap\", \"state\":\"go\" }, \"value1\", \"value2\", \"value3\" ]";

        std::cout << "Test: " << test++ << ", String parse, uses callbacks and original string as buffer.  Replace some strings.  " << std::endl;
        UserBuffer ubjio(js, sizeof(js), strlen(js));
        JSONSimpleStringParse<12> pr(&ubjio);

        std::cout << "Input: " << js << std::endl;

        uint16_t fci = pr.FromStringSearch("offset");
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Look for offset, found at " << fci << std::endl;
        }

        pr.LimitNodeLevel(false);
        fci = pr.FromStringReplace(fci, "routine", "mouse");
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Look for routine, replaced" << std::endl;
        }
        pr.LimitNodeLevel(true);

        fci = pr.FromStringReplace("value2", "middle");
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Look for middle, replaced" << std::endl;
        }

        std::cout << "Output: " << js << std::endl;
        if (110 != strlen(js)) {
            std::cout << "Error expected resulting string to be length " << strlen(js) << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }

        std::cout << "Test passed" << std::endl << std::endl;
    }

    // Obvious fail conditions
    {
        char js[32] = "[ \"red\", \"green";

        std::cout << "Test: " << test++ << ", Simple string parse, bad JSON, unterminated value string" << std::endl;
        JSONParse<12> pr(reinterpret_cast<Buffer<sizeof(js)>*>(&jio), 512);

        JSONElement<12>* a = NULL;
        std::cout << "Str: " << js << std::endl;
        uint16_t fci = pr.FromString(js, &a);
        std::cout << "Size " << pr.GetBinarySize() << std::endl;
        pr.Release(&a);
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
        }else {
            std::cout << "Parse should have failed" << std::endl << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Test passed" << std::endl << std::endl;
    }

    {
        char js[32] = "[ \"brown\", \"blue\"";

        std::cout << "Test: " << test++ << ", Simple string parse, bad JSON, no ending array" << std::endl;
        JSONParse<12> pr(reinterpret_cast<Buffer<sizeof(js)>*>(&jio), 512);

        JSONElement<12>* a = NULL;
        std::cout << "Str: " << js << std::endl;
        uint16_t fci = pr.FromString(js, &a);
        std::cout << "Size " << pr.GetBinarySize() << std::endl;
        pr.Release(&a);
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
        }else {
            std::cout << "Parse should have failed" << std::endl << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Test passed" << std::endl << std::endl;
    }

    {
        char js[32] = "[ \"orange\", { \"yellow\"";

        std::cout << "Test: " << test++ << ", Simple string parse, bad JSON, bad map" << std::endl;
        JSONParse<12> pr(reinterpret_cast<Buffer<sizeof(js)>*>(&jio), 512);

        JSONElement<12>* a = NULL;
        std::cout << "Str: " << js << std::endl;
        uint16_t fci = pr.FromString(js, &a);
        std::cout << "Size " << pr.GetBinarySize() << std::endl;
        pr.Release(&a);
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
        }else {
            std::cout << "Parse should have failed" << std::endl << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Test passed" << std::endl << std::endl;
    }

    {
        char js[32] = "[ \"orange2\", { \"yellow2\" :";

        std::cout << "Test: " << test++ << ", Simple string parse, bad JSON, bad map" << std::endl;
        JSONParse<12> pr(reinterpret_cast<Buffer<sizeof(js)>*>(&jio), 512);

        JSONElement<12>* a = NULL;
        std::cout << "Str: " << js << std::endl;
        uint16_t fci = pr.FromString(js, &a);
        std::cout << "Size " << pr.GetBinarySize() << std::endl;
        pr.Release(&a);
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
        }else {
            std::cout << "Parse should have failed" << std::endl << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Test passed" << std::endl << std::endl;
    }

    {
        char js[32] = "[ \"orange3\", { \"yellow3\" : 0";

        std::cout << "Test: " << test++ << ", Simple string parse, bad JSON, bad map" << std::endl;
        JSONParse<12> pr(reinterpret_cast<Buffer<sizeof(js)>*>(&jio), 512);

        JSONElement<12>* a = NULL;
        std::cout << "Str: " << js << std::endl;
        uint16_t fci = pr.FromString(js, &a);
        std::cout << "Size " << pr.GetBinarySize() << std::endl;
        pr.Release(&a);
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
        }else {
            std::cout << "Parse should have failed" << std::endl << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Test passed" << std::endl << std::endl;
    }

    {
        char js[32] = "[ \"orange4\", { \"yellow4\" : 0 }";

        std::cout << "Test: " << test++ << ", Simple string parse, bad JSON, bad array" << std::endl;
        JSONParse<12> pr(reinterpret_cast<Buffer<sizeof(js)>*>(&jio), 512);

        JSONElement<12>* a = NULL;
        std::cout << "Str: " << js << std::endl;
        uint16_t fci = pr.FromString(js, &a);
        std::cout << "Size " << pr.GetBinarySize() << std::endl;
        pr.Release(&a);
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
        }else {
            std::cout << "Parse should have failed" << std::endl << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }
        std::cout << "Test passed" << std::endl << std::endl;
    }

    // { "{test}":{ "date": "25Oct19" }, "time" : "12:45,34", "date" : "29Oct19", "c[o]de" : "3" }
    // JSON quoted string with control characters
    {
        char js[128] = "{ \"{test}\":{ \"date\": \"25Oct19\" }, \"time\" : \"12:45,34\", \"date\" : \"29Oct19\", \"c[o]de\" : 1.3E-24 }";

        UserBuffer ubjio(js, sizeof(js), strlen(js));         // Maximum N, string dimension (including NULL) for JSON string parsing and building
        JSONSimpleStringParseAttr<15> pr(&ubjio);

        std::cout << "Test: " << test++ << ", String parsing containing code characters" << std::endl;
        std::cout << "Input:  " << ubjio.Get() << std::endl;

        // Look for attribute/property "time"
        uint16_t fci = pr.FromStringSearch("time");
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Look for time.  Found: " << pr.GetAttrValue() << std::endl;
        }

        // We've moved over the test date as we searched for time, so we should replace the next date, 29Oct19 with another
        pr.LimitNodeLevel(false);  // Optional, so we don't get to the end with an error other than not found so we specifically don't want to make sure brackets align (we started parsing at some place in middle of JSON string)
        fci = pr.FromStringReplace(fci, "date", "26Oct19");
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Replaced proceeding date with " << pr.GetAttrValue() << std::endl;
        }
        pr.LimitNodeLevel(true);
        std::cout << "Output: " << ubjio.Get() << std::endl;
        std::cout << "Test passed" << std::endl << std::endl;
    }

    // Fail condition where no white space?? caused error?
    // { "speed" : "fast", "type" : "machine",  "wire" : { "type" : null } , "ctrl" : false }
    {
        char js[128] = "{\"speed\":\"fast\",\"type\":\"machine\",\"wire\":{\"type\":null},\"ctrl\":false}";

        gjson::UserBuffer ubjio(js, sizeof(js), strlen(js));            // Maximum N, string dimension (including NULL) for JSON string parsing and building
        gjson::JSONSimpleStringParseAttr<12> pr(&ubjio);

        std::cout << "Test: " << test++ << ", String parsing containing code characters" << std::endl;
        std::cout << "Input:  " << ubjio.Get() << std::endl;
        
        // Look for attribute/property "type"
        uint16_t fci = pr.FromStringSearch("type");
        if (pr.GetLastError() != gjson::JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Look for type.  Found: " << pr.GetAttrValue() << std::endl;
        }

        // Continue looking for same attribute/property
        pr.LimitNodeLevel(false);  // So we don't get to the end with an error other than not found so we specifically don't want brackets alignment
        fci = pr.FromStringSearch(fci, "type");
        if (pr.GetLastError() != gjson::JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Look for type.  Found: " << pr.GetAttrValue() << std::endl;
        }
        pr.LimitNodeLevel(true);
        std::cout << "Test passed" << std::endl << std::endl;
    }

    {
        char js[] = "[100,200,300,{\"value\":\"1000\"},2000,\"5\",10,20]";

        gjson::UserBuffer ubjio(js, sizeof(js), strlen(js));
        JSONSimpleStringParse<8> pr(&ubjio);

        std::cout << "Test: " << test++ << ", Searching for strings" << std::endl;
        std::cout << "Input: " << ubjio.Get() << std::endl;

        // Look for any string as "200"
        std::cout <<  "Look for 200" << std::endl;
        uint16_t fci = pr.FromStringSearch("200");
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Found at character index " << fci << std::endl;
        }

        // Continue looking for "2000"
        std::cout <<  "Continue to look for 2000" << std::endl;
        pr.LimitNodeLevel(false);
        fci = pr.FromStringSearch(fci, "2000");
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Found at character index " << fci << std::endl;
        }
        pr.LimitNodeLevel(true);

        // Look for any string as "10"
        std::cout <<  "Look for 10" << std::endl;
        fci = pr.FromStringSearch("10");
        if (pr.GetLastError() != JSONConstant::ERR_NONE) {
            std::cout << "Last error: " << pr.GetLastError() << " at character " << fci << std::endl;
            std::cout << "Test failed" << std::endl << std::endl;
            return 1;
        }else {
            std::cout << "Found at character index " << fci << std::endl;
        }
        std::cout << "Test passed" << std::endl << std::endl;
    }

    return 0;
} // _tmain(...)
