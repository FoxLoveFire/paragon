#include "sdk.h"

bool sdk::initialize()
{
	if (!I::initialize())
		return false;

	return true;
}
