/**
 * @file utilities.h
 * @author Bonino, Francisco Ignacio (franbonino82@gmail.com).
 * @brief Header de librería con funciones útiles generales
 *        adaptada para el TP #1 de Sistemas Operativos II.
 * @version 4.9.1
 * @since 2021-08-18
 */

#ifndef __UTILITIES__
#define __UTILITIES__

/* ---------- Librerías a utilizar -------------- */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>

/* ---------- Definición de constantes ---------- */

#define _NORM_ERR_ 0
#define _FATAL_ERR_ 1

#define _CLIENT_SRC_ 0
#define _SERVER_SRC_ 1
#define _GENERAL_SRC_ 2

#define _EOT_MSG_ "STOP" // End-Of-Transmission message

#define _MAX_BUFF_SIZE_ 10000

/* ---------- Prototipado de funciones ---------- */

void show_err(int, int, int, char *);
void show_examples(void);
void show_help(void);

void try_kill(int, int);
void try_write(int, char *);

char *itoa(int, char[]);
char *mk_err_msg(int, int, int, char *);

#endif