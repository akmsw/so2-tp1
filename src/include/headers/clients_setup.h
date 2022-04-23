/**
 * @file clients_setup.h
 * @author Bonino, Francisco Ignacio (franbonino82@gmail.com).
 * @brief Header de librería con funciones de creación de clientes de
 *        distintos protocolos para el TP #1 de Sistemas Operativos II.
 * @version 0.5
 * @since 2022-03-23
 */

#ifndef __SERVERS__
#define __SERVERS__

/* ---------- Librerías a utilizar -------------- */

#include "utilities.h"

#include <arpa/inet.h>
#include <net/if.h>

/* ---------- Definición de constantes ---------- */

#define _IPV4_ "ipv4"
#define _IPV6_ "ipv6"
#define _LOCAL_ "local"

/* ---------- Definición de variables ----------- */

int socket_fd;

/* ---------- Prototipado de funciones ---------- */

void handler(int);
void run_ipv4_cl(char *, uint16_t, int);
void run_ipv6_cl(char *, char *, uint16_t, int);
void run_local_cl(char *, int);

#endif