#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

/**
 * @brief Hardware text mode color constants.
 *
 * This enum defines the available colors for the VGA text mode.
 * Each color is represented by a unique integer value.
 */
enum vga_color {
    VGA_COLOR_BLACK = 0,    ///< Represents the color black.
    VGA_COLOR_BLUE = 1,     ///< Represents the color blue.
    VGA_COLOR_GREEN = 2,    ///< Represents the color green.
    VGA_COLOR_CYAN = 3,     ///< Represents the color cyan.
    VGA_COLOR_RED = 4,      ///< Represents the color red.
    VGA_COLOR_MAGENTA = 5,  ///< Represents the color magenta.
    VGA_COLOR_BROWN = 6,    ///< Represents the color brown.
    VGA_COLOR_LIGHT_GREY = 7,///< Represents the color light grey.
    VGA_COLOR_DARK_GREY = 8,///< Represents the color dark grey.
    VGA_COLOR_LIGHT_BLUE = 9,///< Represents the color light blue.
    VGA_COLOR_LIGHT_GREEN = 10,///< Represents the color light green.
    VGA_COLOR_LIGHT_CYAN = 11,///< Represents the color light cyan.
    VGA_COLOR_LIGHT_RED = 12,///< Represents the color light red.
    VGA_COLOR_LIGHT_MAGENTA = 13,///< Represents the color light magenta.
    VGA_COLOR_LIGHT_BROWN = 14,///< Represents the color light brown.
    VGA_COLOR_WHITE = 15,   ///< Represents the color white.
};

/**
 * @brief Calculates a color value for the VGA text mode.
 *
 * This function takes two parameters, representing the foreground and background colors,
 * and returns a single 8-bit value that can be used to set the color of characters in the VGA text mode.
 * The foreground color is placed in the lower 4 bits of the result, and the background color is placed in the upper 4 bits.
 *
 * @param fg The foreground color. This should be one of the enum values defined in the vga_color enum.
 * @param bg The background color. This should be one of the enum values defined in the vga_color enum.
 * @return An 8-bit value representing the combined foreground and background colors.
 */
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}

static inline uint16_t /**
 * @brief Calculates a 16-bit VGA entry value from a character and a color.
 *
 * This function takes a character and a color as input and returns a 16-bit value
 * that can be written directly to the VGA text mode buffer. The character is placed
 * in the lower 8 bits of the 16-bit value, and the color is placed in the upper 8 bits.
 *
 * @param uc The character to be displayed.
 * @param color The color of the character.
 * @return A 16-bit value that can be written to the VGA text mode buffer.
 */
vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t) uc | (uint16_t) color << 8;
}

/**
 * @brief Calculates the length of a null-terminated string.
 *
 * This function iterates over the characters in the given string, incrementing a counter
 * until it encounters a null character ('\0'). The function then returns the count, which
 * represents the length of the string.
 *
 * @param str A pointer to the null-terminated string whose length is to be calculated.
 * @return The length of the string, not including the null terminator.
 */
size_t strlen(const char* str)
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

/**
 * @brief Initializes the terminal interface.
 *
 * This function sets up the terminal by initializing the cursor position, color,
 * and buffer address. It also clears the screen by filling the entire buffer with
 * spaces in the current color.
 *
 * @param void No parameters are expected.
 * @return void This function does not return a value.
 */
void terminal_initialize(void)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

/**
 * @brief Sets the color for subsequent terminal output.
 *
 * This function updates the terminal_color variable with the given color,
 * which will be used for subsequent calls to terminal_putchar and terminal_write.
 * The color is specified using the vga_color enum, which contains predefined color constants.
 *
 * @param color The color to be used for subsequent terminal output.
 * @return void This function does not return a value.
 */
void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}

/**
 * @brief Writes a character at a specific position in the terminal buffer.
 *
 * This function calculates the index in the terminal buffer based on the given x and y coordinates,
 * and then writes the character and its color to that position in the buffer.
 *
 * @param c The character to be written to the terminal.
 * @param color The color of the character.
 * @param x The horizontal position of the character in the terminal.
 * @param y The vertical position of the character in the terminal.
 * @return void This function does not return a value.
 */
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

/**
 * @brief Clears a specific row in the terminal buffer.
 *
 * This function iterates over the specified row in the terminal buffer and replaces each character
 * with a space in the current color. This effectively clears the contents of the specified row.
 *
 * @param row_number The number of the row to be cleared. The top row is numbered 0.
 * @return void This function does not return a value.
 */
void clear_terminal_row(int row_number){
    for (size_t i = 0; i < VGA_WIDTH; i++){
        terminal_putentryat(' ', terminal_color, i, row_number);
    }
}

/**
 * @brief Prints a single character to the terminal at the current cursor position.
 *
 * This function writes a single character to the terminal at the current cursor position,
 * and then updates the cursor position accordingly. If the cursor reaches the end of a line,
 * it wraps to the next line. If the cursor reaches the bottom of the screen, it scrolls the
 * contents up by one line.
 *
 * @param c The character to be printed to the terminal.
 * @return void This function does not return a value.
 */
void terminal_putchar(char c)
{
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
    }
    if (terminal_row == VGA_HEIGHT){
        for (size_t i = 0; i < VGA_HEIGHT-1; i++){
            terminal_buffer[i] = terminal_buffer[i+1];
        }
        clear_terminal_row(VGA_HEIGHT);
        terminal_row = VGA_HEIGHT-1;
    }
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_row = 0;
    }
}

/**
 * @brief Writes a specified number of characters from a given string to the terminal.
 *
 * This function iterates over the given string, calling the terminal_putchar function
 * for each character. It continues until it has processed the specified number of characters.
 *
 * @param data A pointer to the string from which characters will be written to the terminal.
 * @param size The number of characters to be written to the terminal.
 * @return void This function does not return a value.
 */
void terminal_write(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

/**
 * @brief Writes a null-terminated string to the terminal.
 *
 * This function calculates the length of the input string using the strlen function,
 * and then calls the terminal_write function to print the string to the terminal.
 *
 * @param data A pointer to the null-terminated string to be printed.
 * @return void This function does not return a value.
 */
void terminal_writestring(const char* data)
{
    terminal_write(data, strlen(data));
}

/**
 * @brief The main entry point of the kernel.
 *
 * This function initializes the terminal interface, prints a welcome message,
 * and then enters an infinite loop. Newline support is left as an exercise.
 *
 * @param void No parameters are expected.
 * @return void This function does not return a value.
 */
void kernel_main(void)
{
    /* Initialize terminal interface */
    terminal_initialize();

    /* Newline support is left as an exercise. */
    terminal_writestring("Hello, kernel World!\n");

    /* Infinite loop to prevent the kernel from terminating */
    for (int i = 0; ; i++) {
        /* Add your code here if you want to add functionality */
        if ((i % 2) == 0)
            terminal_writestring("New Line\n");
        else
            terminal_writestring("Another Line\n");
    }
}