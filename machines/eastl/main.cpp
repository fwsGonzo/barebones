#include <main.h>
#include <assert.h>
#include <kprint.hpp>

#include <EASTL/vector.h>

using callback_t = int (*) (eastl::vector<int>&);

void kernel_main(uint32_t /*eax*/, uint32_t /*ebx*/)
{
	kprintf(KERNEL_DESC "\n");

	const callback_t callback = [] (auto& vec) -> int {
		kprintf("EASTL vector size: %u (last element is %d)\n",
				vec.size(), vec.back());
		return 0;
	};

	eastl::vector<int> test;
	for (int i = 0; i < 16; i++)
	{
		test.push_back(i);
		assert(callback(test) == 0);
	}
}
