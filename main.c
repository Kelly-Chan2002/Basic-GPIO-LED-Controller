#include <gpiod.h>      // GPIOD library
#include <stdio.h>      // Standard library
#include <unistd.h>     // POSIX standard library
#include <time.h>       // Standard timing library

#define HALF_SECOND 500000000       // Define Macro in nanoseconds

void variable_nanosleep(long nanoseconds)       // Function for sleeping
{
    struct timespec req = {
        .tv_sec = nanoseconds / 1000000000,     // Convert ns to s
        .tv_nsec = nanoseconds % 1000000000     // Remainder in ns
    };

    nanosleep(&req, NULL);      // Call nanosleep system call
}

int main()
{
    const char *chip_title = "gpiochip0";       // Declare variables
    struct gpiod_chip *chip;        // GPIO chip structure
    struct gpiod_line *led;     // GPIO connected to LED                                                                     
    struct gpiod_line *button;      // GPIO connected to button

    chip = gpiod_chip_open_by_name(chip_title);     // Open chip that handles GPIO
    led = gpiod_chip_get_line(chip, 5);     // Grab lines from chip for output (LED)
    button = gpiod_chip_get_line(chip, 6);      // Grab lines from chip for input (button)
    gpiod_line_request_output(led, "Output", 0);       // Output line
    gpiod_line_request_input(button, "Input");      // Input line

    unsigned long pressed_time = 0;    // Initialize to 0
    const struct timespec delay = {0, 100};    // 100ns 
    long sleep_time = HALF_SECOND;      // init
    int prev_button_state = 1;  // assume unpressed initially

    while (1)       // Infinite loop
    {
        int button_state = gpiod_line_get_value(button);        // Read current button value

        if (prev_button_state == 1 && button_state == 0) {      // falling edge detected → start press timing
            
            printf("Button press is detected.\n");

            struct timespec start_time, end_time;       // To store timestap for measuring press duration
            clock_gettime(CLOCK_MONOTONIC, &start_time);        // Record start time

            while (gpiod_line_get_value(button) == 0) {     // Wait until button is released
                variable_nanosleep(20000);  // debounce
            }

            clock_gettime(CLOCK_MONOTONIC, &end_time);      // Record end time
            pressed_time = (end_time.tv_sec - start_time.tv_sec) * 1e9 +        
                           (end_time.tv_nsec - start_time.tv_nsec);     // Calculate time difference

            printf("Pressed time: %ld\n", pressed_time);

            if (pressed_time < 1000000000) { // < 1 second
                sleep_time /= 2;        // Increase blink frequency
                printf("Type: Short press, blinking frequency will be doubled.\n");
            } else {        // 1 second >
                sleep_time = HALF_SECOND;       // Reset to default blink time
                printf("Type: Long press, blinking reset to default.\n");
            }

            pressed_time = 0;       // Reset pressed time
        }

        prev_button_state = button_state;       // Update state

        const long tick_ns = 10000000; // Blink with short sleeps-10ms
        long elapsed_ns = 0;        // Check button between short sleeps

        gpiod_line_set_value(led, 0); // LED ON
        while (elapsed_ns < sleep_time) {       // Loop until the total blink interval has passed
            variable_nanosleep(tick_ns);        // Sleep for a short duration-10ms
            elapsed_ns += tick_ns;      // Adding elapsed time

            button_state = gpiod_line_get_value(button);        // Check current state of button
            if (prev_button_state == 1 && button_state == 0) {      // Detect falling edge
                break;      // Exit early
            }
            prev_button_state = button_state;       // Update previous button state
        }

        elapsed_ns = 0;     // Reset elapsed
        gpiod_line_set_value(led, 1); // LED OFF
        while (elapsed_ns < sleep_time) {       // Turn off LED
            variable_nanosleep(tick_ns);        // Sleep briefly
            elapsed_ns += tick_ns;      // Add elapsed time with tick duration

            button_state = gpiod_line_get_value(button);        // Read current state
            if (prev_button_state == 1 && button_state == 0) {      // Detect new button press
                break;      // Exit early
            }
            prev_button_state = button_state;       // Update previous button state
        }
    }

    gpiod_line_release(led);        // Release lines
    gpiod_line_release(button);     // Release button
    gpiod_chip_close(chip);         // Release chip

    return 0;       // Exit
}
