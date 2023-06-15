#include "../inc/io_config.h"
#include "../inc/io_epoll.h"
#ifndef _HAVE_EPOLL_
int io_init_epoll(void *io_base, int maxev)
{
    return -1;
}
#else
#include <stdlib.h>
#include <errno.h>

#include "../inc/io.h"
#include "../inc/io_epoll.h"

#include <unistd.h>
#include <sys/epoll.h>

struct tag_epoll_data {
    struct epoll_event *events;
    int nevents;
    int epfd;
};
typedef struct tag_epoll_data io_epoll_data_t;

static uint32_t to_epoll_events(uint32_t what)
{
    uint32_t events=0;
    if(what & EV_TYPE_READ)    
    {
        events = EPOLLIN;
    }
    if(what & EV_TYPE_WRITE)    
    {
        events |= EPOLLOUT;
    }
    return events;
}

static uint32_t from_epoll_events(uint32_t events)
{
    uint32_t what=0;
    if(events & (EPOLLHUP|EPOLLERR))
    {
        what = EV_TYPE_READ | EV_TYPE_WRITE;
    }
    else
    {
        if(events & EPOLLIN){what |= EV_TYPE_READ;}
        if(events & EPOLLOUT){what |= EV_TYPE_WRITE;}
    }
    return what;            
}


static void epoll_free(io_base_t *base)
{
    io_epoll_data_t *io_data = (io_epoll_data_t *)(base->model_data);
    if(!io_data){return;}
    if(io_data->events)
    {
        free(io_data->events);
    }
    if(io_data->epfd >= 0)
    {
        close(io_data->epfd);
    }
    free(io_data);
    base->model_data = (void *)0;
}

static int epoll_dispatch(io_base_t * base, int timeout)
{
    int i;
    int id = 0;
    io_t *iop = NULL;
    io_epoll_data_t *io_data = (io_epoll_data_t *)(base->model_data);
    int n = 0;
    do{
        n = epoll_wait(io_data->epfd, io_data->events, io_data->nevents, timeout);    
    }while((n < 0) && (errno == EINTR));
    base->cur_time = time(NULL);
    for(i = 0; i < n; i++)
    {
        id = (int)((io_data->events)[i].data.u32);
		if(id >= 0 && id < base->maxio)
		{
			iop = (base->iops)+id;
			IOP_CB(base,iop,from_epoll_events(io_data->events[i].events));
		}
    }
    return n;
}

static int epoll_add(io_base_t *base, int id, io_handle_t handle, unsigned int events)
{
    /*test-info,yanghui,begin*/
    //printf("base_dispatch1.611=%u,name=%s\n",base->op_imp.base_dispatch,base->op_imp.name);
    //fflush(stdout);
    /*test-info,yanghui,end*/
    io_epoll_data_t *io_data = (io_epoll_data_t *)(base->model_data);
    struct epoll_event ev;
    ev.data.u32 = id;  
    ev.events = to_epoll_events(events);
    //io_set_nonblock(handle);
    return epoll_ctl(io_data->epfd, EPOLL_CTL_ADD, (int)handle, &ev);                
}


static int epoll_del(io_base_t *base, int id,io_handle_t handle)
{
    io_epoll_data_t *io_data = (io_epoll_data_t *)(base->model_data);
    struct epoll_event ev;
    ev.data.u32 = id;  
    ev.events = 0;
    return epoll_ctl(io_data->epfd, EPOLL_CTL_DEL, (int)handle, &ev);                
}

static int epoll_mod(io_base_t *base, int id, io_handle_t handle, unsigned int events)
{
    io_epoll_data_t *io_data = (io_epoll_data_t *)(base->model_data);
    struct epoll_event ev;
    ev.data.u32 = id;  
    ev.events = to_epoll_events(events);
    return epoll_ctl(io_data->epfd, EPOLL_CTL_MOD, (int)handle, &ev);                
}



int io_init_epoll(void *io_base, int maxev)
{
	io_base_t *base = (io_base_t *)io_base;
	io_op_t *io_op = &(base->op_imp);

	io_epoll_data_t *io_data = (io_epoll_data_t *)(malloc(sizeof(io_epoll_data_t)));
    if(!io_data)
    {
        return -1;
    }
    io_data->nevents = maxev;
    io_data->events = (struct epoll_event *)(malloc(sizeof(struct epoll_event) * maxev));
    if(!io_data)
    {
        free(io_data);
        return -1;
    }

    io_op->name = "epoll";
    io_op->base_free = epoll_free;
    io_op->base_dispatch = epoll_dispatch;
    io_op->base_add = epoll_add;
    io_op->base_del = epoll_del;
    io_op->base_mod = epoll_mod;
    /*test-info,yanghui,begin*/
    //printf("epoll_dispatch=%u,base_dispatch=%u,epoll_add=%u\n",epoll_dispatch,io_op->base_dispatch,epoll_add);
    //fflush(stdout);
    /*test-info,yanghui,end*/
    
    //1024 is not the max events limit.
    int epfd = epoll_create(1024);
    if(epfd < 0)
    {
        free(io_data->events);
        free(io_data);
        free(io_op);
        return -1;
    }
    io_data->epfd = epfd;



    base->model_data = io_data;
    
    return 0;
}

#endif

