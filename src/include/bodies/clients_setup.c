/**
 * @file clients_setup.c
 * @author Bonino, Francisco Ignacio (franbonino82@gmail.com)
 * @brief Librería con funciones de creación de clientes de distintos
 *        protocolos para el TP #1 de Sistemas Operativos II.
 * @version 0.5
 * @since 2022-03-23
 */

#include "../headers/clients_setup.h"

/**
 * @brief Creación y ejecución de cliente con conexión
 *        TCP/IPv4.
 *
 * @param hostname Dirección del host a conectarse.
 * @param port Puerto del host a conectarse.
 * @param buffer_size Tamaño de los mensajes a enviar.
 */
void run_ipv4_cl(char *address, uint16_t port, int buffer_size)
{
    if (buffer_size > _MAX_BUFF_SIZE_)
        show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Buffer size is greater than allowed. Run this program with '-h', '--help' or '?' for help");

    char buffer[buffer_size];

    for (int i = 0; i < buffer_size - 1; i++)
        buffer[i] = 'b';

    buffer[buffer_size - 1] = '\0';

    struct sockaddr_in struct_sv;

    struct hostent *server;

    // Validación de dirección IPv4
    if (!(server = gethostbyname(address)))
        show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Failed resolving IPv4 address {IPv4}");

    // Creación del socket para el cliente
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Failed creating client socket {IPv4}");

    // Inicialización de la estructura del servidor
    memset(&struct_sv, 0, sizeof(struct_sv));
    memset(&struct_sv.sin_zero, '\0', 8);
    bcopy((char *)server->h_addr_list[0], (char *)&struct_sv.sin_addr.s_addr, (size_t)server->h_length);
    struct_sv.sin_family = AF_INET;
    struct_sv.sin_port = (in_port_t)htons(port);

    // Conexión cliente-servidor
    if (connect(socket_fd, (struct sockaddr *)&struct_sv, sizeof(struct_sv)) == -1)
        show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Failed connecting to socket {IPv4}");

    while (1)
        if (send(socket_fd, buffer, strlen(buffer), 0) == -1)
            show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Failed sending message {IPv4}");
}

/**
 * @brief Creación y ejecución de cliente con conexión
 *        TCP/IPv6.
 *
 * @param address Dirección del host a conectarse.
 * @param interface Interfaz del host a conectarse.
 * @param port Puerto del host a conectarse.
 * @param buffer_size Tamaño de los mensajes a enviar.
 */
void run_ipv6_cl(char *address, char *interface, uint16_t port, int buffer_size)
{
    if (buffer_size > _MAX_BUFF_SIZE_)
        show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Buffer size is greater than allowed. Run this program with '-h', '--help' or '?' for help");

    char buffer[buffer_size];

    for (int i = 0; i < buffer_size - 1; i++)
        buffer[i] = 'c';

    buffer[buffer_size - 1] = '\0';

    struct sockaddr_in6 struct_sv;

    // Creación del socket para el cliente
    if ((socket_fd = socket(AF_INET6, SOCK_STREAM, 0)) == -1)
        show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Failed creating client socket {IPv6}");

    // Inicialización de la estructura del servidor
    struct_sv.sin6_family = AF_INET6;
    struct_sv.sin6_port = (in_port_t)htons(port);
    struct_sv.sin6_scope_id = if_nametoindex(interface);

    // Validación de dirección IPv6
    if (inet_pton(AF_INET6, address, &struct_sv.sin6_addr) != 1)
        show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Failed resolving IPv6 address {IPv6}");

    // Conexión cliente-servidor
    if (connect(socket_fd, (struct sockaddr *)&struct_sv, sizeof(struct_sv)) == -1)
        show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Failed connecting socket {IPv6}");

    while (1)
        if (send(socket_fd, buffer, strlen(buffer), 0) == -1)
            show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Failed sending message {IPv6}");
}

/**
 * @brief Creación y ejecución de cliente con conexión
 *        TCP local.
 *
 * @param socket_filename Nombre del archivo local a usar como
 *                        socket entre servidor y cliente.
 * @param buffer_size Tamaño de los mensajes a enviar.
 */
void run_local_cl(char *socket_filename, int buffer_size)
{
    if (buffer_size > _MAX_BUFF_SIZE_)
        show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Buffer size is greater than allowed. Run this program with '-h', '--help' or '?' for help");

    socklen_t sv_len;

    char buffer[buffer_size];

    for (int i = 0; i < buffer_size - 1; i++)
        buffer[i] = 'a';

    buffer[buffer_size - 1] = '\0';

    struct sockaddr_un struct_sv;

    // Creación del socket para el cliente
    if ((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Failed creating socket {LOCAL}");

    // Inicialización de la estructura del cliente
    memset(&struct_sv, 0, sizeof(struct_sv));

    struct_sv.sun_family = AF_UNIX;

    strcpy(struct_sv.sun_path, socket_filename);

    sv_len = (socklen_t)(strlen(struct_sv.sun_path) + sizeof(struct_sv.sun_family));

    // Conexión cliente-servidor
    if (connect(socket_fd, (struct sockaddr *)&struct_sv, sv_len) == -1)
        show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Failed connecting socket {LOCAL}");

    while (1)
        if (send(socket_fd, buffer, strlen(buffer), 0) == -1)
            show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Failed sending message {LOCAL}");
}