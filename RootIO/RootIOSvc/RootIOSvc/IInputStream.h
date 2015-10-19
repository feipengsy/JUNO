#ifndef I_INPUT_STREAM_H
#define I_INPUT_STREAM_H 1

class TObject;

class IInputStream {

    public:
        IInputStream() {}
        virtual ~IInputStream() {}

        // Interface to initialize and finalize this stream
        virtual bool initialize();
        virtual bool finalize();
        // Interfaces to handle the stream
        // Read the event if read is set to true
        virtual bool next(int step = 1, bool read = true);
        virtual bool previous(int step = 1, bool read = true);
        virtual bool first(bool read = true);
        virtual bool last(bool read = true);
        virtual bool setEntry(int entry, bool read = true);
        virtual TObject* get() = 0;

};

inline bool IInputStream::initialize()
{
    return true;
}

inline bool IInputStream::finalize()
{
    return true;
}

// Default implementations of stream-control methods
// Should never be called

inline bool IInputStream::next(int step, bool read)
{
    return false;
}

inline bool IInputStream::previous(int step, bool read)
{
    return false;
}

inline bool IInputStream::first(bool read)
{
    return false;
}

inline bool IInputStream::last(bool read)
{
    return false;
}

inline bool IInputStream::first(bool read)
{
    return false;
}

inline bool IInputStream::setEntry(int entry, bool read)
{
    return false;
}

#endif
