/**
 * \file
 * Embedded JSON builder and parser - I/O buffer
 * PROJECT: GJSON library
 * TARGET SYSTEM: Arduino, STM32, Atmel AT91SAM, portable
 */

#ifndef GJSON_IO_BUFFER_H
#define GJSON_IO_BUFFER_H

namespace gjson {

/**
 * An abstract class describing the interface of a JSON parser I/O instance.  Customise by subclass to meet your requirements
 *
 * I/O for a JSON parser can be both source and destination of a parsing operation.  This interface offers control over where 
 * data resides
 */
class IO {
public:
    /**
     * Get I/O length, actual length and maximum
     *
     * \param[in] maximum Boolean indicating query for maximum characters(true), default(false)
     * \return Character count, not including NULL
     */
    virtual uint16_t GetLength(const bool maximum=false) const = 0;


    /**
     * Set I/O length
     *
     * \param[in] length Characters, not including NULL
     */
    virtual void Length(const uint16_t length) = 0;


    /**
     * Get I/O string pointer
     *
     * \return Pointer to internally managed character string
     */
    virtual const char* Get() const = 0;


    /**
     * Get character from I/O at specific index
     *
     * \param[in] i Index
     * \return Character
     */
    virtual char Get(uint16_t i) const = 0;


    /**
     * Put character at index
     *
     * \attention Length of I/O string should include index
     *
     * \param[in] i Index
     * \param[in] v Character
     * \retval true Success
     * \retval false Failure (likely index out of range)
     */
    virtual bool Put(uint16_t i, char v) = 0;


    /**
     * Append character
     *
     * \param[in] v Character
     * \retval true Success
     * \retval false Failure, buffer full
     */
    virtual bool Append(char v) = 0;


    /**
     * Append NULL terminated string
     *
     * \attention No character data will be appended upon failure, it is all or nothing
     *
     * \param[in] s Pointer to NULL termined string for append
     * \retval true Success
     * \retval false Failure, buffer full
     */
    virtual bool Append(const char* s) = 0;


    /**
     * Helper, replace string content between start and end indexes with given string
     *
     * \attention This is an optional string feature and generally if objects are used then this may not be required, especially if 
     * no updates are made to source JSON string.
     *
     * \param[in] s Pointer to NULL termined string for replace
     * \param[in] sp Start index (characters)
     * \param[in] ep End index (characters), should be larger than start index
     * \return Replace state
     */
    virtual bool Replace(const char* s, uint16_t sp, uint16_t ep) = 0;
}; // class IO


/**
 * An class for a NULL buffer when JSON parsing.  This offers the smallest footprint but limits use of JSON parsing to your source string
 */
class NullBuffer : public IO {
public:
    /**
     * Get I/O string pointer
     *
     * \return NULL
     */
    const char* Get() const {
        return NULL;
    }


    /**
     * Get character from I/O at specific index
     *
     * \attention Ignore parameters for NULL I/O
     *
     * \param[in] i Index
     * \return Character '\0' (NULL terminator)
     */
    char Get(uint16_t i) const {
        return '\0';
    }


    /**
     * Get I/O length, actual length and maximum
     *
     * \attention Ignore parameters for NULL I/O
     *
     * \param[in] maximum Boolean
     * \return Character count, always 0
     */
    uint16_t GetLength(const bool maximum=false) const {
        (void)maximum;
        return 0U;
    }


    /**
     * Set I/O length
     *
     * \attention Ignore parameters for NULL I/O
     *
     * \param[in] length Characters
     */
    void Length(const uint16_t length) {
        (void)length;
    }


    /**
     * Put character at index
     *
     * \attention Ignore parameters for NULL I/O
     *
     * \param[in] i Index
     * \param[in] v Character
     * \retval false Always failure (there is no I/O)
     */
    bool Put(uint16_t i, char v) {
        (void)i;
        (void)v;
        return false;
    }


    /**
     * Append character
     *
     * \attention Ignore parameters for NULL I/O
     *
     * \param[in] v Character
     * \retval false Always failure (there is no I/O)
     */
    bool Append(char v) {
        (void)v;
        return false;
    }


    /**
     * Append NULL terminated string
     *
     * \attention Ignore parameters for NULL I/O
     *
     * \param[in] s Pointer to NULL termined string for append
     * \retval true Success
     * \retval false Failure, buffer full
     */
    bool Append(const char* s) {
        (void)s;
        return false;
    }


