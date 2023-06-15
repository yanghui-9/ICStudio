// sigslot.h: Signal/Slot classes
// Written by Sarah Thompson (sarah@telergy.com) 2002
// more info : http://sigslot.sourceforge.net/
// ** only reserve no paramete function for CSerialPort 4.0 2019-07-21 itas109 **

#ifndef SIGSLOT_H__
#define SIGSLOT_H__

#include <set>
#include <list>




namespace sigslot {


class _connection_base0
{
public:
    virtual void _emit(int len) = 0;
};

template<class dest_type>
class _connection0: public _connection_base0
{
public:
    _connection0()
    {
        this->pobject = NULL;
        this->pmemfun = NULL;
    }

    _connection0(dest_type* pobject, void (dest_type::*pmemfun)(int))
    {
        m_pobject = pobject;
        m_pmemfun = pmemfun;
    }

    virtual void _emit(int len)
    {
        (m_pobject->*m_pmemfun)(len);
    }

private:
    dest_type* m_pobject;
    void (dest_type::* m_pmemfun)(int);
};

class signal0
{
public:
    typedef std::list<_connection_base0 *>  connections_list;
    typedef  connections_list::const_iterator const_iterator;
    connections_list m_connected_slots;

    signal0()
    {
        ;
    }

    template<class desttype>
    void connect(desttype* pclass, void (desttype::*pmemfun)(int))
    {
        _connection0<desttype>* conn =
                new _connection0<desttype>(pclass, pmemfun);
        this->m_connected_slots.push_back(conn);
    }

    void _emit(int len)
    {
        const_iterator itNext, it = this->m_connected_slots.begin();
        const_iterator itEnd = this->m_connected_slots.end();

        while(it != itEnd)
        {
            itNext = it;
            ++itNext;

            (*it)->_emit(len);

            it = itNext;
        }
    }

    void _disconnectAll()
    {
        this->m_connected_slots.clear(); ;
    }

    void operator()()
    {
        const_iterator itNext, it = this->m_connected_slots.begin();
        const_iterator itEnd = this->m_connected_slots.end();

        while(it != itEnd)
        {
            itNext = it;
            ++itNext;

            (*it)->_emit(0);

            it = itNext;
        }
    }
};

};

#endif // SIGSLOT_H__
