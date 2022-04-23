# author: Bonino, Francisco Ignacio.
# version: 0.4.4
# since: 2022-03-20

# Flags a utilizar
CC = gcc
CFLAGS = -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11
CCOMPILE = $(CC) $(CFLAGS)
SLIBF = ar rcs
DIRS = ./bin ./obj ./slib ./src/resources/log

# En caso de ejecutar 'make' sin argumento, se aplica el target indicado
all: build_folders srv cln

# Directorios donde se guardarán los archivos
build_folders:
	mkdir -p $(DIRS)

# Librería estática propia: utilities
lib_utilities.a: utilities.o
	$(SLIBF) slib/$@ obj/$<

utilities.o: src/include/bodies/utilities.c src/include/headers/utilities.h
	$(CCOMPILE) -c $< -o obj/$@

# Librería estática propia: servers_setup
lib_servers_setup.a: servers_setup.o
	$(SLIBF) slib/$@ obj/$<

servers_setup.o: src/include/bodies/servers_setup.c src/include/headers/servers_setup.h
	$(CCOMPILE) -c $< -o obj/$@

# Librería estática propia: clients_setup
lib_clients_setup.a: clients_setup.o
	$(SLIBF) slib/$@ obj/$<

clients_setup.o: src/include/bodies/clients_setup.c src/include/headers/clients_setup.h
	$(CCOMPILE) -c $< -o obj/$@

# Binario del servidor
srv: srv.o lib_utilities.a lib_servers_setup.a
	$(CCOMPILE) -o bin/$@ obj/$< slib/lib_utilities.a slib/lib_servers_setup.a

srv.o: src/server.c
	$(CCOMPILE) -c $< -o obj/$@

# Binario del cliente
cln: cln.o lib_utilities.a lib_clients_setup.a
	$(CCOMPILE) -o bin/$@ obj/$< slib/lib_utilities.a slib/lib_clients_setup.a

cln.o: src/client.c
	$(CCOMPILE) -c $< -o obj/$@

# Limpieza de archivos y carpetas creados
clean:
	rm -r $(DIRS)