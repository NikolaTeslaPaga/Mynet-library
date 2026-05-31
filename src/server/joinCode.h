#pragma once
#ifndef _SERVER_JOINCODE_H_
#define _SERVER_JOINCODE_H_

#include <string>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <array>

#include <enet/enet.h>

namespace mynet::joinCode {

	struct Config {
	public:
		constexpr Config() : Config(36) {}
		constexpr explicit Config(size_t _base)
			: base(_base),
			digits(_base <= maxDigits.size()
				? maxDigits.substr(0, _base)
				: throw std::invalid_argument("base is out of bounds!")),
			lookup(generate_lookup(digits))
		{}

		static constexpr size_t MaxBase() {
			return maxDigits.size();
		}

	private:

		static constexpr std::array<int, 256> generate_lookup(std::string_view d) {
			std::array<int, 256> table;
			table.fill(-1);
			for (size_t i = 0; i < d.size(); i++)
				table[d[i]] = i;
			return table;
		}
	public:
		size_t base;
		const std::string_view digits;
		const std::array<int, 256> lookup;
	private:
		static constexpr std::string_view maxDigits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	};

	std::string IntToCode(uint64_t value, const Config& config);

	bool CheckCode(const std::string& code, const Config& config);

	uint64_t CodeToInt(const std::string& code, const Config& config);

	std::string EncodeJoinCode(const std::string& ip, uint16_t port, const Config& config);

	std::pair<std::string, uint16_t> DecodeJoinCode(const std::string& code, const Config& config);

} //namespace mynet::joinCode

#endif //_SERVER_JOINCODE_H_