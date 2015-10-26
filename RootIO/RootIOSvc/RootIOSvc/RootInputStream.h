#ifndef ROOT_INPUT_STREAM_H
#define ROOT_INPUT_STREAM_H 0

#include "IInputStream.h"
#include "RootIOStream.h"

class RootInputStream : public IInputStream, public RootIOStream {

    public:
        RootInputStream() : RootIOStream("RootInputStream") {}
        ~RootInputStream() {}

        virtual TObject* get() { return 0; }
};

#endif
