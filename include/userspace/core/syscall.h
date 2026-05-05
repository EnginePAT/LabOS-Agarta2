#ifndef SYSCALL_H
#define SYSCALL_H

int exit(int code);
int write(int fd, const char* buf, unsigned int len);

#endif      // SYSCALL_H
