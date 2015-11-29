OBJS = main.o open_list_input_output.o open_list_solver.o open_list_party.o open_list_party_basic_info.o
CC = gcc
DEBUG = -O3 -std=c++98 
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

hklc : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -lstdc++ -o hklc

main.o : main.cpp open_list_input_output.h open_list_solver.h open_list_party.h open_list_party_basic_info.h open_list_includes.h
	$(CC) $(CFLAGS) main.cpp

open_list_input_output.o : open_list_input_output.h open_list_input_output.cpp open_list_solver.h open_list_party.h open_list_party_basic_info.h open_list_includes.h
	$(CC) $(CFLAGS) open_list_input_output.cpp 

open_list_solver.o : open_list_solver.h open_list_solver.cpp open_list_party.h open_list_party_basic_info.h open_list_includes.h
	$(CC) $(CFLAGS) open_list_solver.cpp

open_list_party.o : open_list_party.h open_list_party.cpp  open_list_party_basic_info.h open_list_includes.h
	$(CC) $(CFLAGS) open_list_party.cpp

open_list_party_basic_info.o : open_list_party_basic_info.h open_list_party_basic_info.cpp open_list_includes.h
	$(CC) $(CFLAGS) open_list_party_basic_info.cpp

clean:
	\rm *.o hklc

tar:
	tar cfv hklc.tar main.cpp open_list_input_output.h open_list_input_output.cpp \
            open_list_solver.h open_list_solver.cpp open_list_party.h open_list_party.cpp \
            open_list_party_basic_info.h open_list_party_basic_info.cpp open_list_includes.h