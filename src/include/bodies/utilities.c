/**
 * @file utilities.c
 * @author Bonino, Francisco Ignacio (franbonino82@gmail.com).
 * @brief Librería con funciones útiles generales adaptada
 *        para el TP #1 de Sistemas Operativos II.
 * @version 4.6.2
 * @since 2021-08-18
 */

#include "../headers/utilities.h"

/**
 * @brief Esta función muestra el mensaje solicitado en pantalla
 *        y termina la ejecución del programa si es necesario.
 *
 * @param pid ID del proceso que mostrará el error.
 * @param source Fuente del error.
 * @param err_type Gravedad del error.
 * @param msg Mensaje a mostrar.
 */
void show_err(int pid, int source, int err_type, char *msg)
{
    try_write(STDERR_FILENO, mk_err_msg(pid, source, err_type, msg));

    if (err_type == _FATAL_ERR_)
        exit(EXIT_FAILURE);
}

/**
 * @brief Esta función muestra un mensaje de ayuda para
 *        iniciar tanto un cliente como un servidor.
 */
void show_examples()
{
    // +817 por el largo del mensaje
    char *h_msg = malloc((sizeof(char) * 817) + sizeof(NULL));

    if (!h_msg)
        show_err(getpid(), _GENERAL_SRC_, _FATAL_ERR_, "Failed in memory allocation");

    strcpy(h_msg, "///////////////////////////////////////////////////////////////////   E X A M P L E S   //////////////////////////////////////////////////////////////////\n\n\
These examples are provided assuming the correct project compilation, standing in the project's root folder.\n\n\
<SERVER>\n\n\
    ./bin/srv my_socket 2222 5000\n\n\
<CLIENT>\n\n\
    ./bin/cln local my_socket 100\n\
    ./bin/cln ipv4 localhost 2222 50\n\
    ./bin/cln ipv4 [$HOSTNAME] 2222 77\n\
    ./bin/cln ipv4 127.0.0.1 2222 12\n\
    ./bin/cln ipv4 [IPv4 address] 2222 3\n\
    ./bin/cln ipv6 ::1 lo 5000 37\n\
    ./bin/cln ipv6 [IPv6 address] [interface] 5000 242\n\n\
For more help, run this program with '-h', '--help', or '?'.\n\n\
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n");

    try_write(STDOUT_FILENO, h_msg);

    free(h_msg);
}

/**
 * @brief Esta función muestra un mensaje de ayuda acerca
 *        de los argumentos esperados para inicializar
 *        tanto el servidor como los clientes.
 */
void show_help()
{
    char max_buff_size_str[5];

    itoa(_MAX_BUFF_SIZE_, max_buff_size_str);

    // +2582 por el largo del mensaje
    char *h_msg = malloc(strlen(max_buff_size_str) + (sizeof(char) * 2582) + sizeof(NULL));

    if (!h_msg)
        show_err(getpid(), _GENERAL_SRC_, _FATAL_ERR_, "Failed in memory allocation");

    strcpy(h_msg, "///////////////////////////////////////////////////////////////////////   H E L P   //////////////////////////////////////////////////////////////////////\n\n\
<SERVER>\n\
    In order to setup the server correctly, the user must provide the following arguments:\n\n\
        First argument:\n\
            Local TCP/IP connection socket file name.\n\
        Second argument:\n\
            TCP/IPv4 port number.\n\
        Third argument:\n\
            TCP/IPv6 port number.\n\
        Fourth argument (optional):\n\
            Logging time interval (in seconds).\n\n\
<CLIENT>\n\
    In order to setup the client correctly, the user must provide the following arguments:\n\n\
        First argument:\n\
            Client's connection protocol.\n\
                Valid protocols:\n\
                    local: The client will be connected to the server via local TCP/IP.\n\
                    ipv4: The client will be connected to the server via TCP/IPv4.\n\
                    ipv6: The client will be connected to the server via TCP/IPv6.\n\
        Second argument:\n\
            If the client is connected via local TCP/IP, the second argument must be the socket file name.\n\
            If the client is connected via TCP/IPvX, the second argument must be the server IPvX address.\n\
        Third argument:\n\
            If the client is connected via local TCP/IP, the third argument must be the size of the buffer to be sent, and no more arguments are needed.\n\
            If the client is connected via TCP/IPv4, the third argument must be the IPv4 port used for the connection.\n\
            If the client is connected via TCP/IPv6, the third argument must be the IPv6 interface.\n\
        If the client is connected via TCP/IPv4, the fourth argument must be:\n\
            The size of the buffer to be sent, and no more arguments are needed.\n\
        If the client is connected via TCP/IPv6, the fourth argument must be:\n\
            The IPv6 port used for the connection.\n\
        If the client is connected via TCP/IPv6, the fifth argument must be:\n\
            The size of the buffer to be sent, and no more arguments are needed.\n\n\
The maximum buffer size allowed is 10000.\n\n\
If the user does not provide a logging time interval, or enters a negative number, or enters a number less or equal to zero, or the input is not\n\
a number, the logging interval will be set to its default value of 1 second between logs.\n\n\
For execution examples, run this program with '-e', '--examples', or '!'.\n\n\
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n");

    try_write(STDOUT_FILENO, h_msg);

    free(h_msg);
}

