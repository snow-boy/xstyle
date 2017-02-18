#ifndef OBJECTPTR_H
#define OBJECTPTR_H
#include <list>
#include "pluginmanager.h"

namespace exsys{

template <typename T>
class ObjectPtr{
public:
    ObjectPtr(const char *iid)
    {
        Load(iid);
    }

    ObjectPtr(const ObjectPtr<T> &other)
    {
        p_ = other.p_;
    }

    ObjectPtr(T *p)
    {
        p_ = p;
    }

    ObjectPtr():
        p_(nullptr)
    {}

    void Load(const char *iid)
    {
        p_ = PlnMgr.QueryObject<T>(iid);
    }

    T *GetP()
    {
        return p_;
    }

    T *operator->() const
    {
        return p_;
    }

    ObjectPtr<T> & operator=(const ObjectPtr<T> &other)
    {
        p_ = other.p_;
    }

    bool operator==(const ObjectPtr<T> &other)
    {
        return (p_ == other.p_);
    }

    bool operator!=(const ObjectPtr<T> &other)
    {
        return !((*this) == other);
    }

    bool operator==(nullptr_t)
    {
        return (p_ == nullptr);
    }

    bool operator!=(nullptr_t)
    {
        return (p_ != nullptr);
    }

private:
    T *p_;
};

template <typename T>
class ObjectPtrList: public std::list<ObjectPtr<T>>
{
public:
    ObjectPtrList(const char *iid)
    {
        Load(iid);
    }

    ObjectPtrList()
    {}

    void Load(const char *iid)
    {
        clear();
        for(T* p:PlnMgr.QueryObjects<T>(iid)){
            push_back(ObjectPtr<T>(p));
        }
    }
};

}

#define DefObjPtr(interface, iid)\
    typedef class __##interface##ObjPtr: public exsys::ObjectPtr<interface>\
    {\
        public:\
            __##interface##ObjPtr():exsys::ObjectPtr<interface>(iid)\
            {}\
            __##interface##ObjPtr(std::nullptr_t)\
            {}\
            void Load()\
            {\
                exsys::ObjectPtr<interface>::Load(iid);\
            }\
            static bool Retain()\
            {\
                return PlnMgr.RetainObjects(iid);\
            }\
            static void Release()\
            {\
                PlnMgr.ReleaseObjects(iid);\
            }\
    }

#define DefObjPtrList(interface, iid)\
    typedef class __##interface##ObjPtrList: public exsys::ObjectPtrList<interface>\
    {\
        public:\
            __##interface##ObjPtrList():\
                exsys::ObjectPtrList<interface>(iid)\
            {}\
            __##interface##ObjPtrList(std::nullptr_t)\
            {}\
            void Load()\
            {\
                exsys::ObjectPtrList<interface>::Load(iid);\
            }\
    }

#endif // OBJECTPTR_H
