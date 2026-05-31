#include "joinCode.h"

namespace mynet::joinCode {

std::string IntToCode(uint64_t value, const Config& config) {
	if(value == 0)
		return std::string(1, config.digits[0]);

	std::string res;
	while (value > 0) {
		res.push_back(config.digits[value % config.base]);
		value /= config.base;
	}
	std::reverse(res.begin(), res.end());
	return res;
}

bool CheckCode(const std::string& code, const Config& config) {
	uint64_t res = 0;
	for (char c : code) {
		int val = config.lookup[int(c)];
		if (val < 0)
			return false;
		if (res > (UINT64_MAX - val) / config.base)
			return false;
		res = res * config.base + val;
	}
	return true;
}

uint64_t CodeToInt(const std::string& code, const Config& config) {
	uint64_t res = 0;
	for (char c : code) {
		int val = config.lookup[int(c)];
		if (val < 0)
			throw std::invalid_argument("Invalid character in code!");
		if (res > (UINT64_MAX - val) / config.base)
			throw std::overflow_error("Code represents number too large for uint64_t!");
		res = res * config.base + val;
	}
	return res;
}


std::string EncodeJoinCode(const std::string& ip, uint16_t port, const Config& config) {
	ENetAddress addr;
	if (enet_address_set_host(&addr, ip.c_str()) != 0)
		throw std::invalid_argument("Invalid IPv4 address!");

	uint64_t packed = (uint64_t(addr.host) << 16) | port;

	return IntToCode(packed, config);
}

std::pair<std::string, uint16_t> DecodeJoinCode(const std::string& code, const Config& config) {
	uint64_t packed = CodeToInt(code, config);
	
	uint16_t port = uint16_t(packed & 0xFFFF);
	uint32_t ipInt = uint32_t(packed >> 16);

	ENetAddress addr;
	addr.host = ipInt;

	char ip[64];
	if (enet_address_get_host_ip(&addr, ip, sizeof(ip)) != 0)
		throw std::runtime_error("Failed to decode IP address!");

	return { std::string(ip), port };
}

} //namespace mynet::joinCode