#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int is_prime(int num) {
    int is_prime = 1;
    for(int j=2; j<num; j++) {
        if(num%j==0){
            is_prime=0;
            break;
        }
    }
    return is_prime;
}

void receive_and_spawn(int reader) {
    int i;
    if(read(reader, &i, sizeof i)==0) {
        exit(0);
    }
    printf("prime %d\n", i);

    int p[2];
    pipe(p);

    if(fork()==0) {
        // child
        close(p[1]);
        receive_and_spawn(p[0]);
    }

    // parent
    int status;

    close(p[0]);
    while(read(reader, &i, sizeof i)!=0) {
        if(!is_prime(i)) {
            continue;
        }
        write(p[1], &i, sizeof i);
    }
    close(p[1]);
    wait(&status);
    exit(0);
}

int
main() {
    int p[2];
    pipe(p);

    if(fork()==0) {
        // child
        close(p[1]);
        receive_and_spawn(p[0]);
    }

    // parent
    int status;

    close(p[0]);
    for(int i=2; i<36;i++) {
        write(p[1], &i, sizeof i);
    }
    close(p[1]);

    wait(&status);
    exit(0);
}