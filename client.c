#include "zgossip.h"

int main(int argn, char *argv[])
{

    if (argn < 2) {
        printf("Usage: %s tcp://peer:port [tcp://peer:port]\n", argv[0]);
        exit(1);
    }
    zactor_t *server1 = zactor_new (zgossip, "server1");
    assert (server1);
    //zstr_sendx (server1, "VERBOSE", NULL);
    zpoller_t *poller = zpoller_new (NULL);
    assert(poller);
    zpoller_add (poller, server1);

    printf("Will connect to:\n");
    for (int n=1; n<argn; n++) {
        printf("-  %s\n", argv[n]);
        zstr_sendx (server1, "CONNECT", argv[n], NULL);
    }

    char *key_str = zsys_sprintf ("Client-%d", getpid());

    zclock_sleep (1000);
    zstr_sendx (server1, "PUBLISH", key_str, key_str, NULL);

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
