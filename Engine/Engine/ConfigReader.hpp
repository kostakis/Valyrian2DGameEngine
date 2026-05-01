#ifndef CONFIGREADER_HPP
#define CONFIGREADER_HPP

#include "IniConfig.hpp"
#include "MyDebug.hpp"

#include <cassert>
#include <map>
#include <string>

namespace ValyrianEngine {

class ConfigReader {
public:
	~ConfigReader() = default;

	static ConfigReader& getInstance() {
		static ConfigReader instance;
		return instance;
	}

	const char* getConfigValue(const std::string& path, const char* key) {
		const char* value = getValue(path, "", key);
		assert(value);
		return value;
	}

	const char* getValue(const std::string& path, const std::string& section, const std::string& key) {
		auto it = m_configs.find(path);
		if(it == m_configs.end()) {
			IniConfig cfg;
			if(!cfg.load(path))
				return nullptr;
			m_configs.emplace(path, std::move(cfg));
			it = m_configs.find(path);
		}
		return it->second.getValue(section, key);
	}

private:
	std::map<std::string, IniConfig> m_configs;
};

} // namespace ValyrianEngine
#endif
