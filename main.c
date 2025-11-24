#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

// Shared data structures
char buffer[BUFFER_SIZE];
pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

// Progress tracking structures
typedef struct {
    int adder_done;
    int multiplier_done;
    int degrouper_done;
    int buffer_changed;
} progress_t;

progress_t progress = {0, 0, 0, 0};
pthread_mutex_t progress_mutex = PTHREAD_MUTEX_INITIALIZER;

// Helper function to check if a character is an operator
int is_operator(char c) {
    return c == '+' || c == '*' || c == '-';
}

// Helper function to perform addition in the buffer
void* adder(void* arg) {
    (void)arg; // Unused parameter
    while (1) {
        pthread_mutex_lock(&buffer_mutex);
        
        // Look for pattern: number + number
        int i = 0;
        int modified = 0;
        while (buffer[i] != '\0') {
            if (isdigit(buffer[i])) {
                // Found a digit, check for + pattern
                int j = i + 1;
                while (isdigit(buffer[j])) j++;
                
                if (buffer[j] == '+') {
                    // Found '+', check for next number
                    int k = j + 1;
                    if (isdigit(buffer[k])) {
                        // Extract the two numbers
                        int num1 = 0, num2 = 0;
                        int idx = i;
                        while (isdigit(buffer[idx])) {
                            num1 = num1 * 10 + (buffer[idx] - '0');
                            idx++;
                        }
                        idx++; // skip '+'
                        while (isdigit(buffer[idx])) {
                            num2 = num2 * 10 + (buffer[idx] - '0');
                            idx++;
                        }
                        
                        // Calculate result
                        int result = num1 + num2;
                        
                        // Create new buffer
                        char new_buffer[BUFFER_SIZE];
                        strncpy(new_buffer, buffer, i);
                        new_buffer[i] = '\0';
                        sprintf(new_buffer + i, "%d", result);
                        strcat(new_buffer, buffer + idx);
                        strcpy(buffer, new_buffer);
                        
                        modified = 1;
                        
                        // Update progress
                        pthread_mutex_lock(&progress_mutex);
                        progress.buffer_changed = 1;
                        pthread_mutex_unlock(&progress_mutex);
                        
                        break;
                    }
                }
            }
            i++;
        }
        
        // Mark adder as done
        pthread_mutex_lock(&progress_mutex);
        progress.adder_done = 1;
        pthread_mutex_unlock(&progress_mutex);
        
        pthread_mutex_unlock(&buffer_mutex);
        
        if (modified) {
            usleep(1000); // Small delay to allow other threads
        } else {
            usleep(10000); // Longer delay if no work done
        }
    }
    return NULL;
}

// Helper function to perform multiplication in the buffer
void* multiplier(void* arg) {
    (void)arg; // Unused parameter
    while (1) {
        pthread_mutex_lock(&buffer_mutex);
        
        // Look for pattern: number * number
        int i = 0;
        int modified = 0;
        while (buffer[i] != '\0') {
            if (isdigit(buffer[i])) {
                // Found a digit, check for * pattern
                int j = i + 1;
                while (isdigit(buffer[j])) j++;
                
                if (buffer[j] == '*') {
                    // Found '*', check for next number
                    int k = j + 1;
                    if (isdigit(buffer[k])) {
                        // Extract the two numbers
                        int num1 = 0, num2 = 0;
                        int idx = i;
                        while (isdigit(buffer[idx])) {
                            num1 = num1 * 10 + (buffer[idx] - '0');
                            idx++;
                        }
                        idx++; // skip '*'
                        while (isdigit(buffer[idx])) {
                            num2 = num2 * 10 + (buffer[idx] - '0');
                            idx++;
                        }
                        
                        // Calculate result
                        int result = num1 * num2;
                        
                        // Create new buffer
                        char new_buffer[BUFFER_SIZE];
                        strncpy(new_buffer, buffer, i);
                        new_buffer[i] = '\0';
                        sprintf(new_buffer + i, "%d", result);
                        strcat(new_buffer, buffer + idx);
                        strcpy(buffer, new_buffer);
                        
                        modified = 1;
                        
                        // Update progress
                        pthread_mutex_lock(&progress_mutex);
                        progress.buffer_changed = 1;
                        pthread_mutex_unlock(&progress_mutex);
                        
                        break;
                    }
                }
            }
            i++;
        }
        
        // Mark multiplier as done
        pthread_mutex_lock(&progress_mutex);
        progress.multiplier_done = 1;
        pthread_mutex_unlock(&progress_mutex);
        
        pthread_mutex_unlock(&buffer_mutex);
        
        if (modified) {
            usleep(1000); // Small delay to allow other threads
        } else {
            usleep(10000); // Longer delay if no work done
        }
    }
    return NULL;
}

