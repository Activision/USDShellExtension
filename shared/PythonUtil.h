#pragma once

#include <exception>

class CPyInterpreter
{
public:
    CPyInterpreter()
    {
        Py_Initialize();
    }
    ~CPyInterpreter()
    {
        Py_Finalize();
    }
};

class CPyObject
{
public:
    constexpr CPyObject() noexcept {}
    constexpr CPyObject( std::nullptr_t ) noexcept {}
    constexpr CPyObject( PyObject *pyObject ) noexcept 
    { 
        p = pyObject;  
    }
    CPyObject( CPyObject &&pyObject ) noexcept 
    { 
        p = pyObject.p; 
        pyObject.p = nullptr; 
    }
    ~CPyObject()
    {
        reset( nullptr );
    }

    PyObject *get() const noexcept
    {
        return p;
    }

    PyObject *release() noexcept
    {
        PyObject *pyObject = p;
        p = nullptr;
        return pyObject;
    }

    void reset( PyObject *pyObject ) noexcept
    {
        if ( p != nullptr )
        {
            Py_DECREF( p );
        }

        p = pyObject;
    }
    void reset( std::nullptr_t ) noexcept
    {
        if ( p != nullptr )
        {
            Py_DECREF( p );
        }

        p = nullptr;
    }

    void swap( CPyObject &other ) noexcept
    {
        PyObject *pTemp = other.get();
        other.p = p;
        p = pTemp;
    }

    explicit operator bool() const noexcept
    {
        return get() != nullptr;
    }

    PyObject operator*() const
    {
        return *p;
    }

    PyObject* operator->() const noexcept
    {
        return p;
    }

    CPyObject &operator=( CPyObject &&rhs ) noexcept
    {
        reset( rhs.p );
        rhs.p = nullptr;

        return *this;
    }

    CPyObject &operator=( std::nullptr_t ) noexcept
    {
        reset( nullptr );

        return *this;
    }

    bool operator==(PyObject* pT) const noexcept
    {
        return p == pT;
    }

    PyObject *p = nullptr;
};

class CPyString : public CPyObject
{
public:
    constexpr CPyString() noexcept {}
    constexpr CPyString( std::nullptr_t ) noexcept {}
    constexpr CPyString( PyObject *pyObject ) noexcept 
    { 
        p = pyObject;  
    }
    CPyString( CPyString &&pyObject ) noexcept 
    { 
        p = pyObject.p; 
        pyObject.p = nullptr; 
    }
    ~CPyString()
    {
        reset( nullptr );
    }

    explicit operator bool() const noexcept
    {
        return get() != nullptr;
    }

    PyObject operator*() const
    {
        return *p;
    }

    PyObject* operator->() const noexcept
    {
        return p;
    }

    CPyString &operator=( CPyString &&rhs ) noexcept
    {
        reset( rhs.p );
        rhs.p = nullptr;

        return *this;
    }

    CPyString &operator=( std::nullptr_t ) noexcept
    {
        reset( nullptr );

        return *this;
    }

    bool operator==(PyObject* pT) const noexcept
    {
        return p == pT;
    }


    const char *c_str() const noexcept
    {
        if ( p == nullptr )
            return "";

        const char* s = PyUnicode_AsUTF8( p );
        if ( s == nullptr )
            return "";

        return s;
    }

    static CPyString FromObject( PyObject *pyObject )
    {
        if ( pyObject == nullptr )
            return CPyString();

        CPyString result( PyObject_Str( pyObject ) );
        return result;
    }
};

class CPyStringW : public CPyObject
{
public:
    constexpr CPyStringW() noexcept {}
    constexpr CPyStringW( std::nullptr_t ) noexcept {}
    constexpr CPyStringW( PyObject *pyObject ) noexcept 
    { 
        p = pyObject;  
    }
    CPyStringW( CPyStringW &&pyObject ) noexcept 
    { 
        p = pyObject.p; 
        pyObject.p = nullptr; 
    }
    ~CPyStringW()
    {
        reset( nullptr );
    }

    explicit operator bool() const noexcept
    {
        return get() != nullptr;
    }

    PyObject operator*() const
    {
        return *p;
    }

    PyObject* operator->() const noexcept
    {
        return p;
    }

    CPyStringW &operator=( CPyStringW &&rhs ) noexcept
    {
        reset( rhs.p );
        rhs.p = nullptr;

        return *this;
    }

    CPyStringW &operator=( std::nullptr_t ) noexcept
    {
        reset( nullptr );

        return *this;
    }

    bool operator==(PyObject* pT) const noexcept
    {
        return p == pT;
    }


    const wchar_t *c_str() const noexcept
    {
        if ( p == nullptr )
            return L"";

        if ( s == nullptr )
            s = PyUnicode_AsWideCharString( p, nullptr );

        return s;
    }

    void reset( PyObject *pyObject ) noexcept
    {
        if ( s )
        {
            PyMem_Free ( s );
            s = nullptr;
        }

        CPyObject::reset( pyObject );
    }
    void reset( std::nullptr_t ) noexcept
    {
        if ( s )
        {
            PyMem_Free ( s );
            s = nullptr;
        }

        CPyObject::reset( nullptr );
    }

