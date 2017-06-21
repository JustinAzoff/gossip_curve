#include "zgossip.h"

int main(int argn, char *argv[])
{

    char *bind;

    if (argn < 2) {
        printf("Usage: %s tcp://*:9999\n", argv[0]);
        exit(1);
    }
    zactor_t *server1 = zactor_new (zgossip, "server1");
    assert (server1);
    zstr_send (server1, "VERBOSE");
    //zstr_sendx (server1, "SET", "server/timeout", "60000", NULL);
    zpoller_t *poller = zpoller_new (NULL);
    assert(poller);
    zpoller_add (poller, server1);

    bind = argv[1];
    printf("Will bind to %s\n", bind);
    zstr_sendx (server1, "BIND", bind, NULL);

    char *key_str = zsys_sprintf ("Pid-%d", getpid());
    //zclock_sleep (1000);
    //zstr_sendx (server1, "PUBLISH", bind, key_str, NULL);

    while(true) {
        zsock_t *which = (zsock_t *) zpoller_wait (poller, 1000);
        if (!which) {
            if (zpoller_terminated(poller)) {
                break;
            }
            continue;
        }
        char *command = NULL, *key, *value;
        zstr_recvx (which, &command, &key, &value, NULL);
        printf("Got %s %s %s\n", command, key, value);
        zstr_free (&command);
        zstr_free (&key);
        zstr_free (&value);
    }


    zclock_sleep (1000);
    zactor_destroy(&server1);
}
