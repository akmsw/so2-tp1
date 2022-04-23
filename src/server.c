/**
 * @file server.c
 * @author Bonino, Francisco Ignacio (franbonino82@gmail.com)
 * @brief Servidor para el TP #1 de Sistemas Operativos II.
 * @version 1.3.1
 * @since 2022-03-20
 */

#include "include/headers/servers_setup.h"

/**
 * @brief Función principal del servidor.
 *
 * @param argc Cantidad de argumentos recibidos.
 * @param argv Vector con los argumentos recibidos.
 *
 * @return 0 Si la ejecución del servidor fue exitosa.
 *         1 Si la ejecución del servidor tuvo errores.
 */
int main(int argc, char *argv[])
{
    int parent_pid = getpid();

    // Validación de argumentos
    if (argc == 2)
    {
        if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0) || (*argv[1] == '?'))
        {
            show_help();

            exit(EXIT_SUCCESS);
        }
        else if ((strcmp(argv[1], "-e") == 0) || (strcmp(argv[1], "--examples") == 0) || (*argv[1] == '!'))
        {
            show_examples();

            exit(EXIT_SUCCESS);
        }
        else
            show_err(parent_pid, _SERVER_SRC_, _FATAL_ERR_, "Invalid arguments amount. Run this program with '-h', '--help' or '?' for help");
    }

    if ((argc != _SV_PARAMS_) && argc != (_SV_PARAMS_ - 1))
        show_err(parent_pid, _SERVER_SRC_, _FATAL_ERR_, "Invalid arguments amount. Run this program with '-h', '--help' or '?' for help");

    /*
     * Según 'The Linux programming interface', al ignorar la señal SIGCHLD
     * se logra que cualquier sub-proceso (del proceso actual) que termine
     * sea removido del sistema en lugar de convertirse en un proceso zombie.
     */
    if (signal(SIGCHLD, SIG_IGN) == SIG_ERR)
        show_err(parent_pid, _SERVER_SRC_, _FATAL_ERR_, "Failed trying to ignore signal SIGCHLD");

    // Creación de memoria compartida para estructura de velocidades de comunicación
    key_t key = 11111; // Key arbitraria - consultar ftok

    int smid = shmget(key, sizeof(struct_data), (IPC_CREAT | 0666));

    if (smid == -1)
        show_err(parent_pid, _SERVER_SRC_, _FATAL_ERR_, "Failed on shared memory creation process [creation]");

    struct_data *sd = (struct_data *)shmat(smid, 0, 0);

    if (sd == (void *)-1)
        show_err(parent_pid, _SERVER_SRC_, _FATAL_ERR_, "Failed on shared memory creation process [attachment]");

    /* ----------------- SOCKET LOCAL ----------------- */

    int cp_local_pid = fork();

    if (cp_local_pid == -1)
        show_err(parent_pid, _SERVER_SRC_, _FATAL_ERR_, "Failed on process forking for IPv6 socket");

    if (cp_local_pid == 0) // Proceso hijo - Creación de socket local
        startup_local_sv(argv[1], &sd->local);

    /* ----------------- SOCKET IPv4 ----------------- */

    int cp_ipv4_pid = fork();

    if (cp_ipv4_pid == -1)
        show_err(parent_pid, _SERVER_SRC_, _FATAL_ERR_, "Failed on process forking for IPv4 socket");

    if (cp_ipv4_pid == 0) // Proceso hijo - Creación de socket TCP/IPv4
        startup_ipv4_sv((uint16_t)atoi(argv[2]), &sd->ipv4);

    /* ----------------- SOCKET IPv6 ----------------- */

    int cp_ipv6_pid = fork();

    if (cp_ipv6_pid == -1)
        show_err(parent_pid, _SERVER_SRC_, _FATAL_ERR_, "Failed on process forking for IPv6 socket");

    if (cp_ipv6_pid == 0) // Proceso hijo - Creación de socket TCP/IPv6
        startup_ipv6_sv((uint16_t)atoi(argv[3]), &sd->ipv6);

    /* --------------------- LOG --------------------- */

    // El tiempo de log será de un segundo por defecto
    unsigned int log_interval;

    if ((argc == _SV_PARAMS_) && (atoi(argv[4]) > 0))
        log_interval = (unsigned int)atoi(argv[4]);
    else
        log_interval = 1;

    // Creación del archivo de log
    FILE *log = fopen("src/resources/log/log.txt", "w");

    if (!log)
        show_err(parent_pid, _SERVER_SRC_, _FATAL_ERR_, "Failed trying to create log file");

    if (fclose(log) != 0)
        show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed trying to close log file");

    /*
     * El archivo de log sólo se abre luego de que pase el tiempo establecido entre lecturas,
     * se le escribe la información sobre la velocidad de cada tipo de conexión, y antes de
     * volver a dormir se lo cierra.
     */
    while (1)
    {
        sleep(log_interval);

        log = fopen("src/resources/log/log.txt", "w");

        stats_sum(sd);

        if (!log)
            show_err(parent_pid, _SERVER_SRC_, _FATAL_ERR_, "Failed trying to open log file");

        if (fprintf(log, "Local TCP speed: %ld[MB/s]\nTCP/IPv4 speed: %ld[MB/s]\nTCP/IPv6 speed: %ld[MB/s]\n\nTotal speed: %ld[MB/s]",
                    (((sd->local * 8) / 1000000) / log_interval),
                    (((sd->ipv4 * 8) / 1000000) / log_interval),
                    (((sd->ipv6 * 8) / 1000000) / log_interval),
                    (((sd->total * 8) / 1000000)) / log_interval) < 0)
            show_err(parent_pid, _SERVER_SRC_, _FATAL_ERR_, "Failed trying to write in log file");

        if (fclose(log) != 0)
            show_err(getpid(), _SERVER_SRC_, _FATAL_ERR_, "Failed trying to close log file");

        stats_reset(sd);
    }

    return 0;
}