#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

struct msgbuf {
    long mtype;
    int mvalue;
};

void send_result(int msgid, int result, int result_type) {
    struct msgbuf msg;
    msg.mtype = result_type;
    msg.mvalue = result;
    msgsnd(msgid, &msg, sizeof(msg.mvalue), 0);
}

int f(int x) {
    return x * x;
}

int g(int x) {
    return x + 1; 
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <integer value>\n", argv[0]);
        return -1;
    }

    int x = atoi(argv[1]);
    int msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    if (fork() == 0) {
        int result = f(x);
        send_result(msgid, result, 1);
        exit(0);
    }

    if (fork() == 0) {
        int result = g(x);
        send_result(msgid, result, 2);
        exit(0);
    }

    struct msgbuf msg;
    int results[2] = {0};
    int count = 0;
    while (count < 2) {
        if (msgrcv(msgid, &msg, sizeof(msg.mvalue), 0, 0) > 0) {
            results[msg.mtype - 1] = msg.mvalue;
            count++;
        }
    }

    int fin_result = results[0] * results[1];
    printf("Result: %d\n", fin_result);

    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}