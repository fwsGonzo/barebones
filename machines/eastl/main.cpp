#include <main.h>
#include <assert.h>
#include <kprint.h>
#include <exception>

#include <EASTL/vector.h>
class IdioticException : public std::exception
{
    const char* oh_god;
public:
	IdioticException(const char* reason) : oh_god(reason) {}
    const char* what() const noexcept override
    {
        return oh_god;
    }
};

using callback_t = int (*) (eastl::vector<int>&);

void kernel_main(uint32_t /*eax*/, uint32_t /*ebx*/)
{
	kprintf(KERNEL_DESC "\n");

	const callback_t callback = [] (auto& vec) -> int {
		kprintf("EASTL vector size: %zu (last element is %d)\n",
				vec.size(), vec.back());
		throw IdioticException{"Test!"};
	};

	eastl::vector<int> test;
	int caught = 0;
	for (int i = 0; i < 16; i++)
	{
		test.push_back(i);
		try {
			assert(callback(test) == 0);
		}
		catch (const std::exception& e) {
			kprintf("Exceptions caught: %d\n", ++caught);
		}
	}
	assert(caught == 16);
}
