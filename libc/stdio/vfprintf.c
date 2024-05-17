#include <string.h>

typedef int (*pfnStreamWriteBuf)(char*);

enum ParseMode { NORMAL, ARGUMENT, FORMAT_SPECIFIER };

/**
 * @brief A structure representing a stream for writing data.
 *
 * This structure is used to manage a buffer for writing data to an output stream.
 * It includes a buffer length, buffer index, buffer pointer, and a function pointer
 * to a function that writes all data in the buffer to the output stream.
 */
struct Stream {
    /**
     * @brief The length of the buffer.
     *
     * This value represents the total size of the buffer in bytes.
     */
    size_t buf_len;

    /**
     * @brief The current index in the buffer.
     *
     * This value represents the position in the buffer where the next character
     * will be written.
     */
    size_t buf_i;

    /**
     * @brief A pointer to the buffer.
     *
     * This pointer points to the beginning of the buffer where data will be stored.
     */
    char *buf;

    /**
     * @brief A function pointer to a function that writes all data in the buffer.
     *
     * This function pointer points to a function that takes a pointer to a character
     * array (the buffer) and writes all the data in the buffer to the output stream.
     *
     * @param buf A pointer to the buffer to be written.
     * @return The number of bytes written to the output stream.
     */
    pfnStreamWriteBuf pfn_write_all;
};

/**
 * @brief Prints the content of the buffer to the output stream.
 *
 * This function ensures that the buffer is null-terminated before writing it to the output stream.
 * It then calls the function pointed to by `stream->pfn_write_all` to write the buffer to the output stream.
 * If the number of bytes written does not match the length of the buffer, it returns 1 to indicate an error.
 * Otherwise, it clears the buffer and returns 0 to indicate success.
 *
 * @param stream A pointer to the Stream structure containing the buffer and the function to write to the output stream.
 * @return 0 on success, 1 on failure.
 */
static int print_stream_buf(struct Stream *stream) {
    // Ensure we are null terminated. Can never be too safe!
    stream->buf[stream->buf_len - 1] = '\0';

    // Write the buf.
    if ((size_t)stream->pfn_write_all(stream->buf)!= strlen(stream->buf)) {
        // We didn't write the entire usable portion of the buffer, for some reason.
        return 1;
    }

    // Clear the buffer.
    memset(stream->buf, 0, stream->buf_len);

    return 0;
}

/**
 * @brief Pushes a character to the buffer and flushes it if necessary.
 *
 * This function appends a character to the buffer and checks if the buffer is full.
 * If the buffer is full, it calls the `print_stream_buf` function to flush the buffer.
 * If the flush operation fails, it returns the error code.
 *
 * @param stream A pointer to the Stream structure containing the buffer and the function to write to the output stream.
 * @param c The character to be pushed to the buffer.
 * @return 0 on success, non-0 on failure.
 */
static int push_to_buf(struct Stream *stream, char c) {
    stream->buf[stream->buf_i] = c;
    stream->buf_i += 1;

    // Check if the buffer is full. If so, flush it.
    if (stream->buf_i == stream->buf_len - 2) {
        int err;
        stream->buf_i = 0;

        err = print_stream_buf(stream);
        if (err!= 0) {
            // We didn't write the entire buffer, for some reason.
            return err;
        }
    }

    return 0;
}

/**
 * @brief Pushes all characters from a string to the buffer and flushes it if necessary.
 *
 * This function iterates over each character in the provided string. For each character,
 * it calls the `push_to_buf` function to add the character to the buffer. If the `push_to_buf`
 * function returns a non-zero value, it means an error occurred during the buffer flushing process.
 * In this case, the function immediately returns the error code.
 *
 * If all characters are successfully added to the buffer, the function returns 0 to indicate success.
 *
 * @param stream A pointer to the Stream structure containing the buffer and the function to write to the output stream.
 * @param str A pointer to the null-terminated string to be pushed to the buffer.
 * @return 0 on success, non-0 on failure.
 */
static int push_all_to_buf(struct Stream *stream, char *str) {
    int err;

    for (char c = *str; c!= '\0'; c = *++str) {
        err = push_to_buf(stream, c);

        if (err!= 0) {
            return err;
        }
    }

    return 0;
}

/**
 * @brief Pushes a signed integer to the buffer and flushes it if necessary.
 *
 * This function converts a signed integer to an unsigned integer, then converts it to a string
 * representation in the specified base (decimal, hexadecimal, or octal). The resulting string
 * is then pushed to the buffer. If the integer is negative, a '-' character is prepended to the string.
 *
 * @param stream A pointer to the Stream structure containing the buffer and the function to write to the output stream.
 * @param val The signed integer to be pushed to the buffer.
 * @param base The base to which the integer should be converted. This can be 10 (decimal), 16 (hexadecimal), or 8 (octal).
 *
 * @note This function assumes that the buffer size (MAXBUF) is sufficient to hold the converted string.
 */
