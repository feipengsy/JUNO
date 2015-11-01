#ifndef I_INPUT_STREAM_H
#define I_INPUT_STREAM_H 1

#define UNUSED __attribute__((unused))

#include <string>

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
        virtual bool getObj(TObject*& obj, const std::string& objName);

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
// Use UNUSED macro to eliminate warnings

inline bool IInputStream::next(int step UNUSED, bool read UNUSED)
{
    return false;
}

inline bool IInputStream::previous(int step UNUSED, bool read UNUSED)
{
    return false;
}

inline bool IInputStream::first(bool read UNUSED)
{
    return false;
}

inline bool IInputStream::last(bool read UNUSED)
{
    return false;
}

inline bool IInputStream::setEntry(int entry UNUSED, bool read UNUSED)
{
    return false;
}

inline bool IInputStream::getObj(TObject*& obj UNUSED, const std::string& objName UNUSED)
{
    return false;
}

#endif
