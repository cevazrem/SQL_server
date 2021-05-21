COMPILER=g++
FLAGS=-Wall -Wformat-security -Wignored-qualifiers -Winit-self -Wswitch-default -Wfloat-equal -Wpointer-arith -Wtype-limits -Wempty-body -Wlogical-op -Wmissing-field-initializers -Wcast-qual -Wwrite-strings

main: main.o socket.o server.o client.o sql.o dbhandler.o
	$(COMPILER) $^ -o $@

main.o: ./src/main.cpp ./src/main.h
	$(COMPILER) $(FLAGS) -c ./src/main.cpp
socket.o: ./src/socket.cpp ./src/socket.h
	$(COMPILER) $(FLAGS) -c ./src/socket.cpp
client.o: ./src/client.cpp ./src/client.h
	$(COMPILER) $(FLAGS) -c ./src/client.cpp
server.o: ./src/server.cpp ./src/server.h
	$(COMPILER) $(FLAGS) -c ./src/server.cpp
sql.o: ./sql/sql.cpp ./sql/sql.h
	$(COMPILER) $(FLAGS) -c ./sql/sql.cpp
dbhandler.o: ./dbt/dbhandler.cpp ./dbt/dbhandler.h
	$(COMPILER) $(FLAGS) -c ./dbt/dbhandler.cpp

clear: 
	rm *.o
	rm main
