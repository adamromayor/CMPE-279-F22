// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>

#define PORT 8080
int main(int argc, char *const argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[102] = {0};
    char new_socket_buffer[50] = {0};
    char *hello = "Hello from server";

    printf("execve=0x%p\n", execve);

    int c, sflag = 0;

    while ((c = getopt(argc, argv, "s:")) != -1)
    {
        switch (c)
        {
        case 's':
            // socket flag
            printf("Flag Set\n");
            sflag = 1;
            new_socket = atoi(optarg);
            break;

        default:
            break;
        }
    }

    if (sflag == 0)
    {
        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        // Attaching socket to port 80
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                       &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        // Forcefully attaching socket to the port 80
        if (bind(server_fd, (struct sockaddr *)&address,
                 sizeof(address)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // PRIVILEGE SEPATATION

        pid_t pid;
        int status;

        // convert new_socket to string
        sprintf(new_socket_buffer, "%d", new_socket);
        char *nargv[] = {argv[0], "-s", new_socket_buffer, NULL};
        switch (pid = fork())
        {
        case 0:
            printf("In Child: %d\n", pid);
            if (execvp(argv[0], nargv) == -1)
            {
                perror("execvp");
            }
            break;

        default:
            status = 0;
            printf("In Parent: %d\n", pid);
            waitpid(pid, &status, 0);
            break;
        }
    }
    else
    {
        // Privilege Dropping
        struct passwd *pwd = getpwnam("nobody");
        printf("'nobody' uid: %u\n", pwd->pw_uid);
        setuid(pwd->pw_uid);
        valread = read(new_socket, buffer, 1024);
        printf("%s\n", buffer);
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");
    }

    return 0;
}
