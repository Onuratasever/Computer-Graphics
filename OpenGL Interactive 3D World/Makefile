CXX      := g++
SRC_DIR  := src
INC_DIR  := include
LIB_DIR  := C:/Users/onur/Downloads/glfw-master/glfw-master/build/src

SRCS := $(SRC_DIR)/main.cpp \
        $(SRC_DIR)/glad.c    \
        $(SRC_DIR)/Shader.cpp

# Eğer Camera.cpp varsa:
# SRCS += $(SRC_DIR)/Camera.cpp

OUT  := app.exe

CXXFLAGS := -std=c++17 -I$(INC_DIR) -Wall -g
LDFLAGS  := -L$(LIB_DIR) -lglfw3 -lopengl32 -lgdi32

$(OUT): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

clean:
	del /Q $(OUT)