    /**
     * Helper, replace string content between start and end indexes with given string
     *
     * \attention Ignore parameters for NULL I/O
     *
     * \param[in] s Pointer to NULL termined string for replace
     * \param[in] sp Start index (characters)
     * \param[in] ep End index (characters), should be larger than start index
     * \retval false Replace state, always fail
     */
    bool Replace(const char* s, uint16_t sp, uint16_t ep) {
        (void)s;
        (void)sp;
        (void)ep;
        return false;
    }

}; // class NullBuffer


/**
 * An class for a user specified buffer when JSON parsing.  This allows callers to specify where the JSON parser string I/O resides and 
 * allows for the situation where a user may want to reuse a UART buffer rather than create another.
 */
class UserBuffer : public IO {
public:
    /**
     * Constructor, make stable instance.  Take user buffer details, string pointer, string length and buffer length
     *
     * \attention The given string used for buffering must exist for the entire life of related parser instance
     *
     * \param[in] s Pointer to source NULL terminated string buffer for parser I/O
     * \param[in] ml Maximum buffer length (characters, including NULL terminator)
     * \param[in] l Current string length in buffer, if any, default 0 characters
     */
    UserBuffer(char *s, uint16_t ml, uint16_t l=0) : max_length_(ml), length_(l), buffer_(s) {
        buffer_[l] = '\0'; // Make sure input buffer is NULL terminated
    }


    const char* Get() const {
        return &buffer_[0];
    }


    char Get(uint16_t i) const {
        if (i>=length_) {
            i = length_-1;
        }

        return buffer_[i];
    }


    uint16_t GetLength(const bool maximum=false) const {
        uint16_t l;

        if (maximum) {
            l = max_length_;
        }else {
            l = length_;
        }

        return l;
    }


    void Length(const uint16_t length) {
        length_ = length;
    }


    bool Put(uint16_t i, char v) {
        bool wr_err = true;

        if (i<length_) {
            wr_err = false;
            buffer_[i] = v;
        }

        return wr_err;
    }


    bool Append(char v) {
        bool wr_err = true;

        if (length_ < max_length_) {
            wr_err = false;
            buffer_[length_++] = v;
        }

        return wr_err;
    }


    bool Append(const char* s) {
        bool wr_err = true;
        uint8_t l = strlen(s);

        if ((length_ + l) < max_length_) {
            wr_err = false;
            for(uint8_t i=0; i<l; i++) {
                buffer_[length_++] = s[i];
            }
        }

        return wr_err;
    }


    bool Replace(const char* s, uint16_t sp, uint16_t ep) {
        // Implementation optional, used if output buffer also input and updates desired (string manipulation) rather than objects
        bool r = true;

        if (sp>=0 && ep>=0 && sp<length_ && ep<=length_) {
            length_ = JSONSupport::Replace(&buffer_[0], length_, max_length_, sp, ep, s, strlen(s));
            r = false;
        }

        return r;
    }

protected:
    uint16_t    max_length_;                ///< Maximum buffer length, Characters
    uint16_t    length_;                    ///< Length of buffer, Characters.  Has to be less than equal to \ref max_length_
    char        *buffer_;                   ///< Character buffer pointer
}; // class UserBuffer


/**
 * An class for an internally managed character buffer when JSON parsing.  Use this in situations where you require some 
 * storage for JSON, when data may get changed and there is no user string to be reused.
 *
 * \tparam BL Maximum buffering length in characters (including any NULL terminator)
 */
template<uint16_t BL>
class Buffer : public IO {
public:
    /**
     * Default constructor, make stable instance
     */
    Buffer() : length_(0) {
    }


    const char* Get() const {
        return &buffer_[0];
    }


    char Get(uint16_t i) const {
        if (i>=length_) {
            i = length_-1;
        }

        return buffer_[i];
    }


    uint16_t GetLength(const bool maximum=false) const {
        uint16_t l;

        if (maximum) {
            l = sizeof(buffer_);
        }else {
            l = length_;
        }

        return l;
    }


    void Length(const uint16_t length) {
        length_ = length;
    }


    bool Put(uint16_t i, char v) {
        bool wr_err = true;

        if (i<length_) {
            wr_err = false;
            buffer_[i] = v;
        }

        return wr_err;
    }


    bool Append(char v) {
        bool wr_err = true;

        if (length_ < sizeof(buffer_)) {
            wr_err = false;
            buffer_[length_++] = v;
        }

        return wr_err;
    }


    bool Append(const char* s) {
        bool wr_err = true;
        uint8_t l = strlen(s);

        if ((length_ + l) < sizeof(buffer_)) {
            wr_err = false;
            for(uint8_t i=0; i<l; i++) {
                buffer_[length_++] = s[i];
            }
        }

        return wr_err;
    }


    bool Replace(const char* s, uint16_t sp, uint16_t ep) {
        // Implementation optional, used if output buffer also input and updates desired (string manipulation) rather than objects
        bool r = true;

        if (sp>=0 && ep>=0 && sp<length_ && ep<=length_) {
            length_ = JSONSupport::Replace(&buffer_[0], length_, sizeof(buffer_), sp, ep, s, strlen(s));
            r = false;
        }

        return r;
    }

protected:
    uint16_t    length_;                    ///< Length of buffer, Characters.  Has to be less than equal to \see BL
    char        buffer_[BL];                ///< Character buffer
}; // class Buffer

}; // namespace gjson

#endif // GJSON_IO_BUFFER_H
