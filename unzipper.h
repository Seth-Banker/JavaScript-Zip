#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <filesystem>

namespace JavaScript_Zip {

    extern "C" {
        #include "deps/miniz.h" 
    }

	struct ZippedFile {
		std::string name;
		std::vector<uint8_t> data;
	};

    static std::vector<ZippedFile> unzipAllFromMemory(const unsigned char* zipData, size_t zipSize) {
        mz_zip_archive zip{};
        if (!mz_zip_reader_init_mem(&zip, zipData, zipSize, 0))
            throw std::runtime_error("invalid zip");

        std::vector<ZippedFile> out;
        mz_uint n = mz_zip_reader_get_num_files(&zip);

        for (mz_uint i = 0; i < n; ++i) {
            if (mz_zip_reader_is_file_a_directory(&zip, i)) continue;

            mz_zip_archive_file_stat st{};
            if (!mz_zip_reader_file_stat(&zip, i, &st)) continue;

            size_t sz = 0;
            void* p = mz_zip_reader_extract_to_heap(&zip, i, &sz, 0);
            if (!p) { mz_zip_reader_end(&zip); throw std::runtime_error("extract failed"); }

            ZippedFile f;
            f.name = st.m_filename;                 // path inside zip
            f.data.assign(static_cast<uint8_t*>(p), // file bytes
                static_cast<uint8_t*>(p) + sz);
            mz_free(p);
            out.push_back(std::move(f));
        }

        mz_zip_reader_end(&zip);
        return out;
    }
	

    static void writeAllToDisk(const std::vector<ZippedFile>& files, const std::string& outDir) {
        for (const auto& f : files) {
            std::filesystem::path p = std::filesystem::path(outDir) / f.name;
            std::filesystem::create_directories(p.parent_path());
            std::ofstream out(p, std::ios::binary);
            if (!out) throw std::runtime_error("cannot open output file");
            out.write(reinterpret_cast<const char*>(f.data.data()), static_cast<std::streamsize>(f.data.size()));
        }
    }
}