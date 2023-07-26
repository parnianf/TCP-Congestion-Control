#Reference
#http://makepp.sourceforge.net/1.19/makepp_tutorial.html

CC = gcc -c
SHELL = /bin/bash

# compiling flags here
CFLAGS = -Wall -I.

LINKER = gcc -o 
# linking flags here
LFLAGS   = -Wall

OBJDIR = ../obj

CLIENT_OBJECTS := $(OBJDIR)/sender.o $(OBJDIR)/common.o $(OBJDIR)/packet.o $(OBJDIR)/queue.o
SERVER_OBJECTS := $(OBJDIR)/receiver.o $(OBJDIR)/common.o $(OBJDIR)/packet.o $(OBJDIR)/queue.o
ROUTER_OBJECTS := $(OBJDIR)/router.o $(OBJDIR)/common.o $(OBJDIR)/packet.o $(OBJDIR)/queue.o

#Program name
CLIENT := $(OBJDIR)/sender
SERVER := $(OBJDIR)/receiver
ROUTER := $(OBJDIR)/router

rm       = rm -f
rmdir    = rmdir 

TARGET:	$(OBJDIR) $(CLIENT)	$(SERVER) $(ROUTER)


$(CLIENT):	$(CLIENT_OBJECTS)
	$(LINKER) $@  $(CLIENT_OBJECTS) 
	@echo "Link complete!"

$(SERVER): $(SERVER_OBJECTS)
	$(LINKER) $@  $(SERVER_OBJECTS) 
	@echo "Link complete!"

$(ROUTER): $(ROUTER_OBJECTS)
	gcc -pthread -o  $@ $(ROUTER_OBJECTS) 
	@echo "Link complete!"

$(OBJDIR)/%.o:	%.c common.h packet.h queue.h
	$(CC) $(CFLAGS)  $< -o $@
	@echo "Compilation complete!"

clean:
	@if [ -a $(OBJDIR) ]; then rm -r $(OBJDIR); fi;
	@echo "Cleanup complete!"

$(OBJDIR):
	@[ -a $(OBJDIR) ]  || mkdir $(OBJDIR)
