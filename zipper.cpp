#include "zipper.h"
#include <stdexcept>
#include <vector>
#include <string>
#include <cstdio>

namespace JavaScript_Zip {

    extern "C" {
        #include "deps/miniz.h" 
    }

    static std::string base_name(const std::string& p) {
        size_t pos = p.find_last_of("/\\");
        return pos == std::string::npos ? p : p.substr(pos + 1);
    }

    static std::vector<uint8_t> read_all(const std::string& path) {
        std::FILE* fp = std::fopen(path.c_str(), "rb");
        if (!fp) throw std::runtime_error("open fail: " + path);

        if (_fseeki64(fp, 0, SEEK_END) != 0) { std::fclose(fp); throw std::runtime_error("seek fail"); }
        long long len = _ftelli64(fp);
        if (len < 0) { std::fclose(fp); throw std::runtime_error("tell fail"); }
        if (_fseeki64(fp, 0, SEEK_SET) != 0) { std::fclose(fp); throw std::runtime_error("seek fail"); }

        size_t n = static_cast<size_t>(len);
        std::vector<uint8_t> buf(n);
        if (n && std::fread(buf.data(), 1, n, fp) != n) {
            std::fclose(fp);
            throw std::runtime_error("read fail: " + path);
        }
        std::fclose(fp);
        return buf;
    }

    template <class Add>
    static std::vector<uint8_t> build_zip_heap(Add add) {
        mz_zip_archive zip{};
        if (!mz_zip_writer_init_heap(&zip, 0, 0))
            throw std::runtime_error("zip init fail");
        try {
            add(zip);
            void* out_ptr = nullptr;
            size_t out_len = 0;
            if (!mz_zip_writer_finalize_heap_archive(&zip, &out_ptr, &out_len))
                throw std::runtime_error("finalize fail");
            std::vector<uint8_t> out((uint8_t*)out_ptr, (uint8_t*)out_ptr + out_len);
            mz_free(out_ptr);
            mz_zip_writer_end(&zip);
            return out;
        }
        catch (...) {
            mz_zip_writer_end(&zip);
            throw;
        }
    }

    std::vector<uint8_t> zip_from_memory(const std::vector<ZipEntryMem>& entries) {
        return build_zip_heap([&](mz_zip_archive& zip) {
            for (const auto& e : entries) {
                if (!mz_zip_writer_add_mem(&zip, e.name.c_str(), e.data, e.size, MZ_BEST_SPEED))
                    throw std::runtime_error("add fail: " + e.name);
            }
            });
    }

    //std::

    std::vector<uint8_t> zip_from_files(const std::vector<std::string>& paths) {
    return build_zip_heap([&](mz_zip_archive& zip) {
            for (const auto& p : paths) {
                auto data = read_all(p);
                std::string name = base_name(p);
                if (!mz_zip_writer_add_mem(&zip, name.c_str(), data.data(), data.size(), MZ_BEST_SPEED))
                    throw std::runtime_error("add fail: " + name);
            }
        });
    }

    void zip_files_to_path(const std::vector<std::string>& paths, const std::string& out_zip) {
        mz_zip_archive zip{};
        if (!mz_zip_writer_init_file(&zip, out_zip.c_str(), 0))
            throw std::runtime_error("zip init file fail");
        try {
            for (const auto& p : paths) {
                std::string name = base_name(p);
                if (!mz_zip_writer_add_file(&zip, name.c_str(), p.c_str(), nullptr, 0, MZ_BEST_SPEED))
                    throw std::runtime_error("add file fail: " + name);
            }
            if (!mz_zip_writer_finalize_archive(&zip))
                throw std::runtime_error("finalize fail");
            mz_zip_writer_end(&zip);
        }
        catch (...) {
            mz_zip_writer_end(&zip);
            throw;
        }
    }
}