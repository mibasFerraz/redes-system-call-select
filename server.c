#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define STDIN 0
#define MAX_ROOMS 100

fd_set master, read_fds;
struct sockaddr_in myaddr, remoteaddr;
int fdmax, newfd, nbytes, yes = 1, addrlen;
char buff[256];

typedef struct
{
    int client_sock;
    char name[256];
    int active;
} Client;

typedef struct
{
    fd_set room_fd;
    int limit;
    int quantity;
    int active;
    char name[100];
    Client *clients;
} Room;

Room rooms[MAX_ROOMS];

void send_message(int sd, int server_sock, int room_id, int client_id)
{
    printf("Sending message from User %d in room %d\n", sd, room_id);
    // For each file descriptor
    for (int j = 0; j <= fdmax; j++)
        // Check if it is in the master's basket
        if (FD_ISSET(j, &rooms[room_id].room_fd))
            // And check if the value is not the descriptor itself
            if (j != sd && j != server_sock)
            {
                // Finally, send the message to that socket descriptor.
                char message[500] = "[";
                strcat(message, rooms[room_id].clients[client_id].name);
                strcat(message, "] => ");
                strcat(message, buff);
                send(j, message, 500, 0);
            }
            void execute_command(int sd, int room_id, int client_id);
}

void leave_room(int sd, int room_id, int client_id, int remove_master)
{
    printf("User %d leaving room %d\n", sd, room_id);
    // When exiting the room, the quantity of clients should be decreased.
    // Remove the descriptor from the master and the room
    if (remove_master == 1)
        FD_CLR(sd, &master);
    FD_CLR(sd, &rooms[room_id].room_fd);

    if (rooms[room_id].quantity == 0)
        return;
    rooms[room_id].clients[client_id].active = 0;
    rooms[room_id].quantity--;

    if (rooms[room_id].quantity == 0)
    {
        free(rooms[room_id].clients);
        rooms[room_id].active = 0;
    }
    void execute_command(int sd, int room_id, int client_id);
}

void reset_rooms()
{
    // Resetting all rooms in the server, initializing their values
    for (int i = 0; i < MAX_ROOMS; i++)
    {
        FD_ZERO(&rooms[i].room_fd);
        rooms[i].limit = 0;
        rooms[i].quantity = 0;
        rooms[i].active = 0;
    }
    void execute_command(int sd, int room_id, int client_id);
}

int create_room(int limit, char *name)
{
    // To create a room, you need to find the first empty room (active = 0), set it as active, and update its limit
    int room;
    for (room = 0; room < MAX_ROOMS; room++)
        if (rooms[room].active == 0)
            break;

    rooms[room].active = 1;
    rooms[room].limit = limit;
    strncpy(rooms[room].name, name, strlen(name));
    rooms[room].clients = malloc(limit * sizeof(Client));

    // After that, you should instantiate its clients' array and deactivate all present clients.
    // It is also necessary to return the value of the room
    for (int i = 0; i < limit; i++)
        rooms[room].clients[i].active = 0;
    printf("Created Room with id %d name \"%s\" and limit %d\n", room, rooms[room].name, rooms[room].limit);
    return room;
    void execute_command(int sd, int room_id, int client_id);
}

void join_room(int sd, int room_id, char name[])
{
    printf("User %d entering room %d\n", sd, room_id);
    // To insert into the room, you should increase the quantity, add the descriptor to the room's basket, find an empty position in the room (client.active = 0)
    // and insert its attributes such as socket descriptor, active, and name

    printf("Disconnecting descriptor %d\n", sd);

    char resp_buff[256];
    FD_SET(sd, &rooms[room_id].room_fd);
    if (rooms[room_id].quantity == rooms[room_id].limit)
    {
        snprintf(resp_buff, sizeof(resp_buff), "Error: Room %d is full\n", room_id);
        send(sd, resp_buff, strlen(resp_buff), 0);
        return;
    }
    rooms[room_id].quantity++;
    for (int i = 0; i < rooms[room_id].limit; i++)
    {
        if (rooms[room_id].clients[i].active == 0)
        {
            rooms[room_id].clients[i].client_sock = sd;
            rooms[room_id].clients[i].active = 1;
            strncpy(rooms[room_id].clients[i].name, name, strlen(name));
            snprintf(resp_buff, sizeof(resp_buff), "Entering in room %d sucessfuly", room_id);
            send(sd, resp_buff, strlen(resp_buff), 0);
            break;
        }
    }
}

