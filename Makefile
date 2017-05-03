main : snake.cc
	$(CXX) -std=c++11 snake.cc -o main -lsfml-graphics -lsfml-window -lsfml-system

clean :
	rm main
