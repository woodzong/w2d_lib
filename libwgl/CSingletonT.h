#ifndef WSINGLETON_T_Hasdasdasd31231231
#define WSINGLETON_T_Hasdasdasd31231231

template <class TYPE>
class CSingletonT
{
public:
    static TYPE* Instance();

private:
    CSingletonT();
    CSingletonT(const CSingletonT&);
    ~CSingletonT(); 

private:
	TYPE s_aGuestInstance;
	static CSingletonT<TYPE> s_aHostInstance;
};

template <class TYPE>
CSingletonT<TYPE> CSingletonT<TYPE>::s_aHostInstance;

template <class TYPE>
CSingletonT<TYPE>::CSingletonT()
{
}

template <class TYPE>
CSingletonT<TYPE>::~CSingletonT()
{
}

template <class TYPE>
TYPE* 
CSingletonT<TYPE>::Instance()
{
    //Lock Guard
    //....
    return &(s_aHostInstance.s_aGuestInstance);
}

#endif