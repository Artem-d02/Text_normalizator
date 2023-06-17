#include "Text_normalizator.h"
#include <fstream>
#include <algorithm>
#include <iostream>

namespace NRML {

	TextNormalizator::TextNormalizator(const std::string& path, const RuleContainer& rules) noexcept
		:	filePath(path), ruleContainer(rules)
	{}

	std::string TextNormalizator::normalizeFilePath(const std::string& path) const noexcept {
		std::string result(path);
		auto it = std::find(result.rbegin(), result.rend(), '.');
		size_t lastDotIndex = std::distance(result.begin(), (it + 1).base());
		result = result.substr(0, lastDotIndex);
		return result;
	}

	std::string TextNormalizator::normilize() noexcept {
		std::fstream fin;	//	source file
		std::fstream fout;	//	result file
		std::string resultName = normalizeFilePath(filePath) + "_NORMALIZED" + ".txt";
		fin.open(filePath);
		if (!fin.is_open()) {
			std::cout << "Error: file could not found" << std::endl;
			exit(-1);
		}

		fout.open(resultName, std::ios_base::out);
		if (!fin.is_open()) {
			std::cout << "Error: target file was not created" << std::endl;
			exit(-1);
		}

		auto isInRange = [](auto num, auto lowBorder, auto upBorder) {
			if ((lowBorder <= num) && (num <= upBorder))
				return true;
			else
				return false;
		};

		while (!fin.eof()) {
			char symbol;
			fin >> std::noskipws >> symbol;
			std::string meaningfulSequence;

			//	skip all non-ascii symbols
			while (!isInRange(symbol, lowerBorderOfMeaningfulSymbols, upperBorderOfMeaningfulSymbols) && !fin.eof()) {
				fout << symbol;
				fin >> std::noskipws >> symbol;
			};

			while (isInRange(symbol, lowerBorderOfMeaningfulSymbols, upperBorderOfMeaningfulSymbols) && !fin.eof()) {
				meaningfulSequence.push_back(symbol);
				fin >> std::noskipws >> symbol;
			};

			for (auto& rule : ruleContainer.getRules()) {
				if (rule.first.checkFunction(meaningfulSequence))
					if (rule.second(meaningfulSequence))	//	do smth with sequence
						isContainStrangeSymbols = true;		//	check in %00
			}
			for (auto newSymbol : meaningfulSequence) {
				fout << newSymbol;
			}

			if (!fin.eof())
				fout << symbol;
		}

		fin.close();
		fout.close();

		return resultName;
	}

}	//	namespace NRML