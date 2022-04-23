/**
 * @file servers_setup.c
 * @author Bonino, Francisco Ignacio (franbonino82@gmail.com)
 * @brief Librería con funciones de creación de servidores de distintos
 *        protocolos para el TP #1 de Sistemas Operativos II.
 * @version 0.4.3
 * @since 2022-03-23
 */

#include "../headers/servers_setup.h"

/**
 * @brief Este método se encarga de reiniciar las
 *        estadísticas de datos recibidos en
 *        cada uno de los protocolos del servidor.
 *
 * @param sd Puntero a estructura de velocidades de comunicación a reiniciar.
 */
void stats_reset(struct_data *sd)
{
    sd->local = 0;
    sd->ipv4 = 0;
    sd->ipv6 = 0;
    sd->total = 0;
}

/**
 * @brief Este método se encarga de sumar las
 *        estadísticas de datos recibidos en
 *        cada uno de los protocolos del servidor.
 *
 * @param sd Puntero a estructura de velocidades de comunicación
 *           de la cual se obtendrán los resultados.
 */
void stats_sum(struct_data *sd)
{
    long int sum = 0;

    sum += sd->local;
    sum += sd->ipv4;
    sum += sd->ipv6;

    sd->total = sum;
}

/**
 * @brief Se inicializa la conexión TCP/IPv4.
 *
 * @details Se inicia la conexión TCP/IPv4 y por cada
 *          cliente que se conecte al servidor mediante
 *          este protocolo, se crea un hilo hijo para
 *          escuchar los mensajes que el cliente envíe.
 *
 * @param port Número de puerto a utilizar para la conexión.
 * @param acc Acumulador donde se registrarán la cantidad de bytes
 *            recibidos en los mensajes de los clientes conectados.
 */
void startup_ipv4_sv(uint16_t port, long int *acc)
{
    struct sockaddr_in struct_sv;
    struct sockaddr_in struct_cl;

    socklen_t client_len = sizeof(struct_cl);

    int socket_fd;

    char buffer[_MAX_BUFF_SIZE_];

    // Creación del socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed in socket creation {IPv4}");

    if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
        show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed trying to set port as reusable {IPv6}");

    // Inicialización de la estructura del servidor
    memset(&struct_sv, 0, sizeof(struct_sv));

    struct_sv.sin_family = AF_INET;
    struct_sv.sin_addr.s_addr = INADDR_ANY;
    struct_sv.sin_port = htons(port);

    // Binding del socket del server
    if (bind(socket_fd, (struct sockaddr *)&struct_sv, sizeof(struct_sv)) == -1)
        show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed binding socket {IPv4}");

    if (listen(socket_fd, 5) == -1) // Máximo 5 clientes en espera simultánea
        show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed trying to listen to socket {IPv4}");

    fprintf(stdout, "[PID: %d] <SERVER@IPv4> Available port: %d\n", getpid(), htons(struct_sv.sin_port));

    while (1)
    {
        // Se esperan conexiones
        int cl_socket_fd = accept(socket_fd, (struct sockaddr *)&struct_cl, &client_len);

        if (cl_socket_fd == -1)
            show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed trying to accept client {IPv4}");

        // Por cada conexión, se crea un proceso hijo que reciba los mensajes
        int ch_pid = fork();

        if (ch_pid == -1)
            show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed on process forking for client handling {IPv4}");

        if (ch_pid == 0)
        {
            // Proceso hijo
            close(socket_fd);

            while (1)
            {
                memset(buffer, 0, _MAX_BUFF_SIZE_);

                ssize_t aux = read(cl_socket_fd, buffer, (_MAX_BUFF_SIZE_ - 1));

                if (aux == -1)
                    show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed receiving message {IPv4}");

                if (strcmp(buffer, _EOT_MSG_) == 0)
                {
                    close(cl_socket_fd);

                    exit(EXIT_FAILURE);
                }

                *acc += aux;
            }
        }
        else
        {
            // Proceso padre
            fprintf(stdout, "[PID: %d] <SERVER@IPv4> New client accepted, managed by subprocess #%d.\n", getpid(), ch_pid);

            close(cl_socket_fd);
        }
    }
}

/**
 * @brief Se inicializa la conexión TCP/IPv6.
 *
 * @details Se inicia la conexión TCP/IPv6 y por cada
 *          cliente que se conecte al servidor mediante
 *          este protocolo, se crea un hilo hijo para
 *          escuchar los mensajes que el cliente envíe.
 *
 * @param port Número de puerto a utilizar para la conexión.
 * @param acc Acumulador donde se registrarán la cantidad de bytes
 *            recibidos en los mensajes de los clientes conectados.
 */
