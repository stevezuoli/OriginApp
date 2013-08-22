////////////////////////////////////////////////////////////////////////
// TPComBase.h
// Contact: xzhan
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef TPCOMBASE_H_
#define TPCOMBASE_H_

#include "dkWinError.h"
#include "dkBaseType.h"


#define GUID_STRING_LEN 36

#define TOHEX(c) ((unsigned char)(((c)<='9' && (c)>='0')?((c)-'0'):(((c)<='f'&&(c)>='a')?((c)-'a'+10):(((c)<='F'&&(c)>='A')?((c)-'A'+10):0))))
#define TOBYTE(sz) ((unsigned char)(TOHEX(*(sz))*(1<<4) + TOHEX(*(sz+1))))
#define TOWORD(sz) ((unsigned short)(TOBYTE(sz)*(1<<8) + TOBYTE(sz+2)))
#define TODWORD(sz) ((unsigned long)(TOWORD(sz)*(1<<16) + TOWORD(sz+4)))

template <typename T_Type>
class CPtrBase
{
public:
    /*--------------------------------------------------------------------------
        CPtrBase::Constructor
    --------------------------------------------------------------------------*/
    CPtrBase() // default
    {
        m_p = NULL;
    }

    /*--------------------------------------------------------------------------
        CPtrBase::operator T_Type * (cast to ordinary T_Type *)
    --------------------------------------------------------------------------*/
    operator T_Type *() const
    {
        return m_p;
    }

    /*--------------------------------------------------------------------------
        CPtrBase::operator * (dereference)
    --------------------------------------------------------------------------*/
    T_Type & operator * () const
    {   
        return *m_p;
    }

    T_Type * operator -> () const
    {
        return m_p;
    }

    /*--------------------------------------------------------------------------
        CPtrBase::operator & (address of)
    --------------------------------------------------------------------------*/
    T_Type ** operator & ()
    {
        return &m_p;
    }

    /*--------------------------------------------------------------------------
        CPtrBase::AddressOf

        Same as operator & except that we don't assert that the current
        contents of m_p are null.
    --------------------------------------------------------------------------*/
    T_Type ** AddressOf()
    {
        return &m_p;
    }
    
private:
    T_Type * m_p;
}; // class CPtrBase<>

template <
    typename T_Type,
    typename T_Cast = T_Type >
class CIPtr : public CPtrBase<T_Type>
{
protected:
    typedef CIPtr<T_Type, T_Cast> T_This;
public:
    /*--------------------------------------------------------------------------
        CIPtr::Constructor
    --------------------------------------------------------------------------*/
    // default
    CIPtr()
    {
    }

    // T_Type *
    CIPtr(T_Type * p)
    {
        AddRef(p);
        *(this->AddressOf()) = p;
    }

    // another CIPtr
    CIPtr(const T_This & sp)
    {
        T_Type * const p = (T_Type *)sp;
        AddRef(p);
        *(this->AddressOf()) = p;
    }


    /*--------------------------------------------------------------------------
        CIPtr::Destructor
    --------------------------------------------------------------------------*/
    ~CIPtr()
    {
        T_Type * const pOld = *(this->AddressOf());
        Release(pOld);
    }


    /*--------------------------------------------------------------------------
        CIPtr::Release
    --------------------------------------------------------------------------*/
    void Release()
    {
        Assign(NULL);
    }


    /*--------------------------------------------------------------------------
        CIPtr::operator =
    --------------------------------------------------------------------------*/
     // T_Type *
    T_This & operator = (T_Type * p)
    {
        Assign(p);
        return *this;
    }

    // another CIPtr
    T_This & operator = (const T_This & sp)
    {
        Assign((T_Type *)sp);
        return *this;
    }

    /*--------------------------------------------------------------------------
        CIPtr::Assign

        Assign a new T_Type * into us, making a new reference on the new
        object and releasing any old one.

        After a lot of thought about the precisely correct order in which
        this should do work, we've concluded that if it did anything other
        than AddRef() and then Attach() it would have to mean that either
        we or Attach() is broken. How math-major is that? :)
    --------------------------------------------------------------------------*/
    void Assign(T_Type * p)
    {
        AddRef(p);
        Attach(p);
    }

    /*--------------------------------------------------------------------------
        CIPtr::Attach

        Same as Assign except this takes over an existing reference.
    --------------------------------------------------------------------------*/
    void Attach(T_Type * p)
    {
        T_Type * const pOld = *(this->AddressOf());
        *(this->AddressOf()) = p; // 'Tis important to update *(this->AddressOf()) before Release(pOld)
        Release(pOld);
    }

    /*--------------------------------------------------------------------------
        CIPtr::Detach

        Extract an ordinary T_Type * without releasing it.
    --------------------------------------------------------------------------*/
    T_Type * Detach()
    {
        T_Type * const p = *(this->AddressOf());
        *(this->AddressOf()) = NULL;
        return p;
    }


private:
    void AddRef(T_Type * p)
    {
        if (p)
            ((T_Cast *)p)->AddRef();
    }

    void Release(T_Type * p)
    {
        if (p)
            ((T_Cast *)p)->Release();
    }
}; // class CIPtr<>

#endif /*TPCOMBASE_H_*/
