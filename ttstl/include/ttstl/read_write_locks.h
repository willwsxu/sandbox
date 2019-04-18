// vim:ts=8:sw=4:sts=4:et:tw=78:ft=cpp:
/***************************************************************************
 *
 *                  Unpublished Work Copyright (c) 2014
 *                  Trading Technologies International, Inc.
 *                       All Rights Reserved Worldwide
 *
 *          * * *   S T R I C T L Y   P R O P R I E T A R Y   * * *
 *
 * WARNING:  This program (or document) is unpublished, proprietary property
 * of Trading Technologies International, Inc. and is to be maintained in
 * strict confidence. Unauthorized reproduction, distribution or disclosure
 * of this program (or document), or any program (or document) derived from
 * it is prohibited by State and Federal law, and by local law outside of
 * the U.S.
 *
 ***************************************************************************/
#ifndef __TTSTL_READ_WRITE_LOCKS_H__
#define __TTSTL_READ_WRITE_LOCKS_H__

#include <atomic>
#include "ttstl/platform.h"

namespace tt{

// Forward declarations
class WriteSpinLock;
class ReadSpinLock;

//----------------------------------------------------------------------------
//--  AtomicReadWrite  -------------------------------------------------------
//----------------------------------------------------------------------------
/** The Atomic Read/Write object that can be used to establish a reader-
 * writer relationship using the ReadSpinLock and WriteSpinLock classes.
 * - only one write is supported at a timer with no reader;
 * - multiple readers are supported at a timer with no writer;
 */
class AtomicReadWrite
{
public:
    AtomicReadWrite(const AtomicReadWrite& rhs) = delete;
    AtomicReadWrite(AtomicReadWrite&& rhs) = delete;
    AtomicReadWrite& operator=(const AtomicReadWrite& rhs) = delete;
    AtomicReadWrite& operator=(AtomicReadWrite&& rhs) = delete;

    AtomicReadWrite()
        : write_(false)
        , readCount_(0)
    {}

    ~AtomicReadWrite()
    {}

private:
    void WriteLock()
    {
        bool expected(false);
        // check if nothing else has raised an intention to write;
        // if that is the case, raise our intention to write and
        // break the spin to check for reading; otherwise keep
        // spinning to check and raise our intention.
        while (false == write_.compare_exchange_weak(
            expected, true,
            std::memory_order_release, std::memory_order_relaxed))
        {
            // spin
            expected = false;
        }

        // successfully raise our intention to write.
        // now check if anything has raised the intention to read;
        // if so, continue to spin; otherwise break the spin and
        // proceed to write.
        while (0 != readCount_.load(std::memory_order_acquire));
    }

    void WriteUnlock()
    {
        write_.store(false, std::memory_order_release);
    }

    void ReadLock()
    {
        // spin to raise the read intention and check for write intention.
        while(true)
        {
            // raise an intention to read by increasing the read count.
            readCount_.fetch_add(1, std::memory_order_release);
            // test if something is already intended on writing.
            // if not, hold the increased read count, break the spin to
            // continue reading.
            if (false == write_.load(std::memory_order_acquire))
                break;
            // if there is a write intention, decrease read count, so
            // that whatever is holding the write intention could finish
            // first.
            readCount_.fetch_add(-1, std::memory_order_release);

            // Wait until writers are finished
            while (true == write_.load(std::memory_order_acquire));
        }
    }

    void ReadUnlock()
    {
        readCount_.fetch_add(-1, std::memory_order_release);
    }

    friend class WriteSpinLock;
    friend class ReadSpinLock;
    friend class WriterLock;
    friend class ReaderLock;

    // The flag for the intention to write (support 1 writer).
    std::atomic<bool> write_;
    char pad0[TT_CACHE_LINE_SIZE - sizeof(std::atomic<bool>)];

