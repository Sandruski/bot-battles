#ifndef __ACK_RANGE_H__
#define __ACK_RANGE_H__

namespace sand {

class OutputMemoryStream;
class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
class AckRange {
public:
    AckRange();
    AckRange(SequenceNumber startSequenceNumber);
    ~AckRange();

    bool AddSequenceNumber(SequenceNumber sequenceNumber);

    void Write(OutputMemoryStream& outputStream) const;
    void Read(InputMemoryStream& inputStream);

    SequenceNumber GetStartSequenceNumber() const
    {
        return m_startSequenceNumber;
    }

    U8 GetCount() const
    {
        return m_count;
    }

private:
    SequenceNumber m_startSequenceNumber;
    U8 m_count;
};
}

#endif
