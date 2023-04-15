#include "display_req.h"

struct passwd *display(){
    struct passwd * details;

    uid_t id = getuid();

    // printf("id : %d\n",id); // debug statement---------------------------------

    details = getpwuid(id);

    // printf("details : %s\n",details); // debug statement---------------------------------

    if( details == NULL ){
        perror("ERROR : getpwuid returns NULL\n");
        exit(0);
    }

    return details;
}