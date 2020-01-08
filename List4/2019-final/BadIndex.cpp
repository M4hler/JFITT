#include "BadIndex.hpp"

BadIndex::BadIndex(string varId, long long index, long long begin, long long end)
{
	message = to_string(index) + " out of bounds in \"" + varId + "\"(indexed from " + to_string(begin) + " to " + to_string(end) + ")";
}
