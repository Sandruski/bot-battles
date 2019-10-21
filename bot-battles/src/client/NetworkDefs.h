#ifndef __NETWORK_DEFS_H__
#define __NETWORK_DEFS_H__

namespace sand {

	using NetworkID = U32;
	const NetworkID INVALID_NETWORK_ID = -1; // TODO: this should be the number of MAX_NETWORK_IDs

	enum class Endianness 
	{
		LITTLE_ENDIAN, // Intel's x86, Intel's x64, Apple's iOS
		BIG_ENDIAN // Xbox 360, PlayStation 3, IBM's PowerPC
	};

	const Endianness STREAM_ENDIANNESS = Endianness::BIG_ENDIAN;
	const Endianness PLATFORM_ENDIANNESS = Endianness::LITTLE_ENDIAN;
}

#endif
