#ifndef ROOT_IO_STREAM
#define ROOT_IO_STREAM 1

class RootIOStream {

    public:
        RootIOStream() : m_addr(0) {}
        virtual ~RootIOStream() {}

    protected:
        void* m_addr;

};

#endif
