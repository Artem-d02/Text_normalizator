#pragma once
#include <map>
#include <functional>
#include <string>

namespace NRML {

	struct PolicyPriority {
		int policyLevel;
		std::function<bool(const std::string&)> checkFunction;
		PolicyPriority(std::function<bool(const std::string&)> check, const int level) noexcept
			:	policyLevel(level), checkFunction(check)
		{}
	};

	struct FuncCompare {
		bool operator()(const PolicyPriority& lhs, const PolicyPriority& rhs) const {
			return lhs.policyLevel < rhs.policyLevel;
		}
	};

	class RuleContainer final {
		std::map < PolicyPriority, std::function<bool(std::string&)>, FuncCompare > rules;	//	set<condition, handler>
	public:
		RuleContainer() = default;
		void addRule(const std::pair<PolicyPriority, std::function<bool(std::string&)>>& rule) noexcept {
			rules.insert(rule);
		}
		void clear() noexcept {
			rules.clear();
		}
		auto getRules() noexcept {
			return rules;
		}
	};

	class TextNormalizator final {
	private:
		RuleContainer ruleContainer;
		std::string filePath;
		const size_t lowerBorderOfMeaningfulSymbols{ 32 };		//	in ascii
		const size_t upperBorderOfMeaningfulSymbols{ 126 };		//	in ascii
		std::string normalizeFilePath(const std::string&) const noexcept;
		bool isContainStrangeSymbols{ false };
	public:
		TextNormalizator() = delete;
		TextNormalizator(const std::string& path, const RuleContainer& rules) noexcept;
		std::string normilize() noexcept;
	};

}	//	namespace NRML