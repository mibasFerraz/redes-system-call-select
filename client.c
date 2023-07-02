#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>
char buff[256];
// clear the screen
void clear()
{
  printf("\033[H\033[J");
}
void clear_input_buffer()
{
  // Clear the input buffer
  while (getchar() != '\n')
    ;
}

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    printf("Usage: %s <server IP> <server port>\n", argv[0]);
    exit(1);
  }

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(atoi(argv[2]));
  inet_pton(AF_INET, argv[1], &(serverAddr.sin_addr));

  if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
  {
    perror("Failed to connect");
    exit(1);
  }
  fd_set read_fds, master;
  FD_ZERO(&master);
  FD_ZERO(&read_fds);
  FD_SET(STDIN_FILENO, &master);
  FD_SET(sock, &master);
  int fdmax = (STDIN_FILENO > sock) ? STDIN_FILENO : sock;

  char nome[256];
  printf("Enter your name: ");
  scanf("%s^\n", nome);
  nome[strcspn(nome, "\n")] = '\0';
  send(sock, nome, strlen(nome), 0);

  int sala_id;
  printf("Enter the room ID (-1 to create a new room): ");
  scanf("%d", &sala_id);
  snprintf(buff, sizeof(buff), "%d", sala_id);
  send(sock, buff, strlen(buff), 0);
  clear_input_buffer();

  if (sala_id == -1)
  {

    int limit;
    char room_name[100];
    printf("Enter the room`s name: ");
    scanf("%s^\n", room_name);

    room_name[strcspn(room_name, "\n")] = '\0';
    send(sock, room_name, strlen(room_name), 0);
    clear_input_buffer();

    printf("Enter the room limit: ");
    scanf("%d", &limit);
    clear_input_buffer();

    printf("limit %d\n", limit);
    if (limit < 10)
      snprintf(buff, sizeof(buff), "0%d", limit);
    else
      snprintf(buff, sizeof(buff), "%d", limit);
    send(sock, buff, strlen(buff), 0);
  }
  clear();
  memset(buff, 0, sizeof(buff));
  int nbytes = recv(sock, buff, sizeof(buff), 0);
  if (strncmp(buff, "Error", 5) == 0)
  {
    sleep(1);
    close(sock);
    printf("%s\n", buff);
    printf("This room is full or doesn't exists. Try to create another room\n");
    exit(1);
  }
  printf("%s\n", buff);
  printf("Send /help to more info...\n");

  while (1)
  {
    read_fds = master;
    if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
    {
      perror("select");
      exit(1);
    }

    for (int i = 0; i <= fdmax; ++i)
    {
      if (FD_ISSET(i, &read_fds))
      {
        if (i == STDIN_FILENO)
        {
          // Read from stdin and send to the server
          memset(buff, 0, sizeof(buff));
          fgets(buff, sizeof(buff), stdin);
          // check if the message is a command, list of comamnds [/exit, /show_users, /change_room]
          buff[strcspn(buff, "\n")] = '\0';
          if (buff[0] == '/')
          {
            // check if is a command
            if (strcmp(buff, "/exit") == 0 | strcmp(buff, "/show_users") == 0 | strcmp(buff, "/show_rooms") == 0)
            {
              send(sock, buff, strlen(buff), 0);
            }
            else if (strcmp(buff, "/change_room") == 0)
            {
              send(sock, buff, strlen(buff), 0);
              // ask for which room the user want to go
              printf("Enter the room ID you wish to go: ");
              int sala_id;
              scanf("%d", &sala_id);
              snprintf(buff, sizeof(buff), "%d", sala_id);
              printf("buffer to change room %s\n", buff);
              send(sock, buff, strlen(buff), 0);
            }
            else if (strcmp(buff, "/help") == 0)
            {
              // print list of commands
              clear();
              printf("I'm Batman and I'll help you\n");
              printf("Use /exit - to exit a room; \n");
              printf("Use /show_users - to show a list of the users in a room; \n");
              printf("Use /show_rooms - to show a list of available rooms ; \n");
              printf("Use /change_room - to change your room; \n");
              printf("Use /clear - Clear the screen \n");
              printf("Use /bat - to show batman taranranran. \n");
            }
            else if (strcmp(buff, "/clear") == 0)
            {
              clear();
            }

            else if (strcmp(buff, "/bat") == 0)
            {
              clear();
              printf("⣀⠀⠻⣿⣿⣦⠀⠀⠈⢿⣿⡄⠀⠀⠸⣿⣆⠀⠀⠀⢀⠀⢸⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
              printf("⣿⣷⣄⠈⢿⣿⣧⡀⠀⠀⢻⣿⡄⠀⠀⢻⣿⡄⠀⠀⠀⢆⠀⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠋⠀⣀⣴⣾⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
              printf("⠙⢿⣿⣷⣄⠙⢿⣷⡀⠀⠀⠹⣿⣆⠀⠈⢿⣷⠀⠀⠀⢸⡄⢸⣿⣿⣿⣦⣄⣀⠀⠀⠀⠀⠀⠀⠀⣀⠔⢰⣤⣾⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
              printf("⠀⠀⠙⠻⣿⣧⡈⠻⣷⡀⠀⠀⠙⣿⡄⠀⠘⣿⣇⠀⠀⠀⣿⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠋⢀⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
              printf("⣄⡀⠀⠀⠈⠻⣿⣦⡈⢷⡀⠀⠀⠘⣿⡄⠀⠹⣿⡄⠀⠀⣹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠁⢀⣾⣥⣀⠀⠈⠙⠻⣇⠀⠀⠀⠀⢀⠞⠀⠀⣀⣀⠀⣀⠴⠊\n");
              printf("⣿⣿⣶⣄⡀⠀⠀⠙⢷⣄⠡⠀⠀⠀⠘⣿⡄⠀⢻⣷⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠀⠀⣼⣿⣿⣿⣿⣶⡄⠀⠈⢳⣄⣠⣶⢋⣶⡾⢋⣠⠔⢋⠀⠀⠀\n");
              printf("⣿⣿⣿⣿⣿⣦⣄⠀⠀⠙⢷⣄⠀⠀⠀⠘⣿⡀⠘⣿⡇⠀⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠏⠀⢀⣼⣿⣿⣿⣿⣿⣿⣿⣦⣶⣶⣿⣿⠃⣼⣿⠷⠋⠀⡴⠁⠀⣀⣤\n");
              printf("⣤⣴⣶⣶⣶⣤⣭⣁⡢⠀⠀⠙⢷⡀⠀⠀⠘⣷⡀⢻⣷⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠀⢀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏⠉⠹⠁⡼⠛⠁⢀⡴⢋⣠⣴⣿⣿⣿\n");
              printf("⠛⠛⠛⠛⠛⠿⠿⠿⣿⣶⣤⡀⠀⠙⠦⡀⠀⠘⣷⠈⢿⡇⠀⢿⣿⣿⣿⣿⣿⣿⣿⡟⠁⠀⣌⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⢀⠞⢀⣤⡾⠋⢀⣾⣿⣿⡿⠛⠁\n");
              printf("⠀⠀⠀⠀⠀⠀⢀⣴⣿⣿⣿⢿⣷⣄⠀⠈⢦⠀⠘⣧⠘⣿⡰⡘⣿⣿⣿⣿⣿⣿⡿⠀⢀⣸⡿⣷⣽⣿⣿⣿⣿⣿⣿⣿⣿⠁⢠⣮⣾⣿⠟⢀⣴⣿⣿⣿⠏⠀⠀⠀\n");
              printf("⣤⣤⣄⠀⠈⠛⠿⢿⣿⣿⣿⠘⣿⣿⣷⣄⠀⠑⠀⠘⣧⡸⣧⠱⡸⣿⣿⣿⣿⣿⠁⡦⣸⣿⣧⡈⢿⣾⣿⣿⣿⣿⣿⡿⠏⣰⣿⣿⠟⢁⣴⣿⣿⣿⠟⠁⣀⣤⣶⣿\n");
              printf("⣛⣛⣛⡓⠶⣿⣿⣿⣿⣿⣿⡆⢿⣿⣿⣿⣷⣄⠀⠀⠈⢷⣿⣇⠳⣽⣿⣿⣿⡇⢰⠇⠘⢿⣿⣿⣿⣿⣿⣿⡛⢓⣶⢀⣼⡿⠋⢁⣴⣿⣿⡿⢋⣠⣴⣿⣿⣿⣿⠿\n");
              printf("⣿⣿⣿⣿⣷⣬⣍⣭⣭⣿⣛⣿⠀⢿⣿⣿⣿⣿⣷⣄⠀⠀⠻⣿⣆⠙⢿⣿⣿⣿⡏⠀⣦⣴⣿⣿⣿⠃⣿⣿⣿⠛⢡⡾⡫⢂⣴⣿⣿⡟⢁⣴⣿⣿⣿⣿⠿⠋⠁⠀\n");
              printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠁⠀⢿⣿⣿⣿⣿⣿⣿⣦⡀⠈⠻⣧⢠⣹⣿⣿⣇⣼⣿⡯⡁⠈⢍⠀⠋⢉⠇⣰⣿⣾⣷⣿⣿⠟⢉⣴⣿⣿⡿⠟⠋⠀⠀⠀⠀⠀\n");
              printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⢀⣶⠀⠘⣿⣿⣿⣿⣿⣿⣿⣿⣷⣄⡈⠳⣿⣿⣿⣿⣿⠟⠳⠤⠀⠘⠂⠠⣢⣾⣿⣿⡿⠟⣋⣤⣾⣿⣿⠟⠋⠀⠀⠀⠀⠀⠀⠀⠀\n");
              printf("⣿⣿⣿⣿⣿⣿⡿⠿⠟⢻⠅⢾⣿⡇⠀⣿⣿⣿⣿⡿⢛⣩⣭⠭⠛⠻⢶⣬⣙⠿⣿⣿⣦⣤⣀⣠⣴⣵⣿⣿⣿⣯⣵⣾⣿⣿⣿⣿⣿⣇⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
              printf("⠛⠉⠉⠉⠀⠀⠀⠀⠀⢸⠀⢀⢿⣿⣆⢸⣿⣿⡟⣴⣿⣿⣿⣀⣠⣴⣡⠈⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀\n");
              printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⢰⠘⡏⣙⠉⠘⣿⣿⡇⣿⠛⣿⣿⣿⣿⣿⡃⠀⠀⣎⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠀⠀⠀⠉⢻⣿⠻⢿⣿⣿⣿⣷⡀⠀⠀⠀⠀⠀\n");
              printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⡆⠈⠁⢰⡿⠾⠏⠿⣿⣿⣄⠀⠁⣼⠿⠿⣿⣿⣷⣾⣿⡌⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣇⣴⣦⡀⠀⠈⠙⠳⠄⠈⢻⣿⣿⣧⠀⠀⠀⠀⠀\n");
              printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⢠⢻⡇⣀⣃⠀⣨⣽⣿⣷⣄⠁⠀⠀⠚⠛⣿⣿⡿⠁⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠁⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⠀⠀⠀⠀⠀\n");
              printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⢀⢸⠏⡾⠛⠻⣿⣿⣿⣿⣿⣿⣷⣦⣄⣀⠒⠛⣋⣠⣾⣿⣿⣿⣿⡿⠟⠛⠿⣿⣿⣿⣿⣷⣄⡀⠀⠀⠀⠀⠀⠀⠈⣿⣿⠀⠀⠀⠀⠀\n");
              printf("⠀⠀⠀⠀⠀⠀⠀⠀⡴⣿⣾⡀⣠⠁⢰⣾⣿⣿⣿⢫⣭⣭⡙⣿⣿⣿⣿⣿⣿⡿⠿⠿⠛⠋⠉⠀⠀⢀⣠⣽⣿⣿⣿⣿⣿⣿⣷⣦⣄⡀⠀⠀⠀⣽⣿⣧⠀⠀⠀⠀\n");
              printf("⠀⠀⠀⠀⠀⠀⡴⡎⢄⠘⣿⣿⣿⡀⣿⡛⢿⡿⢃⣿⣿⣿⣿⣹⣿⣿⣿⣿⣿⣶⣶⣶⡶⠾⠿⠛⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠛⠋⠉⠀⠀⠀⠉⠹⣿⣷⠀⠀⠀\n");
              printf("⠀⠀⠀⠀⠀⡜⣠⣧⠀⠀⠘⣷⢿⡿⣿⣿⣮⢡⣮⣍⠉⠛⠟⠿⠿⠟⣛⣉⡻⠷⡶⠶⠖⠀⠀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⡀⠀⠀⠀⠀⠀⠀⠙⣿⡇⠀⠀\n");
              printf("⠀⠀⠀⠀⡘⢸⣿⣿⠀⠀⠀⣼⣿⣿⣮⣿⣿⣧⣿⣿⣿⣿⠀⠀⠀⢺⣿⣿⣇⠠⡤⠶⣚⠁⡴⠛⢛⠛⠻⠟⠉⠀⠈⠻⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀⢰⠸⣷⣄⠀\n");
              printf("⠀⠀⠀⢠⠃⣿⣿⣿⠀⠀⠀⢹⠛⠛⢿⣿⣿⣿⣿⣿⣯⣅⣴⣿⣷⣿⣿⣿⠿⠔⠚⠉⠁⣈⡴⠋⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⢿⣿⣿⣿⣦⣄⠀⠀⣠⠎⣠⠉⠉⠉\n");
              printf("⠀⠀⠀⢸⠀⣿⣿⣿⣦⣀⣤⡏⠀⠀⠀⠈⠙⠻⢿⣿⣿⣿⣿⣿⣿⣁⣈⣥⣄⠀⣀⠴⠚⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠿⣿⣿⣿⣷⣾⣏⠀⣿⡆⠀⠀\n");
              printf("⠀⠀⠀⢸⡀⣿⣿⣿⣿⢿⣿⣷⣄⡀⠀⠀⠀⠀⠀⢹⠿⡟⠉⠉⠉⠛⠿⣿⣯⣤⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣿⣿⣍⠙⢿⣿⠇⠀⠀\n");
              printf("⠀⠀⠀⠈⡇⣿⣿⣿⣿⡘⣿⣝⣿⣿⣶⣤⣀⣀⣰⣥⠌⠀⠀⠀⠀⠀⠌⠀⢉⡹⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣿⣿⣿⣾⣿⠀⠀⠀\n");
              printf("⠀⠀⠀⠀⡷⣿⣿⣿⣿⣷⠀⣽⣿⣿⣿⣿⣿⣿⣿⣿⣄⣀⣀⣠⣶⣭⣵⡿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⢿⣿⣿⣿⡄⠀⠀\n");
            }
            else
            {
              printf("Invalid command: %s\n", buff);
            }
          }
          else
          {
            // send message to the server
            send(sock, buff, strlen(buff), 0);
          }
        }
        else if (i == sock)
        {
          // Receive and print messages from the server
          memset(buff, 0, sizeof(buff));
          int nbytes = recv(sock, buff, sizeof(buff), 0);
          if (nbytes <= 0)
          {
            // Server disconnected
            printf("Disconnected from the server.\n");
            close(sock);
            exit(1);
          }
          printf("%s\n", buff);
        }
      }
    }
  }

  close(sock);
  return 0;
}
