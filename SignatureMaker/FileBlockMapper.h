#pragma once
#include <boost\interprocess\file_mapping.hpp>
#include <boost\interprocess\mapped_region.hpp>
#include <string>

class FileBlockMapper
{
public:
    FileBlockMapper(const std::string& filename, size_t blockSize);
    size_t MapForRead();
    size_t MapForReadWrite(size_t size);
    size_t Remap(size_t blockId);

    uint8_t* GetMapPtr() const;
    size_t GetMapSize() const;

    size_t GetPageSize() const;
    size_t GetBlockSize() const;
    size_t GetBlocksCount() const;

private:
    size_t MapImpl(size_t blockId = 0);
    void RecreateFile(size_t size);
    void CalculateFileSize();
    void CreateMapping(boost::interprocess::mode_t mode);

private:
    std::string m_filename;
    size_t m_blockSize = 0;
    size_t m_fileSize = 0;
    std::unique_ptr<boost::interprocess::file_mapping> m_pFileMapping;
    std::unique_ptr<boost::interprocess::mapped_region> m_pMappedRegion;
    boost::interprocess::mode_t m_mode = boost::interprocess::mode_t::read_only;

    uint8_t* m_pFile = nullptr;
    size_t m_mapSize = 0;
    size_t m_pageSize = 0;
    size_t m_currentBlockSize = 0;
    size_t m_blocksCount = 0;
};
