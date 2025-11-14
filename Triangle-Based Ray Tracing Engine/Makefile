# === AYARLAR ===
CXX = g++
INCLUDES = -Iinclude     
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 $(INCLUDES)
LDFLAGS = -ltinyxml2                     # <-- BUNU EKLEDİK
TARGET = raytracer

# === KAYNAK DOSYALARI ===
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

# === DEFAULT HEDEF ===
all: $(TARGET)

# === LİNKLEME ===
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# === DERLEME ===
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === ÇALIŞTIR ===
run: $(TARGET)
	./$(TARGET)

# === TEMİZLEME ===
clean:
	rm -f *.o $(TARGET)

.PHONY: all clean run
