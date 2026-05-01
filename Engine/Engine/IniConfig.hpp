#ifndef INICONFIG_HPP
#define INICONFIG_HPP

#include <fstream>
#include <map>
#include <string>

namespace ValyrianEngine {

// Pure C++ INI-style config reader — replaces al_load_config_file / al_get_config_value.
// Supports a default (unnamed) section and named [Section] sections.
class IniConfig {
public:
	bool load(const std::string& path) {
		std::ifstream f(path);
		if(!f.is_open())
			return false;

		std::string currentSection;
		std::string line;
		while(std::getline(f, line)) {
			line = trim(line);
			if(line.empty() || line[0] == ';' || line[0] == '#')
				continue;

			if(line[0] == '[') {
				auto close = line.find(']');
				if(close != std::string::npos)
					currentSection = trim(line.substr(1, close - 1));
				continue;
			}

			auto eq = line.find('=');
			if(eq == std::string::npos)
				continue;

			auto key = trim(line.substr(0, eq));
			auto val = trim(line.substr(eq + 1));
			m_data[currentSection][key] = val;
		}
		return true;
	}

	const char* getValue(const std::string& section, const std::string& key) const {
		auto secIt = m_data.find(section);
		if(secIt == m_data.end())
			return nullptr;
		auto keyIt = secIt->second.find(key);
		if(keyIt == secIt->second.end())
			return nullptr;
		return keyIt->second.c_str();
	}

private:
	std::map<std::string, std::map<std::string, std::string>> m_data;

	static std::string trim(const std::string& s) {
		auto start = s.find_first_not_of(" \t\r\n");
		if(start == std::string::npos)
			return {};
		auto end = s.find_last_not_of(" \t\r\n");
		return s.substr(start, end - start + 1);
	}
};

} // namespace ValyrianEngine
#endif
