#include "zgossip.h"

#define MAX_MESSAGES 1000

void drain(zpoller_t *poller)
{
    int num = 0;
    while(++num < MAX_MESSAGES) {
        zsock_t *which = (zsock_t *) zpoller_wait (poller, 2000);
        if (!which) {
            break;
            /*
            if (zpoller_terminated(poller)) {
                break;
            }
            continue;
            */
        }
        char *command = NULL, *key, *value;
        zstr_recvx (which, &command, &key, &value, NULL);
        fprintf(stderr, "Got %s %s %s\n", command, key, value);
        zstr_free (&command);
        zstr_free (&key);
        zstr_free (&value);
    }
}

zactor_t * setup_actor(char *name, const char *bind, const char *connect, const char *key, const char *value)
{
    zactor_t *server = zactor_new (zgossip, name);
    assert (server);
    zstr_sendx (server, "VERBOSE", NULL);
    zstr_sendx (server, "BIND", bind, NULL);
    zstr_sendx (server, "CONNECT", connect, NULL);
    zstr_sendx (server, "PUBLISH", key, value, NULL);
    return server;
}


int main(int argn, char *argv[])
{

    zactor_t *server1 = setup_actor("server1", "tcp://*:9001", "tcp://localhost:9002", "one", "one");
    zactor_t *server2 = setup_actor("server2", "tcp://*:9002", "tcp://localhost:9001", "two", "two");

    zpoller_t *poller = zpoller_new (NULL);
    assert(poller);

    zpoller_add (poller, server1);
    zpoller_add (poller, server2);

    zstr_sendx (server1, "PUBLISH", "one", "one", NULL);
    zstr_sendx (server2, "PUBLISH", "two", "two", NULL);

    drain(poller);

    zclock_sleep (1000);

    zpoller_remove(poller, server2);
    zactor_destroy(&server2);

    server2 = setup_actor("server2", "tcp://*:9002", "tcp://localhost:9001", "two", "two-different");
    zpoller_add (poller, server2);
    drain(poller);

    zactor_destroy(&server1);
    zactor_destroy(&server2);
}
