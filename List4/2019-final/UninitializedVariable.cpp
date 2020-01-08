#include "UninitializedVariable.hpp"

UninitializedVariable::UninitializedVariable(string varId)
{
	message = "Warning: \"" + varId + "\" hasn't been initialized";
}
