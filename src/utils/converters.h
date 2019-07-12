uint8_t fromHexByte(const char *byteStr);
__int128 atoi128(const char *s);
template <typename T> T fromHex(string hexStr, bool bigEndianBytes = true);
template <> vector<uint8_t> fromHex<vector<uint8_t>>(string hexStr, bool bigEndianBytes);