/**
 * @brief Esta función se encarga de intentar matar un proceso
 *        especificado mediante su ID. Si no se logra, se
 *        imprime un mensaje de error y se aborta el programa.
 *
 * @param pid ID del proceso a matar.
 * @param signal Señal a enviar al proceso.
 */
void try_kill(int pid, int signal)
{
    if (kill(pid, signal) == -1)
        show_err(getpid(), _GENERAL_SRC_, _FATAL_ERR_, "Could not kill process");
}

/**
 * @brief Esta función se encarga de intentar escribir un
 *        mensaje en pantalla de manera thread-safe.
 *        Si no se logra, se imprime un mensaje de error
 *        y se aborta el programa.
 *
 * @param stdx Stream en el que se escribirá el mensaje.
 * @param msg Mensaje a mostrar.
 */
void try_write(int stdx, char *msg)
{
    if (write(stdx, msg, strlen(msg)) == -1)
    {
        fprintf(stderr, "\nFatal error on write instruction --- ABORTING\n");

        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Esta función convierte un número
 *        entero a una cadena de caracteres.
 *
 * @param num Número entero a convertir.
 * @param buff Buffer donde se almacenará el resultado.
 *
 * @return El buffer con la cadena de caracteres.
 */
char *itoa(int num, char buff[])
{
    char const digit[] = "0123456789";

    char *p = buff;

    if (num < 0)
    {
        *p++ = '-';

        num *= (-1);
    }

    int shifter = num;

    do
    {
        ++p;

        shifter /= 10;
    } while (shifter);

    *p = '\0';

    do
    {
        *--p = digit[num % 10];

        num /= 10;
    } while (num);

    return buff;
}

/**
 * @brief Esta función se encarga de armar un mensaje de error.
 *
 * @details Se almacena memoria inicializada con ceros para evitar
 *          error con strcat. Se sobrescribe con el formato
 *          correspondiente para un mensaje de error y el mensaje
 *          que indica la falla, recibido por parámetro.
 *
 * @param pid ID del proceso que mostrará el error.
 * @param source Fuente del error.
 * @param err_type Gravedad del error.
 * @param msg Mensaje a mostrar.
 *
 * @return El mensaje de error formateado con el texto recibido por parámetro.
 */
char *mk_err_msg(int pid, int source, int err_type, char *msg)
{
    char *err_msg;
    char aux[10];

    if (snprintf(aux, sizeof(aux), "%d", pid) < 0)
    {
        fprintf(stderr, "\nFatal snprintf error --- ABORTING\n");

        exit(EXIT_FAILURE);
    }

    if (err_type == _FATAL_ERR_)
    {
        // +9 por "\n[PID : ", +2 por "] ", +7 por "CLIENT" o "SERVER", +23 por "> [[ FATAL ERROR ]] : " y +4 por "\n\n"
        err_msg = (char *)calloc(strlen(msg) + strlen(aux) + sizeof(NULL) + (sizeof(char) * 45), sizeof(char));

        if (!err_msg)
            show_err(getpid(), _GENERAL_SRC_, _FATAL_ERR_, "Failed in memory allocation");

        strcpy(err_msg, "\n[PID: ");
        strcat(err_msg, aux);
        strcat(err_msg, "] <");

        if (source == _SERVER_SRC_)
            strcat(err_msg, "SERVER");
        else if (source == _CLIENT_SRC_)
            strcat(err_msg, "CLIENT");
        else
            strcat(err_msg, "GENERAL");

        strcat(err_msg, "> [[ FATAL ERROR ]] : ");
    }
    else
    {
        // +9 por "\n[PID : ", +2 por "] ", +7 por "CLIENT" o "SERVER", +17 por "> [[ ERROR ]] : " y +4 por "\n\n"
        err_msg = (char *)calloc(strlen(msg) + strlen(aux) + sizeof(NULL) + (sizeof(char) * 39), sizeof(char));

        if (!err_msg)
            show_err(getpid(), _GENERAL_SRC_, _FATAL_ERR_, "Failed in memory allocation");

        strcpy(err_msg, aux);

        if (source == _SERVER_SRC_)
            strcat(err_msg, "SERVER");
        else if (source == _CLIENT_SRC_)
            strcat(err_msg, "CLIENT");
        else
            strcat(err_msg, "GENERAL");

        strcat(err_msg, "> [[ ERROR ]] : ");
    }

    strcat(err_msg, msg);
    strcat(err_msg, ".\n\n");

    return err_msg;
}