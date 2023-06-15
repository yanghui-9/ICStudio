#ifndef EXTENDFUNCHELPER_H
#define EXTENDFUNCHELPER_H

#define ZMaximumParamCount 7
#define Z_ARG(type, data) ZArgument<type >(#type, data)
#define Z_RETURN_ARG(type, data) ZReturnArgument<type >(#type, data)

class ZGenericArgument
{
public:
    inline ZGenericArgument(const char *aName = nullptr, const void *aData = nullptr)
        : _data(aData), _name(aName) {}
    inline void *data() const { return const_cast<void *>(_data); }
    inline const char *name() const { return _name; }

private:
    const void *_data;
    const char *_name;
};

class ZGenericReturnArgument: public ZGenericArgument
{
public:
    inline ZGenericReturnArgument(const char *aName = nullptr, void *aData = nullptr)
        : ZGenericArgument(aName, aData)
        {}
};

template <class T>
class ZArgument: public ZGenericArgument
{
public:
    inline ZArgument(const char *aName, const T &aData)
        : ZGenericArgument(aName, static_cast<const void *>(&aData))
        {}
};
template <class T>
class ZArgument<T &>: public ZGenericArgument
{
public:
    inline ZArgument(const char *aName, T &aData)
        : ZGenericArgument(aName, static_cast<const void *>(&aData))
        {}
};


template <typename T>
class ZReturnArgument: public ZGenericReturnArgument
{
public:
    inline ZReturnArgument(const char *aName, T &aData)
        : ZGenericReturnArgument(aName, static_cast<void *>(&aData))
        {}
};


#endif // EXTENDFUNCHELPER_H
