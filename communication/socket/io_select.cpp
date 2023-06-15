#include "io_config.h"
#include "io_select.h"
#include "io_def.h"

#ifndef _HAVE_SELECT_
int io_init_select(void *io_base, int maxev)
{
    return -1;
}
#else


/////START...
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#ifndef WIN32
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "io.h"


struct tag_io_select_data {
    fd_set rset_out;
    fd_set wset_out;

    fd_set rset;
    fd_set wset;
    int maxfd;
};
typedef struct tag_io_select_data io_select_data_t;



static void select_free(io_base_t *base)
{
    io_select_data_t *p = (io_select_data_t *)(base->model_data);
    if(p)
    {
        free(p);
    }
    base->model_data = (void *)0;
}

static int select_dispatch(io_base_t * base, int timeout)
{
    int tv_sec = timeout/1000;
    int tv_usec = (timeout*1000)%1000000;
    struct timeval tv = {tv_sec, tv_usec};
    int n = 0;
	unsigned int revents = 0;

	io_select_data_t *model_data = (io_select_data_t *)(base->model_data);
    memcpy(&(model_data->rset_out), &(model_data->rset), sizeof(model_data->rset));
    memcpy(&(model_data->wset_out), &(model_data->wset), sizeof(model_data->wset));


#ifdef WIN32
        n = select(0, &(model_data->rset_out),&(model_data->wset_out),NULL,&tv);
        if(n < 0)
        {
            if(WSAEINVAL == socket_errno)
            {
                Sleep(timeout);
                n = 0;
            }
        }
#else
    do{
        n = select(model_data->maxfd+1, &(model_data->rset_out),&(model_data->wset_out),NULL,&tv);
     }while((n < 0) && (errno == EINTR));
#endif
    base->cur_time = time(nullptr);
    if(n <= 0)
    {
        return n;
    }

    //客户端遍历
    for (auto iop : base->iops) {
        if (FD_ISSET(iop.handle, &(model_data->rset_out)))
        {
            revents = EV_TYPE_READ;
        }
        if (FD_ISSET(iop.handle, &(model_data->wset_out)))
        {
            revents |= EV_TYPE_WRITE;
        }
        if(revents != 0)
        {
            base->clientEventCB(iop,revents);
            iop.last_dispatch_time = base->cur_time;
        }
    }

    //服务端
    revents = 0;
    if (FD_ISSET(base->handle, &(model_data->rset_out)))
    {
        revents = EV_TYPE_READ;
    }
    if(revents != 0)
    {
        base->serverEventCB(revents);
    }

    return n;
}

static int select_add(io_base_t *base, int id, io_handle_t handle, unsigned int events)
{
    io_select_data_t *p = (io_select_data_t *)(base->model_data);
    if(events & EV_TYPE_READ)
    {
        FD_SET(handle, &(p->rset));
    }
    if(events & EV_TYPE_WRITE)
    {
        FD_SET(handle, &(p->wset));
    }
#ifndef WIN32
    if(handle > p->maxfd)
    {
        p->maxfd = handle;
    }
#endif
    return 0;
}


static int select_del(io_base_t *base, int id,io_handle_t handle)
{
    io_select_data_t *p = (io_select_data_t *)(base->model_data);
    FD_CLR(handle, &(p->rset));
    FD_CLR(handle, &(p->wset));
#ifndef WIN32
	/***linux下需要提供maxfd的值 ***/
    if(handle == p->maxfd)
    {
        io_t *iop;
		int cur_id = base->io_list_head;
        p->maxfd = -1;

		while(cur_id != -1)
		{
			iop = base->iops+cur_id;
			if(cur_id!=id)
			{
				if(p->maxfd < iop->handle)
				{
					p->maxfd = iop->handle;
				}
			}
			cur_id = iop->next;
		}

    }
#endif
    return 0;
}

static int select_mod(io_base_t *base, int id, io_handle_t handle, unsigned int events)
{
    io_select_data_t *p = (io_select_data_t *)(base->model_data);
    if(events & EV_TYPE_READ)
    {
        FD_SET(handle, &(p->rset));
    }
    else
    {
        FD_CLR(handle, &(p->rset));
    }

    if(events & EV_TYPE_WRITE)
    {
        FD_SET(handle, &(p->wset));
    }
    else
    {
        FD_CLR(handle, &(p->wset));
    }
    return 0;
}



int io_init_select(void *io_base, int maxev)
{
	io_base_t *base = (io_base_t *)io_base;
	io_op_t *io_op = 0;
	io_select_data_t *model_data = 0;
#ifdef WIN32
        static int init = 0;
        if(init == 0)
        {
            WSADATA     wsadata;
            if(WSAStartup(MAKEWORD( 1, 1 ),&wsadata)!=0)
            {
                    return   -1;
            }
            init = 1;
        }
#endif
    model_data = (io_select_data_t *)(malloc(sizeof(io_select_data_t)));
    if(!model_data)
    {
        return -1;
    }
    model_data->maxfd = -1;
    FD_ZERO(&(model_data->rset));
    FD_ZERO(&(model_data->wset));
    FD_ZERO(&(model_data->rset_out));
    FD_ZERO(&(model_data->wset_out));

    io_op = &(base->op_imp);

    io_op->name = "select";
    io_op->_base_free = select_free;
    io_op->_base_dispatch = select_dispatch;
    io_op->_base_add = select_add;
    io_op->_base_del = select_del;
    io_op->_base_mod = select_mod;

	base->model_data = model_data;
    return 0;
}



#endif

