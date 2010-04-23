#ifndef ErrorCheck_H
#define ErrorCheck_H


#include <sys/errno.h>

#define SOCK_ERRORCHECK(rc,namestr) \
    if (rc < 0) { \
       fprintf(stderr,namestr" failed:%d %s\n",errno,strerror(errno)); \
       exit(1); \
    }

#define MAKESURE(cond,str) \
    if (!(cond)) { \
        fprintf(stderr,str" "#cond" failed!\n");  \
        exit(1); \
    }

#endif
