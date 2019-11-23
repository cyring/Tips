# Call Simulator. (C) Bouygues Telecom
APP_NAME=CallSim
APP_CFLAGS=-threads
APP_LDFLAGS=-threads
APP_LIBS=
APP_SYSLIBS=$(SOCKET)
APP_OBJ=Extend.o \
	CallSim.o
#
all : $(APP_NAME).x

clean :
	-@rm *.x *.o
#
$(APP_NAME).x : $(APP_OBJ)
	$(CC) $(LDFLAGS) $(APP_LDFLAGS) -o $(APP_NAME).x $(APP_OBJ) $(APP_LIBS) $(APP_SYSLIBS) $(CUST_SYSLIBS)
#
CallSim.o: CallSim.c 
	$(CC) $(CFLAGS) $(APP_CFLAGS) $(CUST_INCLUDE) -c CallSim.c
#
Extend.o: Extend.c 
	$(CC) $(CFLAGS) $(APP_CFLAGS) $(CUST_INCLUDE) -c Extend.c
