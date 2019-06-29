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
static void test_rtti();


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
	test_rtti();
	throw IdioticException("This is on purpose");
}

class A
{
public:
	virtual void f() { kprintf("A::f() called\n"); }
};

class B : public A
{
public:
	void f() { kprintf("B::f() called\n"); }
};

static void test_rtti()
{
	A a;
	B b;
	a.f();        // A::f()
	b.f();        // B::f()

	A *pA = &a;
	B *pB = &b;
	pA->f();      // A::f()
	pB->f();      // B::f()

	pA = &b;
	// pB = &a;      // not allowed
	pB = dynamic_cast<B*>(&a); // allowed but it returns NULL
	assert(pB == nullptr);
}
