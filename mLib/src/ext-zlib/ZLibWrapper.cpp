namespace ml
{

std::vector<BYTE> ZLibWrapper::CompressStreamToMemory(const std::vector<BYTE> &decompressedStream, bool writeHeader)
{
    std::vector<BYTE> result;
    CompressStreamToMemory(decompressedStream, result, writeHeader);
    return result;
}

std::vector<BYTE> ZLibWrapper::CompressStreamToMemory(const BYTE *decompressedStream, UINT64 decompressedStreamLength, bool writeHeader)
{
    std::vector<BYTE> result;
    CompressStreamToMemory(decompressedStream, decompressedStreamLength, result, writeHeader);
    return result;
}

void ZLibWrapper::CompressStreamToMemory(const std::vector<BYTE> &decompressedStream, std::vector<BYTE> &compressedStream, bool writeHeader)
{
    CompressStreamToMemory(&decompressedStream[0], decompressedStream.size(), compressedStream, writeHeader);
}

void ZLibWrapper::CompressStreamToMemory(const BYTE *decompressedStream, UINT64 decompressedStreamLength, std::vector<BYTE> &compressedStream, bool writeHeader)
{
    compressedStream.resize(decompressedStreamLength + 64);

    z_stream zstream;

    zstream.zalloc = Z_NULL;
    zstream.zfree = Z_NULL;
    zstream.opaque = Z_NULL;

    zstream.avail_in = (uInt)decompressedStreamLength;
    zstream.next_in = const_cast<BYTE*>(decompressedStream);

    zstream.data_type = Z_BINARY;

    zstream.avail_out = (uInt)decompressedStreamLength + 64;

    int headerOffset = sizeof(UINT64);
    if (!writeHeader) headerOffset = 0;
    zstream.next_out = &compressedStream[0] + headerOffset;

    const int Level = 7; // 1 (fastest speed) to 9 (best compression)
    int result = deflateInit2(&zstream, Level, Z_DEFLATED, 15, 8, Z_DEFAULT_STRATEGY);
    if (result != 0) throw MLIB_EXCEPTION("deflateInit2 failed");

    result = deflate(&zstream, Z_FINISH);
    if (result != Z_STREAM_END) throw MLIB_EXCEPTION("deflate failed");

    deflateEnd(&zstream);

    if (writeHeader) {
        ((UINT64*)&compressedStream[0])[0] = decompressedStreamLength;
    }

    int compressedSize = zstream.total_out;
    if (writeHeader)
        compressedSize += sizeof(UINT64);
    compressedStream.resize(compressedSize);
}

std::vector<BYTE> ZLibWrapper::DecompressStreamFromMemory(const std::vector<BYTE> &compressedStream)
{
    std::vector<BYTE> result;
    DecompressStreamFromMemory(compressedStream, result);
    return result;
}

void ZLibWrapper::DecompressStreamFromMemory(const std::vector<BYTE> &compressedStream, std::vector<BYTE> &decompressedStream)
{
    UINT decompressedByteCount = ((UINT*)&compressedStream[0])[0];
    decompressedStream.resize(decompressedByteCount);
    DecompressStreamFromMemory(&compressedStream[0] + sizeof(UINT64), compressedStream.size() - sizeof(UINT64), &decompressedStream[0], decompressedStream.size());
}

void ZLibWrapper::DecompressStreamFromMemory(const BYTE *compressedStream, UINT64 compressedStreamLength, BYTE *decompressedStream, UINT64 decompressedStreamLength)
{
    if (decompressedStreamLength == 0) throw MLIB_EXCEPTION("Caller must provide the length of the decompressed stream");

    uLongf finalByteCount = (uLongf)decompressedStreamLength;
    int result = uncompress(decompressedStream, &finalByteCount, compressedStream, (uLong)compressedStreamLength);
    if (result != Z_OK) throw MLIB_EXCEPTION("uncompress failed");
    if (finalByteCount != decompressedStreamLength) throw MLIB_EXCEPTION("Decompression returned invalid length");
}

void ZLibWrapper::CompressStreamToFile(const BYTE *decompressedStream, UINT64 decompressedStreamLength, const std::string &filename)
{
    FILE *file = util::checkedFOpen(filename.c_str(), "wb");
    std::vector<BYTE> compressedStream = CompressStreamToMemory(decompressedStream, decompressedStreamLength, true);
    //CompressStreamToMemory(const BYTE *decompressedStream, UINT64 decompressedStreamLength, std::vector<BYTE> &compressedStream, bool writeHeader)
    
    size_t length = compressedStream.size();
    util::checkedFWrite(&length, sizeof(size_t), 1, file);
    util::checkedFWrite(compressedStream.data(), sizeof(BYTE) * compressedStream.size(), 1, file);
    fclose(file);
}

std::vector<BYTE> ZLibWrapper::DecompressStreamFromFile(const std::string &filename)
{
    FILE *file = util::checkedFOpen(filename.c_str(), "rb");

    size_t length = -1;
    util::checkedFRead(&length, sizeof(size_t), 1, file);

    std::vector<BYTE> compressedStream(length);
    util::checkedFRead(compressedStream.data(), sizeof(BYTE) * compressedStream.size(), 1, file);
    fclose(file);

    return DecompressStreamFromMemory(compressedStream);
}

}  // namespace ml