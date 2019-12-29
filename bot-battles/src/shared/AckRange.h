#ifndef __ACK_RANGE_H__
#define __ACK_RANGE_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class AckRange {
public:
    AckRange();
    AckRange(SequenceNumber startSequenceNumber);

    void Write(OutputMemoryStream& outputStream) const;
    void Read(InputMemoryStream& inputStream);

    bool AddSequenceNumber(SequenceNumber sequenceNumber);

    SequenceNumber GetStartSequenceNumber() const;
    U8 GetCount() const;

private:
    SequenceNumber m_startSequenceNumber;
    U8 m_count;
};
}

#endif
