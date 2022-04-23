/**
 * @file servers_setup.h
 * @author Bonino, Francisco Ignacio (franbonino82@gmail.com).
 * @brief Header de librería con funciones de creación de servidores de
 *        distintos protocolos para el TP #1 de Sistemas Operativos II.
 * @version 0.6
 * @since 2022-03-23
 */

#ifndef __SERVERS__
#define __SERVERS__

/* ---------- Librerías a utilizar -------------- */

#include "utilities.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>

/* ---------- Definición de constantes ---------- */

#define _SV_PARAMS_ 5 // Cantidad máxima de argumentos para el servidor

/* ---------- Definición de estructuras --------- */

typedef struct struct_data
{
    long int local;
    long int ipv4;
    long int ipv6;
    long int total;
} struct_data;

/* ---------- Prototipado de funciones ---------- */

void stats_reset(struct_data *);
void stats_sum(struct_data *);
void startup_ipv4_sv(uint16_t, long int *);
void startup_ipv6_sv(uint16_t, long int *);
void startup_local_sv(char *, long int *);

#endif