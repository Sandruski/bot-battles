#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
class CircularBuffer {
public:
    CircularBuffer();

    void Add(const T& element);
    void Remove(U32 position);
    void RemoveFirst();
    void RemoveLast();
    void Clear();

    const T& Get(U32 position) const;
    T& Get(U32 position);
    const T& GetFirst() const;
    T& GetFirst();
    const T& GetLast() const;
    T& GetLast();

    bool IsEmpty() const;
    bool IsFull() const;
    U32 Count() const;

private:
    U32 IndexFromPosition(U32 position) const;

public:
    U32 m_front;
    U32 m_back;

private:
    std::array<T, size> m_buffer;
};

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
inline CircularBuffer<T, size>::CircularBuffer()
    : m_front(1)
    , m_back(1)
    , m_buffer()
{
}

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
inline void CircularBuffer<T, size>::Add(const T& element)
{
    assert(!IsFull());

    U32 position = m_back;
    U32 index = IndexFromPosition(position);
    m_buffer[index] = element;
    ++m_back;
}

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
inline void CircularBuffer<T, size>::Remove(U32 position)
{
    assert(position < m_back);
    m_front = position + 1;
}

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
inline void CircularBuffer<T, size>::RemoveFirst()
{
    Remove(m_front);
}

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
inline void CircularBuffer<T, size>::RemoveLast()
{
    Remove(m_back - 1);
}

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
inline void CircularBuffer<T, size>::Clear()
{
    m_front = m_back;
}

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
inline const T& CircularBuffer<T, size>::Get(U32 position) const
{
    return Get(position);
}

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
inline T& CircularBuffer<T, size>::Get(U32 position)
{
    U32 index = IndexFromPosition(position);
    return m_buffer.at(index);
}

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
inline const T& CircularBuffer<T, size>::GetFirst() const
{
    return GetFirst();
}

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
inline T& CircularBuffer<T, size>::GetFirst()
{
    assert(m_back > m_front);
    U32 position = m_front;
    U32 index = IndexFromPosition(position);
    return Get(index);
}

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
inline const T& CircularBuffer<T, size>::GetLast() const
{
    return GetLast();
}

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
inline T& CircularBuffer<T, size>::GetLast()
{
    assert(m_back > m_front);
    U32 position = m_back - 1;
    U32 index = IndexFromPosition(position);
    return Get(index);
}

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
inline bool CircularBuffer<T, size>::IsEmpty() const
{
    return Count() == 0;
}

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
inline bool CircularBuffer<T, size>::IsFull() const
{
    return Count() == m_buffer.size();
}

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
inline U32 CircularBuffer<T, size>::Count() const
{
    return m_back - m_front;
}

//----------------------------------------------------------------------------------------------------
template <class T, U32 size>
inline U32 CircularBuffer<T, size>::IndexFromPosition(U32 position) const
{
    return position % m_buffer.size();
}
}

#endif
