#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/prinfo.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/uaccess.h>


inline pid_t child_pid(struct task_struct *task){
        if(list_empty(&task->children))
                return 0;
        else    
                return first_child_task(task)->pid;
}

inline pid_t sibling_pid(struct task_struct *task){
        if(list_is_last(&task->sibling,&task->real_parent->children))
                return 0;
        else    
                return next_sibling_task(task)->pid;
}


void __write_prinfo(struct prinfo *info, struct task_struct *task){
        info->state=task->state;
        info->pid=task->pid;
        info->parent_pid=task->real_parent->pid;
        info->first_child_pid=child_pid(task);
        info->next_sibling_pid=sibling_pid(task);
        strncpy(info->comm,task->comm,TASK_COMM_LEN);
}

int dfs_init_task(struct prinfo *kbuf, int buflen, int *knr){
        int copied=0,count=0;
        struct task_struct *task= &init_task;

        while(true){
                if(list_empty(&task->children)){
                        while(list_is_last(&task->sibling, &task->real_parent->children))
                            task = task->real_parent;
                        task=list_first_entry(&(task)->sibling,struct task_struct, sibling);
                }else task = list_first_entry(&(task)->children, struct task_struct, sibling);
  
                if(task==&init_task)break;

                if(copied<buflen){
                        __write_prinfo(kbuf+copied,task);
                        copied++;
                }
                count++;
        }
        (*knr)=copied;
        return count;
}



int do_ptree(struct prinfo *buf, int *nr){
        if(buf==NULL||nr==NULL) return -EINVAL;
        int buffer,knr,total;
        struct prinfo *kbuf;

        if(copy_from_user(&buffer, nr, sizeof(int))) {
                printk("ERROR: copy_from_user");
                return -EFAULT;
         }
        if(buffer < 0) {
                printk("ERROR: buffer");
                return -EINVAL;
        }
        kbuf=(struct prinfo *) kmalloc(sizeof(struct prinfo)*buffer,GFP_ATOMIC);
        if(kbuf==NULL){
                printk("ERROR: kmalloc");
                return -ENOMEM;
        }
        read_lock(&tasklist_lock);
        total=dfs_init_task(kbuf,buffer,&knr);
        read_unlock(&tasklist_lock);

        if(copy_to_user(buf,kbuf,sizeof(struct prinfo) * buffer)){
                printk("ERROR: copy_to_user");
                return -EFAULT;
        }

        kfree(kbuf);

        if(copy_to_user(nr, &knr, sizeof(int))) {
                printk("ERROR: copy_to_user");
                return -EFAULT;
        }

        return total;
}

SYSCALL_DEFINE2(ptree, struct prinfo*, buf, int*, nr){
        return do_ptree(buf, nr);
}

