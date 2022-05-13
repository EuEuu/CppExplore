
#include <iostream>
#include <vector>
#include <sstream>

namespace kmp {
	void getNext(const std::string& needle, std::vector<int>& next) {
		next.clear();
		if (needle.empty()) {
			return;
		}
		next.resize(needle.size());
		int j = 0;
		next[0] = 0;
		for (int i = 1; i < needle.size(); i++)
		{
			//needle: abaabbabab
			//  next: 0011201232
			while (j > 0 && needle[i] != needle[j]) {
				j = next[j - 1];
			}
			if (needle[i] == needle[j]) {
				j++;
			}
			next[i] = j;
		}
	}

	int substr(const std::string& haystack, const std::string& needle) {
		if (needle.empty()) {
			return 0;
		}

		std::vector<int> next;
		kmp::getNext(needle, next);

		int j = 0;
		for (size_t i = 0; i < haystack.size(); i++)
		{
			while (j > 0 && haystack[i] != needle[j])
			{
				j = next[j - 1];
			}
			if (haystack[i] == needle[j]) {
				j++;
			}
			if (j == needle.size()) {
				return (i - j + 1);
			}
		}
		return -1;
	}
}

int main(int argc, char** argv) {


	int pos = kmp::substr("asdasfagadsfsaabaabbababasdasdasdas", "abaabbabab");
	std::cout << pos << std::endl;

	pos = kmp::substr("asdasfagadsfsaabaabbababasdasdasdas", "");
	std::cout << pos << std::endl;

	pos = kmp::substr("abaabbababasdasdasdas", "abaabbabab");
	std::cout << pos << std::endl;

	pos = kmp::substr("asdasjhfkadsfgafadsghj", "abaabbabab");
	std::cout << pos << std::endl;

	return 0;
}