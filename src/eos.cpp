#include "endian.h"

int main() {
	byte *ptr = 0;

	uint64 a = READ_LE_UINT64(ptr);
	return a;
}