    // The count for the intention to read (support n readers).
    std::atomic<int> readCount_;
    char pad1[TT_CACHE_LINE_SIZE - sizeof(std::atomic<int>)];
};


//----------------------------------------------------------------------------
//--  WriteSpinLock  ---------------------------------------------------------
//----------------------------------------------------------------------------
/** The Write Spin Lock that can be used to establish a write intetntion
 * on an AtomicReadWrite object.  The lock will spin if there is already
 * an existing writer or some readers.  The lock is scope based, i.e.
 * using the constructor to acquire and hold the lock, and the destructor
 * to release the lock.
 */
class WriteSpinLock
{
public:
    WriteSpinLock() = delete;
    WriteSpinLock(const WriteSpinLock& rhs) = delete;
    WriteSpinLock(WriteSpinLock&& rhs) = delete;
    WriteSpinLock& operator=(const WriteSpinLock& rhs) = delete;
    WriteSpinLock& operator=(WriteSpinLock&& rhs) = delete;

    WriteSpinLock(AtomicReadWrite& rw)
        : readWrite_(rw)
    {
        readWrite_.WriteLock();
    }

    ~WriteSpinLock()
    {
        readWrite_.WriteUnlock();
    }

private:
    AtomicReadWrite& readWrite_;
};

//----------------------------------------------------------------------------
//--  ReadSpinLock  ----------------------------------------------------------
//----------------------------------------------------------------------------
/** The Read Spin Lock that can be used to establish a read intetntion
 * on an AtomicReadWrite object.  The lock will spin if there is already
 * a writer; existing readers (with no writer) will not lock.  The lock
 * is scope based, i.e. using the constructor to acquire and hold the lock,
 * and the destructor to release the lock.
 */
class ReadSpinLock
{
public:
    ReadSpinLock() = delete;
    ReadSpinLock(const ReadSpinLock& rhs) = delete;
    ReadSpinLock(ReadSpinLock&& rhs) = delete;
    ReadSpinLock& operator=(const ReadSpinLock& rhs) = delete;
    ReadSpinLock& operator=(ReadSpinLock&& rhs) = delete;

    ReadSpinLock(AtomicReadWrite& rw)
        : readWrite_(rw)
    {
        readWrite_.ReadLock();
    }

    ~ReadSpinLock()
    {
        readWrite_.ReadUnlock();
    }

private:
    AtomicReadWrite& readWrite_;
};

//----------------------------------------------------------------
// std::lock_guard and std::unique_lock conformant
// example:
// class Foo
// {
//      Foo()
//      : writeLock(arw_);
//      , readLock(arw_)
//      {}
//      AtomicReadWrite arw_;
//      WriteLock writeLock_;
//      ReadLock readLock_;
//
//      ...
//      std::lock_guard<ReadLock> lck(readLock_);
//
//      std::lock_guard<WriteLock> lck(writeLock_);
//
// Or if you don't want to create separate read and write locks, you can do this
//      std::lock_guard<WriteLock> lck(WriterLock(atomicReadWrite));

class WriterLock
{
public:
    WriterLock() = delete;
    WriterLock(const WriterLock& rhs) = delete;
    WriterLock(WriterLock&& rhs) = delete;
    WriterLock& operator=(const WriterLock& rhs) = delete;
    WriterLock& operator=(WriterLock&& rhs) = delete;

    WriterLock(AtomicReadWrite& rw)
        : readWrite_(&rw)
    {
    }

    void lock()
    {
        readWrite_->WriteLock();
    }

    void unlock()
    {
        readWrite_->WriteUnlock();
    }

private:
    AtomicReadWrite *readWrite_;
};

class ReaderLock
{
public:
    ReaderLock() = delete;
    ReaderLock(const ReaderLock& rhs) = delete;
    ReaderLock(ReaderLock&& rhs) = delete;
    ReaderLock& operator=(const ReaderLock& rhs) = delete;
    ReaderLock& operator=(ReaderLock&& rhs) = delete;

    ReaderLock(AtomicReadWrite& rw)
        : readWrite_(&rw)
    {
    }

    void lock()
    {
        readWrite_->ReadLock();
    }

    void unlock()
    {
        readWrite_->ReadUnlock();
    }

private:
    AtomicReadWrite *readWrite_;
};

}       // end namespace tt 

#endif
