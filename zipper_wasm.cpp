#include "zipper.h"
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <cstdlib>

namespace JavaScript_Zip {
    extern "C" {

        uint8_t* create_zip_wasm(const char** names, const uint8_t** bufs, const int* sizes, int n, int* out_len) {
            *out_len = 0;
            try {
                std::vector<ZipEntryMem> entries;
                entries.reserve(n);
                for (int i = 0; i < n; ++i) {
                    entries.push_back(ZipEntryMem{ std::string(names[i]), bufs[i], static_cast<size_t>(sizes[i]) });
                }
                auto zip = zip_from_memory(entries);
                uint8_t* out = static_cast<uint8_t*>(std::malloc(zip.size()));
                if (!out) return nullptr;
                std::memcpy(out, zip.data(), zip.size());
                *out_len = static_cast<int>(zip.size());
                return out;
            }
            catch (...) {
                return nullptr;
            }
        }

        void free_zip_buf(uint8_t* p) { std::free(p); }

    }
}