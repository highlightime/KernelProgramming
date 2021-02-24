#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>
#include "prinfo.h"
#include <string.h>
#include <stdlib.h>

int main(){
        int nr=500;
        void *buf=malloc(sizeof(struct prinfo)*nr);
        int res = syscall(548,buf,&nr);

        int i,stack[500]={0},top=1,j;
        for(i=0;i<nr;i++){
                struct prinfo *p=(struct prinfo*)buf+i;
                while(p->parent_pid!=stack[top-1])top--;
                        stack[top++]=p->pid;
                for(j=0;j<2*(top-2);j++)
                        printf(" ");
                printf("├─ %s --- %ld,%d,%d,%d,%d\n",p->comm,p->state,p->pid,p->parent_pid,p->first_child_pid,p->next_sibling_pid);
        }
        printf("Total %d\n",res);
        printf("Copied %d\n",nr);

        return 0;
}