// Helper function to remove parentheses from expressions
void* degrouper(void* arg) {
    (void)arg; // Unused parameter
    while (1) {
        pthread_mutex_lock(&buffer_mutex);
        
        // Look for pattern: (number)
        int i = 0;
        int modified = 0;
        while (buffer[i] != '\0') {
            if (buffer[i] == '(') {
                int j = i + 1;
                int has_only_number = 1;
                
                // Check if parentheses contain only a number
                if (!isdigit(buffer[j])) {
                    has_only_number = 0;
                }
                
                while (buffer[j] != ')' && buffer[j] != '\0') {
                    if (!isdigit(buffer[j])) {
                        has_only_number = 0;
                    }
                    j++;
                }
                
                if (buffer[j] == ')' && has_only_number && j > i + 1) {
                    // Remove parentheses
                    char new_buffer[BUFFER_SIZE];
                    strncpy(new_buffer, buffer, i);
                    new_buffer[i] = '\0';
                    strncat(new_buffer, buffer + i + 1, j - i - 1);
                    strcat(new_buffer, buffer + j + 1);
                    strcpy(buffer, new_buffer);
                    
                    modified = 1;
                    
                    // Update progress
                    pthread_mutex_lock(&progress_mutex);
                    progress.buffer_changed = 1;
                    pthread_mutex_unlock(&progress_mutex);
                    
                    break;
                }
            }
            i++;
        }
        
        // Mark degrouper as done
        pthread_mutex_lock(&progress_mutex);
        progress.degrouper_done = 1;
        pthread_mutex_unlock(&progress_mutex);
        
        pthread_mutex_unlock(&buffer_mutex);
        
        if (modified) {
            usleep(1000); // Small delay to allow other threads
        } else {
            usleep(10000); // Longer delay if no work done
        }
    }
    return NULL;
}

// Sentinel thread to detect deadlock
void* sentinel(void* arg) {
    (void)arg; // Unused parameter
    while (1) {
        usleep(50000); // Wait a bit for threads to work
        
        pthread_mutex_lock(&progress_mutex);
        
        // Check if all threads have run
        if (progress.adder_done && progress.multiplier_done && progress.degrouper_done) {
            // All threads have run, check if buffer was modified
            if (!progress.buffer_changed) {
                // No progress was made - deadlock detected
                pthread_mutex_unlock(&progress_mutex);
                
                // Check if we're done (single number)
                pthread_mutex_lock(&buffer_mutex);
                int done = 1;
                int i = 0;
                while (buffer[i] != '\0') {
                    if (!isdigit(buffer[i])) {
                        done = 0;
                        break;
                    }
                    i++;
                }
                
                if (!done && strlen(buffer) > 0) {
                    // Not done and no progress can be made
                    printf("No progress can be made\n");
                    pthread_mutex_unlock(&buffer_mutex);
                    exit(EXIT_FAILURE);
                } else if (done) {
                    // Successfully reduced to a single number
                    printf("%s\n", buffer);
                    pthread_mutex_unlock(&buffer_mutex);
                    exit(EXIT_SUCCESS);
                }
                pthread_mutex_unlock(&buffer_mutex);
            } else {
                // Reset progress tracking for next cycle
                progress.adder_done = 0;
                progress.multiplier_done = 0;
                progress.degrouper_done = 0;
                progress.buffer_changed = 0;
            }
        }
        
        pthread_mutex_unlock(&progress_mutex);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <expression>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    // Initialize buffer with input
    strncpy(buffer, argv[1], BUFFER_SIZE - 1);
    buffer[BUFFER_SIZE - 1] = '\0';
    
    // Create threads
    pthread_t adder_thread, multiplier_thread, degrouper_thread, sentinel_thread;
    
    pthread_create(&adder_thread, NULL, adder, NULL);
    pthread_create(&multiplier_thread, NULL, multiplier, NULL);
    pthread_create(&degrouper_thread, NULL, degrouper, NULL);
    pthread_create(&sentinel_thread, NULL, sentinel, NULL);
    
    // Wait for sentinel to finish (will exit program)
    pthread_join(sentinel_thread, NULL);
    
    return EXIT_SUCCESS;
}