    static CPyStringW FromObject( PyObject *pyObject )
    {
        if ( pyObject == nullptr )
            return CPyStringW();

        CPyStringW result( PyObject_Str( pyObject ) );
        return result;
    }

private:
    mutable wchar_t *s = nullptr;
};

inline CStringW StringListToString( PyObject *pyObject )
{
    CStringW result;

    if ( pyObject == nullptr )
        return result;

    CPyObject itr = PyObject_GetIter( pyObject );
    if( itr == nullptr )
        return result;

    for ( ;; )
    {
        CPyObject item = PyIter_Next( itr.get() );
        if ( item == nullptr )
            break;

        CPyStringW str = CPyStringW::FromObject( item.get() );
        result += str.c_str();
    }

    return result;
}


class CPyException : public std::exception
{
public:
    CPyException()
    {
        if ( PyErr_Occurred() )
        {
            PyErr_Fetch( &m_pType.p, &m_pValue.p, &m_pTraceback.p );
            PyErr_NormalizeException( &m_pType.p, &m_pValue.p, &m_pTraceback.p );

            m_pTracebackObject = reinterpret_cast<PyTracebackObject*>(m_pTraceback.get());

            m_sValue = CPyString::FromObject( m_pValue.get() );
            m_sValueW = CPyStringW::FromObject( m_pValue.get() );

            m_sTypeW = CPyStringW::FromObject( m_pType.get() );
        }
    }

    _NODISCARD virtual char const* what() const override
    {
        return m_sValue ? m_sValue.c_str() : "Unknown exception";
    }

    _NODISCARD wchar_t const* whatW() const
    {
        return m_sValueW ? m_sValueW.c_str() : L"Unknown exception";
    }

    wchar_t const* typeW() const
    {
        return m_sTypeW ? m_sTypeW.c_str() : L"Unknown type";
    }

    const wchar_t *tracebackW() const
    {
        if ( m_bTracebackResolved == false )
        {
            m_bTracebackResolved = true;

            CPyObject importTraceback = PyImport_ImportModule( "traceback" );
            if ( importTraceback == nullptr )
                return L"";

            CPyObject funcFormatException = PyObject_GetAttrString( importTraceback.get(), "format_exception" );
            if ( funcFormatException == nullptr )
                return L"";

            CPyObject result = PyObject_CallFunctionObjArgs( funcFormatException.get(), m_pType.get(), m_pValue.get(), m_pTraceback.get(), nullptr );
            if ( result == nullptr )
                return L"";

            m_sTraceback = StringListToString( result.get() );
        }

        return m_sTraceback;
    }

    bool IsExceptionSystemExit() const
    {
        // We cannot use PyErr_GivenExceptionMatches() because it prevents the ability to delay link pythonXX.dll
        //return _wcsicmp(m_sTypeW.c_str(), L"<class 'SystemExit'>") == 0;
        return PyErr_GivenExceptionMatches( GetException(), PyExc_SystemExit ) != 0;
    }

    PyObject *GetException() const { return m_pType.get(); }
    PyObject *GetValue() const { return m_pValue.get(); }

private:
    CPyObject m_pType;
    CPyObject m_pValue;
    CPyObject m_pTraceback;

    PyTracebackObject *m_pTracebackObject = nullptr;

    CPyString m_sValue;
    CPyStringW m_sValueW;
    CPyStringW m_sTypeW;
    mutable CStringW m_sTraceback;

    mutable bool m_bTracebackResolved = false;
};

class CPyImport : public CPyObject
{
public:
    CPyImport(const char* name)
    {
        p = PyImport_ImportModule( name );
        if ( p == nullptr )
        {
            throw CPyException();
        }
    }
};

class CPyAttr : public CPyObject
{
public:
    CPyAttr(PyObject* object, const char* name)
    {
        p = PyObject_GetAttrString( object, name );
        if ( p == nullptr )
        {
            throw CPyException();
        }
    }
};

class CPyInstance : public CPyObject
{
public:
    CPyInstance(PyObject* object, const char* name)
    {
        // locate the object
        CPyObject pAttr = PyObject_GetAttrString( object, name );
        if ( pAttr == nullptr )
        {
            throw CPyException();
        }

        // create an instance of the object
        p = PyObject_CallObject( pAttr.get(), nullptr );
        if ( p == nullptr )
        {
            throw CPyException();
        }
    }
};

class CPyCallMethod : public CPyObject
{
public:
    template <typename... Args>
    CPyCallMethod(PyObject* object, const char* name, const char* format, Args&&... args)
    {
        p = PyObject_CallMethod( object, name, format, std::forward<Args>(args)... );
        if ( p == nullptr )
        {
            throw CPyException();
        }
    }
};

class CPyCallFunction : public CPyObject
{
public:
    template <typename... Args>
    CPyCallFunction(PyObject* object, const char* format, Args&&... args)
    {
        p = PyObject_CallFunction( object, format, std::forward<Args>(args)... );
        if ( p == nullptr )
        {
            throw CPyException();
        }
    }
};


