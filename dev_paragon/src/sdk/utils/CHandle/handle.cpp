#include "handle.h"

c_handle::c_handle() noexcept : index(INVALID_EHANDLE_INDEX) {}

c_handle::c_handle(const int entry, const int serial) noexcept
{
	index = entry | (serial << NUM_SERIAL_NUM_SHIFT_BITS);
}

bool c_handle::operator!=(const c_handle& other) const noexcept
{
	return index != other.index;
}

bool c_handle::operator==(const c_handle& other) const noexcept
{
	return index == other.index;
}

bool c_handle::operator<(const c_handle& other) const noexcept
{
	return index < other.index;
}

bool c_handle::is_valid() const noexcept
{
	return index != INVALID_EHANDLE_INDEX;
}

int c_handle::get_entry_index() const noexcept
{
	return static_cast<int>(index & ENT_ENTRY_MASK);
}

int c_handle::get_serial_number() const noexcept
{
	return static_cast<int>(index >> NUM_SERIAL_NUM_SHIFT_BITS);
}

int c_handle::to_int() const
{
	return index;
}