static void push_int_to_buf(struct Stream *stream, int val, unsigned int base) {
    int i = MAXBUF-2;
    static char buf[MAXBUF] = {0};

    // If the value is negative, convert it to positive and prepend a '-' character.
    if (val < 0) {
        push_to_buf(stream, '-');
        val = -val;
    }

    // Convert the unsigned integer to a string representation in the specified base.
    for (; val && i; --i, val /= base)
        buf[i] = "0123456789abcdefghijklmnopqrstuvwxyz"[val % base];

    // Push the converted string to the buffer.
    push_all_to_buf(stream, &buf[i+1]);
}

/**
 * @brief Converts a character representing a display flag to its corresponding base.
 *
 * This function takes a character representing a display flag ('d', 'x', or 'o') and
 * returns the corresponding base (10, 16, or 8) for printing signed or unsigned integers.
 * If the input character does not match any of the expected flags, the function returns 10.
 *
 * @param flag The character representing the display flag.
 * @return The corresponding base for printing integers.
 */
static int int_display_flag_to_base(char flag) {
    switch (flag) {
        case 'd':
            return 10;  // Decimal base
        case 'x':
            return 16;  // Hexadecimal base
        case 'o':
            return 8;   // Octal base
        default:
            return 10;  // Default to decimal base if the flag does not match any expected values
    }
}

/**
 * @brief The main function for formatted output to a stream.
 *
 * This function is a custom implementation of the vfprintf function, which writes formatted output to a stream.
 * It supports the following format specifiers:
 *  - {?<arg>.<format code>} : This allows for including arguments in the format string.
 *    - '?' : Indicates the start of an argument.
 *    - '<arg>' : Represents the argument number.
 *    - '<format code>' : Represents the format code for the argument.
 *  - Printing signed number: {?<base>d}
 *    - '<base>' : Represents the base for the number. It can be 'd' for decimal, 'x' for hexadecimal, or 'o' for octal.
 *  - Printing unsigned number: {?<base>u}
 *    - '<base>' : Represents the base for the number. It can be 'd' for decimal, 'x' for hexadecimal, or 'o' for octal.
 *  - Printing string: {s}
 *
 * @param stream A pointer to the Stream structure containing the buffer and the function to write to the output stream.
 * @param fmt A pointer to the format string.
 * @param args A variable argument list containing the arguments to be formatted.
 * @return 0 on success, non-0 on failure.
 */
int vfprintf(struct Stream *stream, const char *fmt, va_list args) {
    enum ParseMode parse_mode = NORMAL;
    char arg = '\0';

    for (size_t i = 0; i < strlen(fmt); i++) {
        char cur = fmt[i];

        switch (cur) {
            // Handle opening curly brace
            case '{': {
                if (parse_mode == NORMAL) {
                    // Start looking for arguments next, or another '{' for escape.
                    parse_mode = FORMAT_SPECIFIER;
                    goto char_loop_end;
                } else {
                    // We escaped the '{' character.
                    parse_mode = NORMAL;
                }

                break;
            }

            // Handle closing curly brace
            case '}': {
                if (parse_mode == FORMAT_SPECIFIER) {
                    parse_mode = NORMAL;
                    goto char_loop_end;
                }

                break;
            }

            // Handle question mark
            case '?': {
                if (parse_mode == FORMAT_SPECIFIER) {
                    parse_mode = ARGUMENT;
                    goto char_loop_end;
                }

                break;
            }

            // Handle signed decimal number
            case 'd': {
                if (parse_mode == FORMAT_SPECIFIER) {
                    int val = va_arg(args, int);

                    if (val < 0)
                        push_to_buf(stream, '-');

                    push_int_to_buf(stream, (unsigned int) val, int_display_flag_to_base(arg));

                    goto format_spec_end;
                }

                break;
            }

            // Handle unsigned decimal number
            case 'u': {
                if (parse_mode == FORMAT_SPECIFIER) {
                    unsigned int val = va_arg(args, unsigned int);
                    push_int_to_buf(stream, val, int_display_flag_to_base(arg));

                    goto format_spec_end;
                }

                break;
            }

            // Handle string
            case '': {
                if (parse_mode == FORMAT_SPECIFIER) {
                    char *str = va_arg(args, char *);
                    push_all_to_buf(stream, str);

                    goto format_spec_end;
                }

                break;
            }

            // Handle argument
            default: {
                if (parse_mode == ARGUMENT) {
                    arg = cur;
                    parse_mode = FORMAT_SPECIFIER;

                    goto char_loop_end;
                }

                break;
            }
        }

        // Add the current character to the buffer.
        push_to_buf(stream, cur);

        goto char_loop_end;

        format_spec_end:
        arg = '\0';

        char_loop_end:
        continue;
    }

    // At the end, flush the buffer.
    if (print_stream_buf(stream)!= 0) {
        // We didn't write the entire buffer, for some reason.
        // TODO: Decide what to do if we ever get here.
    }

    // And of course reset the buffer index.
    stream->buf_i = 0;

    return 0;
}
