CC = gcc
CFLAGS = -Wall -Wextra -pthread -std=gnu99
TARGET = calc

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET)

test: $(TARGET)
	@echo "Testing valid input: 2+3"
	./$(TARGET) "2+3"
	@echo "\nTesting valid input: 2*3+4"
	./$(TARGET) "2*3+4"
	@echo "\nTesting valid input: (5)"
	./$(TARGET) "(5)"
	@echo "\nTesting invalid input: 5++3"
	./$(TARGET) "5++3" || echo "Correctly detected error"

.PHONY: all clean test
