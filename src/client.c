/**
 * @file client.c
 * @author Bonino, Francisco Ignacio (franbonino82@gmail.com)
 * @brief Generador de clientes de distintos protocolos para
 *        el TP #1 de Sistemas Operativos II.
 * @version 0.9.5
 * @since 2022-03-20
 */

#include "include/headers/clients_setup.h"

/**
 * @brief Función principal del cliente.
 *
 * @param argc Cantidad de argumentos recibidos.
 * @param argv Vector con los argumentos recibidos.
 *
 * @return 0 Si la ejecución del cliente fue exitosa.
 *         1 Si la ejecución del cliente tuvo errores.
 */
int main(int argc, char *argv[])
{
    // Validación de argumentos
    if (argc == 2)
    {
        if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0) || (*argv[1] == '?'))
        {
            show_help();

            exit(EXIT_FAILURE);
        }
        else if ((strcmp(argv[1], "-e") == 0) || (strcmp(argv[1], "--examples") == 0) || (*argv[1] == '!'))
        {
            show_examples();

            exit(EXIT_SUCCESS);
        }
        else
            show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Invalid argument received. Run this program with '-h', '--help' or '?' for help");
    }

    // Se asigna un handler particular a SIGINT para cerrar la comunicación correctamente
    if (signal(SIGINT, handler) == SIG_ERR)
        show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed trying to assign handler to signal SIGCHLD");

    // Sólo se permite matar al cliente mediante SIGINT o SIGKILL
    if ((signal(SIGTSTP, SIG_IGN) == SIG_ERR) ||
        (signal(SIGQUIT, SIG_IGN) == SIG_ERR))
        show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed trying to ignore signals SIGTSTP and SIGQUIT");

    // Creación de cliente en base al protocolo especificado
    char *protocol = argv[1];

    if (strcmp(protocol, _LOCAL_) == 0)
    {
        if (atoi(argv[3]) > _MAX_BUFF_SIZE_)
            show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Buffer size is greater than allowed. Run this program with '-h', '--help' or '?' for help");

        run_local_cl(argv[2], atoi(argv[3]));
    }
    else if (strcmp(protocol, _IPV4_) == 0)
    {
        if (atoi(argv[4]) > _MAX_BUFF_SIZE_)
            show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Buffer size is greater than allowed. Run this program with '-h', '--help' or '?' for help");

        run_ipv4_cl(argv[2], (uint16_t)atoi(argv[3]), atoi(argv[4]));
    }
    else if (strcmp(protocol, _IPV6_) == 0)
    {
        if (atoi(argv[5]) > _MAX_BUFF_SIZE_)
            show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "Buffer size is greater than allowed. Run this program with '-h', '--help' or '?' for help");

        run_ipv6_cl(argv[2], argv[3], (uint16_t)atoi(argv[4]), atoi(argv[5]));
    }
    else
        show_err(getpid(), _CLIENT_SRC_, _FATAL_ERR_, "At least one argument received is invalid. Run this program with '-h', '--help' or '?' for help");

    return 0;
}

/**
 * @brief Handler para señales SIGINT.
 *
 * @details Se envía al server el mensaje "STOP"
 *          y automáticamente se cierra el socket
 *          para cerrar la comunicación del lado
 *          del cliente.
 *
 * @param signal Señal recibida.
 */
void handler(int signal)
{
    fprintf(stdout, "\n[PID: %d] <CLIENT> [[ EXITING ]] : Signal %d received {SIGINT}\n", getpid(), signal);

    send(socket_fd, _EOT_MSG_, strlen(_EOT_MSG_), 0);

    close(socket_fd);

    exit(EXIT_FAILURE);
}