void execute_command(int sd, int room_id, int client_id)
{
    printf("Command \"%s\" triggered in room %d by user %d\n", buff, room_id, sd);
    char resp_buff[256];

    // If the recv function returns 0 or the message was an exit message, remove the socket descriptor from the basket
    if (strncmp(buff + 1, "exit", 4) == 0)
    {
        printf("Disconnecting descriptor %d\n", sd);
        strcpy(resp_buff, "Client Disconnected\n");
        send(sd, resp_buff, strlen(resp_buff), 0);
        close(sd);
        leave_room(sd, room_id, client_id, 1);
    }

    // If the command is show_users, you should iterate through all the active clients in the room
    // and list them by sending them with the send function
    else if (strncmp(buff + 1, "show_users", 10) == 0)
    {
        send(sd, "\n===== Connected Clients =====", 40, 0);
        sleep(0.1);
        for (int i = 0; i < rooms[room_id].limit; i++)
        {
            Client c = rooms[room_id].clients[i];
            if (c.active)
            {
                char name[260];
                if (c.client_sock == sd)
                    snprintf(name, sizeof(name), "\n[%s]", c.name);
                else
                    snprintf(name, sizeof(name), "\n%s", c.name);
                send(sd, name, strlen(name), 0);
            }
        }
        send(sd, "\n\n", 2, 0);
        void execute_command(int sd, int room_id, int client_id);
    }

    // show list of rooms
     if (strncmp(buff + 1, "show_rooms", 10) == 0)
    {
        send(sd, "\n===== Rooms =====", 40, 0);
        sleep(0.1);
        for (int i = 0; i < MAX_ROOMS; i++)
            if (rooms[i].active)
            {
                snprintf(resp_buff, sizeof(resp_buff), "\n%d Room %s, Full (%d/%d)", i, rooms[i].name, rooms[i].quantity, rooms[i].limit);
                printf("resp_buff %s", resp_buff);
                send(sd, resp_buff, strlen(resp_buff), 0);
            }
        send(sd, "\n\n", 2, 0);
        void execute_command(int sd, int room_id, int client_id);
    }

    // If a client wants to switch rooms, two routines should be executed:
    // the exit routine for the current room and the insert routine for the new room

    else if (strncmp(buff + 1, "change_room", 11) == 0)
    {
        recv(sd, buff, 256, 0);
        int new_room = atoi(buff);
        char name[256];
        strcpy(name, rooms[room_id].clients[client_id].name);
        leave_room(sd, room_id, client_id, 0);
        join_room(sd, new_room, name);
        void execute_command(int sd, int room_id, int client_id);
    }
    else
    {
        // Sending error message
        strcpy(resp_buff, "\n");
        send(sd, resp_buff, strlen(resp_buff), 0);
        void execute_command(int sd, int room_id, int client_id);
    }
}
int initialize_server(char *argv[])
{
    // Cleaning sets, masters and rooms
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    reset_rooms();

    // Socket configuration
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = inet_addr(argv[1]);
    myaddr.sin_port = htons(atoi(argv[2]));
    memset(&(myaddr.sin_zero), 0, 8);
    bind(sock, (struct sockaddr *)&myaddr, sizeof(myaddr));
    listen(sock, 10);

    // Adding descriptors files
    FD_SET(sock, &master);
    FD_SET(STDIN, &master);
    fdmax = sock;
    addrlen = sizeof(remoteaddr);
    return sock;
}
int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        printf("Enter the IP and port of the server\n");
        // show format
        printf("With the format xxx.x.x.x xxxx");
        exit(1);
    }
    int sock = initialize_server(argv);

    printf("Server initialized!\n");
    int room;

    for (;;)
    {
        // Inform that the master will receive read descriptors and perform the select operation
        read_fds = master;
        select(fdmax + 1, &read_fds, NULL, NULL, NULL);
        for (int i = 1; i <= fdmax; i++)
        {
            // Test to see if the file descriptor is in the basket
            if (FD_ISSET(i, &read_fds))
            {
                // Check if the file descriptor is the socket
                if (i == sock)
                {
                    // If it is, accept the connection and add the socket descriptor to the basket
                    newfd = accept(sock, (struct sockaddr *)&remoteaddr, &addrlen);
                    FD_SET(newfd, &master);

                    // Receive the user's name and the room they want to enter
                    int limit, size_name, size_buff;
                    char name[256];
                    size_name = recv(newfd, name, 256, 0);
                    size_buff = recv(newfd, buff, 256, 0);
                    room = atoi(buff);
                    printf("User %s room %d\n", name, room);
                    if (room == -1)
                    {
                        int size_rom_name;
                        char rom_name[101];
                        size_rom_name = recv(newfd, rom_name, 100, 0);

                        recv(newfd, buff, sizeof(int), 0);
                        limit = atoi(buff);
                        printf("buff %s", buff);
                        printf("limit %d", limit);
                        room = create_room(limit, rom_name);
                    }
                    join_room(newfd, room, name);

                    // If the value of the socket descriptor (sock) is greater than the current maximum (indicating more items in the basket)
                    // update this value for the next iterations of the loop
                    if (newfd > fdmax)
                        fdmax = newfd;
                }
                else
                {
                    // If it is not the socket descriptor, create a buffer, receive the message, and transmit it to all connected sockets
                    memset(&buff, 0, sizeof(buff));
                    nbytes = recv(i, buff, sizeof(buff), 0);

                    // Find the room where the socket descriptor is located
                    int room_id;
                    for (room_id = 0; room_id < MAX_ROOMS; room_id++)
                        if (FD_ISSET(i, &rooms[room_id].room_fd))
                            break;

                    // Find the ID of the client in their current room
                    int client_id;
                    for (client_id = 0; client_id < rooms[room_id].limit; client_id++)
                        if (rooms[room_id].clients[client_id].client_sock == i)
                            break;

                    if (nbytes == 0)
                    {
                        printf("Forcefully disconnecting the descriptor %d\n", i);
                        leave_room(i, room_id, client_id, 1);
                    }

                    if (buff[0] == '/')
                        execute_command(i, room_id, client_id);
                    else
                        send_message(i, sock, room_id, client_id);
                }
            }
        }
    }

    return 0;
}