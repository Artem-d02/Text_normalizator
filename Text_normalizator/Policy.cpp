#include "Policy.h"
#include <algorithm>

namespace NRML {

	struct ConstWords {
		static const std::string PUT;
		static const std::string GET;
		static const std::string POST;
	};

	const std::string ConstWords::PUT = "PUT";
	const std::string ConstWords::GET = "GET";
	const std::string ConstWords::POST = "POST";


	bool checkSelfReferencing(const std::string& str) {
		std::string targetString = "/./";
		bool result = (str.find(targetString) != std::string::npos);
		return result;
	}

	bool fixSelfReferencing(std::string& str) {
		std::string targetString = "/./";
		auto startTargetIndex = str.find(targetString);
		while (startTargetIndex != std::string::npos) {
			auto startIt = str.begin() + startTargetIndex;
			str.replace(startIt, startIt + targetString.length(), "/");
			startTargetIndex = str.find(targetString);
		}
		return false;
	}

	bool checkPathTraversal(const std::string& str) {
		std::string targetString = "/../";
		bool result = (str.find(targetString) != std::string::npos);
		return result;
	}

	bool fixPathTraversal(std::string& str) {
		std::string targetString = "/../";
		auto startTargetIndex = str.find(targetString);
		while (startTargetIndex != std::string::npos) {
			auto startIt = str.begin() + startTargetIndex;
			auto endIt = startIt + targetString.length();

			//	find previous /
			auto currentIt = startIt;
			if (currentIt != str.begin())
				currentIt--;	// just for skip first / in /../
			while ((*currentIt != '/') && (currentIt != str.begin())) {
				currentIt--;
			}
			//	check PUT, GET, POST
			if (currentIt == str.begin()) {
				if (str.substr(0, 3) == ConstWords::PUT || str.substr(0, 3) == ConstWords::GET) {
					currentIt = str.begin() + 4;	//	SIZEOF(PUT_)
				}
				if (str.substr(0, 4) == ConstWords::POST)
					currentIt = str.begin() + 5;	//	SIZEOF(Post_)
			}
			str.replace(currentIt, endIt, "/");
			startTargetIndex = str.find(targetString);
		}
		return false;
	}

	bool checkURLEncoding(const std::string& str) {
		std::string targetString = "%";
		auto percentIndex = str.find(targetString);
		auto isHexNumber = [](char s) {
			if (('0' <= s && s <= '9') || ('a' <= s && s <= 'f') || ('A' <= s && s <= 'F'))
				return true;
			else
				return false;
		};
		//	if was not found
		if (percentIndex == std::string::npos)
			return false;
		//	if this is the last symbol
		if (percentIndex == str.length() - 1)
			return false;
		//	if this is the previous for the last symbol
		if (percentIndex == str.length() - 2)
			return false;

		if (isHexNumber(str[percentIndex + 1]) && isHexNumber(str[percentIndex + 2]))
			return true;

		return false;
	}

	bool fixURLEncoding(std::string& str) {
		char targetChar = '%';
		auto isHexNumber = [](char s) {
			if (('0' <= s && s <= '9') || ('a' <= s && s <= 'f') || ('A' <= s && s <= 'F'))
				return true;
			else
				return false;
		};
		auto hexToDec = [](std::string hex) {
			std::transform(hex.begin(), hex.end(), hex.begin(), [](unsigned char c) { return std::toupper(c); });
			auto oneHexToDec = [](char h) -> char {
				if ('0' <= h && h <= '9')
					return h - '0';
				else if ('A' <= h && h <= 'F')
					return h - 'A' + 10;
			};
			return oneHexToDec(hex[0]) * 16 + oneHexToDec(hex[1]);
		};

		if (str.length() <= 2)
			return false;

		for (int i = 0; i < str.length(); i++) {
			if (str[i] != targetChar)
				continue;
			int percentIndex = i;
			//	if this is the last symbol
			if (percentIndex == str.length() - 1)
				return false;
			//	if this is the previous for the last symbol
			if (percentIndex == str.length() - 2)
				return false;

			auto startIt = str.begin() + percentIndex;
			if (isHexNumber(str[percentIndex + 1]) && isHexNumber(str[percentIndex + 2])) {
				char asciiCode = hexToDec(str.substr(percentIndex + 1, 2));
				if (asciiCode <= 126) {
					str.replace(startIt, startIt + 3, std::string({ asciiCode }));
				}
			}
		}
		return false;
	}

}	//	namespace NRML