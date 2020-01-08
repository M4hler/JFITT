#include "UndeclaredVariable.hpp"

UndeclaredVariable::UndeclaredVariable(string varId)
{
	message = "\"" + varId + "\" hasn't been declared";
}
