#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>

namespace JavaScript_Zip {
	struct ZipEntryMem {
		std::string name;
		const uint8_t* data;
		size_t size;
	};

	std::vector<uint8_t> zip_from_memory(const std::vector<ZipEntryMem>& entries);
	std::vector<uint8_t> zip_from_files(const std::vector<std::string>& paths);
	void zip_files_to_path(const std::vector<std::string>& paths, const std::string& out_zip);
}