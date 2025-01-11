/*
Create a socket
Bind it to an address (<localhost, port 8080>)
Listen on the socket for incoming connections

*/

struct client_data
{
    int fd;
} clients[NUM_CLIENTS];

int create_socket_and_listen()
{
    struct addrinfo *addr;
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo("127.0.0.1", "8080", &hints, &addr);
    int local_s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    bind(local_s, addr->ai_addr, addr->ai_addrlen);
    listen(local_s, 5);
    return local_s;
}

int main(int argc, char *argv[])
{
    int local_s = create_socket_and_listen();
    int kq = kqueue();
    struct kevent evSet;
    EV_SET(&evSet, local_s, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(kq, &evSet, 1, NULL, 0, NULL);
    run_event_loop(kq, local_s);
    printf("success\n");
    return EXIT_SUCCESS;
}

/*
Lookup: Given a fd, we can find the corresponding client_data by simply iterating over the array
Add: For new connections, we find the first free item (fd = 0) in the array to store the client’s fd
Delete: When the connection is lost, we free that item in the array by setting it’s fd to 0
*/

int get_conn(int fd)
{
    for (int i = 0; i < NUM_CLIENTS; i++)
        if (clients[i].fd == fd)
            return i;
    return -1;
}

int conn_add(int fd)
{
    if (fd < 1)
        return -1;
    int i = get_conn(0);
    if (i == -1)
        return -1;
    clients[i].fd = fd;
    return 0;
}

int conn_del(int fd)
{
    if (fd < 1)
        return -1;
    int i = get_conn(fd);
    if (i == -1)
        return -1;
    clients[i].fd = 0;
    return close(fd);
}

/*
Event Loop
Now, we create an infinite loop where we call kevent(..) to receive incoming events and process them.
*/

while (1)
{
    int num_events = kevent(kq, NULL, 0, evList, MAX_EVENTS, NULL);
    for (int i = 0; i < num_events; i++)
    {
        // receive new connection
        if (evList[i].ident == local_s)
        {
            int fd = accept(evList[i].ident, (struct sockaddr *)&addr, &socklen);
            if (conn_add(fd) == 0)
            {
                EV_SET(&evSet, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                kevent(kq, &evSet, 1, NULL, 0, NULL);
                send_welcome_msg(fd);
            }
            else
            {
                printf("connection refused.\n");
                close(fd);
            }
        } // client disconnected
        else if (evList[i].flags & EV_EOF)
        {
            int fd = evList[i].ident;
            printf("client #%d disconnected.\n", get_conn(fd));
            EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
            kevent(kq, &evSet, 1, NULL, 0, NULL);
            conn_del(fd);
        } // read message from client
        else if (evList[i].filter == EVFILT_READ)
        {
            recv_msg(evList[i].ident);
        }
    }
}
