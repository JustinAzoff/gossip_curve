#include "zgossip.h"

int main(int argn, char *argv[])
{

    char *bind;

    if (argn < 1) {
        printf("Usage: CONFIG_FILE=... %s\n", argv[0]);
        exit(1);
    }

    //Start authenticator
    zactor_t *auth = zactor_new (zauth,NULL);
    zstr_send(auth,"VERBOSE");
    zsock_wait(auth);
    zstr_sendx(auth,"ALLOW","127.0.0.1",NULL);
    zsock_wait(auth);
    //  Tell the authenticator to use the certificate store in ./certs
    zstr_sendx (auth,"CURVE","certs",NULL);
    ////

    zactor_t *server1 = zactor_new (zgossip, "server1");
    assert (server1);
    //zstr_send (server1, "VERBOSE");
    zpoller_t *poller = zpoller_new (NULL);
    assert(poller);
    zpoller_add (poller, server1);

    char *config_file = getenv("CONFIG_FILE");
    zconfig_t *config = NULL;
    if (config_file) {
        printf("Server config file %s\n", config_file);
        zstr_sendx (server1, "LOAD", config_file, NULL);
        config = zconfig_load (config_file);
    }
    if(!config) {
        puts("need config");
        zactor_destroy(&server1);
        exit(1);
    };

    zclock_sleep (1000);
    printf("Will connect to:\n");
    zconfig_t *connect_section = zconfig_locate (config, "zgossip/connect");
    if (connect_section)
          connect_section = zconfig_child (connect_section);
    while(connect_section) {
        char *endpoint = zconfig_value(connect_section);
        printf("-  %s\n", endpoint);
		zstr_sendx (server1, "CONNECT", endpoint, NULL);
        connect_section = zconfig_next (connect_section);
    }
        
    bind = zsys_sprintf ("Bind-pid-%d", getpid());

    char *key_str = zsys_sprintf ("Pid-%d", getpid());

    //zstr_sendx (server1, "PUBLISH", key_str, bind, NULL);
    zstr_sendx (server1, "PUBLISH", bind, key_str, NULL);

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
