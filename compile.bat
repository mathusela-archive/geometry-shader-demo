if not exist "./bin" mkdir "./bin"
g++ ./source/main.cpp ./source/glad.c -o./bin/main -I./include -L./libs -lglfw3 -lopengl32 -lgdi32