void startup_ipv6_sv(uint16_t port, long int *acc)
{
    struct sockaddr_in6 struct_sv;
    struct sockaddr_in6 struct_cl;

    socklen_t client_len = sizeof(struct_cl);

    int socket_fd;

    char buffer[_MAX_BUFF_SIZE_];

    // Creación del socket
    if ((socket_fd = socket(AF_INET6, SOCK_STREAM, 0)) == -1)
        show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed in socket creation {IPv6}");

    if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
        show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed trying to set port as reusable {IPv6}");

    // Inicialización de la estructura del servidor
    memset(&struct_sv, 0, sizeof(struct_sv));

    struct_sv.sin6_family = AF_INET6;
    struct_sv.sin6_port = (in_port_t)htons(port);
    struct_sv.sin6_addr = in6addr_any;

    // Binding del socket del server
    if (bind(socket_fd, (struct sockaddr *)&struct_sv, sizeof(struct_sv)) == -1)
        show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed binding socket {IPv6}");

    if (listen(socket_fd, 5) == -1) // Máximo 5 clientes en espera simultánea
        show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed trying to listen to socket {IPv6}");

    fprintf(stdout, "[PID: %d] <SERVER@IPv6> Available port: %d\n", getpid(), htons(struct_sv.sin6_port));

    while (1)
    {
        // Se esperan conexiones
        int cl_socket_fd = accept(socket_fd, (struct sockaddr *)&struct_cl, &client_len);

        if (cl_socket_fd == -1)
            show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed trying to accept client {IPv6}");

        // Por cada conexión, se crea un proceso hijo que reciba los mensajes
        int cp_ipv6_pid = fork();

        if (cp_ipv6_pid == -1)
            show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed on process forking for client handling {IPv6}");

        if (cp_ipv6_pid == 0)
        {
            // Proceso hijo
            close(socket_fd);

            while (1)
            {
                memset(buffer, 0, _MAX_BUFF_SIZE_);

                ssize_t aux = read(cl_socket_fd, buffer, (_MAX_BUFF_SIZE_ - 1));

                if (aux == -1)
                    show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed receiving message {IPv6}");

                if (strcmp(buffer, _EOT_MSG_) == 0)
                {
                    close(cl_socket_fd);

                    exit(EXIT_FAILURE);
                }

                *acc += aux;
            }
        }
        else
        {
            // Proceso padre
            fprintf(stdout, "[PID: %d] <SERVER@IPv6> New client accepted, managed by child process #%d.\n", getpid(), cp_ipv6_pid);

            close(cl_socket_fd);
        }
    }
}

/**
 * @brief Se inicializa la conexión TCP local.
 *
 * @param socket_file Nombre del archivo a utilizar para la
 *                    comunicación entre cliente y servidor.
 * @param acc Acumulador donde se registrarán la cantidad de bytes
 *            recibidos en los mensajes de los clientes conectados.
 */
void startup_local_sv(char *socket_file, long int *acc)
{
    unlink(socket_file); // Desligamos el archivo en caso de ya existir de corridas anteriores

    struct sockaddr_un struct_sv;
    struct sockaddr_un struct_cl;

    socklen_t sv_len;
    socklen_t client_len = sizeof(struct_cl);

    int socket_fd;

    char buffer[_MAX_BUFF_SIZE_];

    // Creación del socket
    if ((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed in socket creation {LOCAL}");

    // Inicialización de la estructura del servidor
    memset(&struct_sv, 0, sizeof(struct_sv));
    struct_sv.sun_family = AF_UNIX;
    strcpy(struct_sv.sun_path, socket_file);

    sv_len = (socklen_t)(strlen(struct_sv.sun_path) + sizeof(struct_sv.sun_family));

    // Binding del socket del server
    if (bind(socket_fd, (struct sockaddr *)&struct_sv, sv_len) == -1)
        show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed binding socket {LOCAL}");

    if (listen(socket_fd, 5) == -1) // Máximo 5 clientes en espera simultánea
        show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed trying to listen to socket {LOCAL}");

    fprintf(stdout, "[PID: %d] <SERVER@LOCAL> Available socket: %s\n", getpid(), struct_sv.sun_path);

    while (1)
    {
        // Se esperan conexiones
        int cl_socket_fd = accept(socket_fd, (struct sockaddr *)&struct_cl, &client_len);

        if (cl_socket_fd == -1)
            show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed trying to accept client {LOCAL}");

        // Por cada conexión, se crea un proceso hijo que reciba los mensajes
        int cp_ipv4_pid = fork();

        if (cp_ipv4_pid == -1)
            show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed on process forking for client handling {LOCAL}");

        if (cp_ipv4_pid == 0)
        {
            // Proceso hijo
            close(socket_fd);

            while (1)
            {
                memset(buffer, 0, _MAX_BUFF_SIZE_);

                ssize_t aux = read(cl_socket_fd, buffer, (_MAX_BUFF_SIZE_ - 1));

                if (aux == -1)
                    show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed receiving message {LOCAL}");

                if (strcmp(buffer, _EOT_MSG_) == 0)
                {
                    close(cl_socket_fd);

                    exit(EXIT_FAILURE);
                }

                *acc += aux;
            }
        }
        else
        {
            // Proceso padre
            fprintf(stdout, "[PID: %d] <SERVER@LOCAL> New client accepted managed by child process #%d.\n", getpid(), cp_ipv4_pid);

            close(cl_socket_fd);
        }
    }
}