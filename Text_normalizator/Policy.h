#pragma once
#include <string>

namespace NRML {

	bool checkSelfReferencing(const std::string& str);

	bool fixSelfReferencing(std::string& str);

	bool checkPathTraversal(const std::string& str);

	bool fixPathTraversal(std::string& str);

	bool checkURLEncoding(const std::string& str);

	bool fixURLEncoding(std::string& str);

}	//	namespace NRML