#include "AckRange.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
AckRange::AckRange()
    : m_startSequenceNumber(INVALID_SEQUENCE_NUMBER)
    , m_count(0)
{
}

//----------------------------------------------------------------------------------------------------
AckRange::AckRange(SequenceNumber startSequenceNumber)
    : m_startSequenceNumber(startSequenceNumber)
    , m_count(1)
{
}

//----------------------------------------------------------------------------------------------------
void AckRange::Write(OutputMemoryStream& outputStream) const
{
    outputStream.Write(m_startSequenceNumber);
    const bool hasRange = m_count > 1;
    outputStream.Write(hasRange);
    if (hasRange) {
        outputStream.Write(m_count);
    }
}

//----------------------------------------------------------------------------------------------------
void AckRange::Read(InputMemoryStream& inputStream)
{
    inputStream.Read(m_startSequenceNumber);
    bool hasRange = false;
    inputStream.Read(hasRange);
    if (hasRange) {
        inputStream.Read(m_count);
    } else {
        m_count = 1;
    }
}

//----------------------------------------------------------------------------------------------------
bool AckRange::AddSequenceNumber(SequenceNumber sequenceNumber)
{
    SequenceNumber nextSequenceNumber = m_startSequenceNumber + static_cast<SequenceNumber>(m_count);
    if (sequenceNumber == nextSequenceNumber) {
        ++m_count;
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------------------------------
SequenceNumber AckRange::GetStartSequenceNumber() const
{
    return m_startSequenceNumber;
}

//----------------------------------------------------------------------------------------------------
U8 AckRange::GetCount() const
{
    return m_count;
}
}
