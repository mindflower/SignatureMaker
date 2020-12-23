#include "FileBlockMapper.h"
#include <fstream>
#include <filesystem>

using namespace boost::interprocess;
using namespace std;

namespace
{
    constexpr size_t GIGABYTE = 1024 * 1024 * 1024;

    size_t DivideWithCeil(const size_t lhd, const size_t rhd)
    {
        const auto result = static_cast<double>(lhd) / rhd;
        return static_cast<size_t>(ceil(result));
    }
}

FileBlockMapper::FileBlockMapper(const string& filename, const size_t blockSize) :
    m_filename(filename),
    m_blockSize(blockSize),
    m_pageSize(mapped_region::get_page_size())
{
}

size_t FileBlockMapper::MapForRead()
{
    CalculateFileSize();
    CreateMapping(read_only);
    return MapImpl();
}

size_t FileBlockMapper::MapForReadWrite(const size_t size)
{
    RecreateFile(size);
    CalculateFileSize();
    CreateMapping(read_write);
    return MapImpl();
}

size_t FileBlockMapper::Remap(const size_t blockId)
{
    return MapImpl(blockId);
}

uint8_t* FileBlockMapper::GetMapPtr() const
{
    return m_pFile;
}

size_t FileBlockMapper::GetMapSize() const
{
    return m_mapSize;
}

size_t FileBlockMapper::GetPageSize() const
{
    return m_pageSize;
}

size_t FileBlockMapper::GetBlockSize() const
{
    return m_currentBlockSize;
}

size_t FileBlockMapper::GetBlocksCount() const
{
    return m_blocksCount;
}

size_t FileBlockMapper::MapImpl(const size_t blockId)
{
    const auto blockOffset = blockId * m_blockSize;
    if (blockOffset > m_fileSize)
    {
        throw runtime_error("Block " + to_string(blockId) + " of of file bounds");
    }

    const auto pageId = blockOffset / m_pageSize;
    const auto pageOffset = pageId * m_pageSize;
    m_mapSize = min(GIGABYTE, m_fileSize - pageOffset);
    m_pMappedRegion = make_unique<mapped_region>(*m_pFileMapping, m_mode, pageOffset, m_mapSize);
    m_pFile = static_cast<uint8_t*>(m_pMappedRegion->get_address());
    m_currentBlockSize = min(m_mapSize, m_blockSize);
    return DivideWithCeil(m_mapSize, m_currentBlockSize);
}

void FileBlockMapper::RecreateFile(const size_t size)
{
    filesystem::remove(m_filename);
    filebuf fbuf;
    fbuf.open(m_filename, ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary);
    if (fbuf.is_open())
    {
        fbuf.pubseekoff(size - 1, ios_base::beg);
        fbuf.sputc(0);
    }
    else
    {
        throw runtime_error("Can't open file for writing: " + m_filename);
    }
}

void FileBlockMapper::CalculateFileSize()
{
    m_fileSize = filesystem::file_size(m_filename);
    m_blocksCount = DivideWithCeil(m_fileSize, m_blockSize);
}

void FileBlockMapper::CreateMapping(const mode_t mode)
{
    m_mode = mode;
    m_pFileMapping = make_unique<file_mapping>(m_filename.c_str(), m_mode);
}
