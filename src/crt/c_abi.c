#include <kprint.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

__attribute__((noreturn)) void panic(const char*);
void* _impure_ptr = NULL;

void __stack_chk_fail_local()
{
	panic("Stack protector: Canary modified");
	__builtin_unreachable();
}
__attribute__((used))
void __stack_chk_fail()
{
	panic("Stack protector: Canary modified");
	__builtin_unreachable();
}
void __assert_fail(const char *assertion,
                    const char *file,
                    unsigned int line,
                    const char *function)
{
	char buffer[4096];
	snprintf(buffer, sizeof(buffer),
			"Assertion failed: %s in %s:%u, function %s\n",
			assertion, file, line, function);
	panic(buffer);
	__builtin_unreachable();
}
void __assert_func(
	const char *file,
	int line,
	const char *func,
	const char *failedexpr)
{
	kprintf(
		"assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
		failedexpr, file, line,
		func ? ", function: " : "", func ? func : "");
	extern void abort() __attribute__((noreturn));
	abort();
}
void _exit(int status)
{
	char buffer[64];
	snprintf(buffer, sizeof(buffer),
			"Exit called with status %d\n", status);
	panic(buffer);
	__builtin_unreachable();
}

__attribute__((used))
void* memset(char* dest, int ch, size_t size)
{
	for (size_t i = 0; i < size; i++)
		dest[i] = ch;
	return dest;
}
void* memcpy(char* dest, const char* src, size_t size)
{
	for (size_t i = 0; i < size; i++)
		dest[i] = src[i];
	return dest;
}
void* memmove(char* dest, const char* src, size_t size)
{
	if (dest <= src)
	{
		for (size_t i = 0; i < size; i++)
			dest[i] = src[i];
	}
	else
	{
		for (int i = size-1; i >= 0; i--)
			dest[i] = src[i];
	}
	return dest;
}
int memcmp(const void* ptr1, const void* ptr2, size_t n)
{
	const uint8_t* iter1 = (const uint8_t*) ptr1;
	const uint8_t* iter2 = (const uint8_t*) ptr2;
	while (n > 0 && *iter1 == *iter2) {
		iter1++;
		iter2++;
		n--;
	}
	return n == 0 ? 0 : (*iter1 - *iter2);
}

// naive version (needed for EASTL)
float ceilf(float n)
{
	long int i = (int) n;
	return (n == (float) i) ? n : i + 1;
}

char* strcpy(char* dst, const char* src)
{
	while ((*dst++ = *src++));
	*dst = 0;
	return dst;
}
size_t strlen(const char* str)
{
	const char* iter;
	for (iter = str; *iter; ++iter);
	return iter - str;
}
int strcmp(const char* str1, const char* str2)
{
	while (*str1 != 0 && *str2 != 0  && *str1 == *str2) {
      str1++;
      str2++;
   }
   return *str1 - *str2;
}
char* strcat(char* dest, const char* src)
{
	strcpy(dest + strlen(dest), src);
	return dest;
}

void* __memcpy_chk(void* dest, const void* src, size_t len, size_t destlen)
{
  assert (len <= destlen);
  return memcpy(dest, src, len);
}
void* __memset_chk(void* dest, int c, size_t len, size_t destlen)
{
  assert (len <= destlen);
  return memset(dest, c, len);
}
char* __strcat_chk(char* dest, const char* src, size_t destlen)
{
  size_t len = strlen(dest) + strlen(src) + 1;
  assert (len <= destlen);
  return strcat(dest, src);
}

int isdigit(int c)
{
	return (('0' <= c) && (c <= '9')) ? 1 : 0;
}
int isalpha(int c)
{
 	return ((c > 64 && c < 91) || (c > 96 && c <= 122)) ? 1 : 0;
}
int isxdigit(int c)
{
	return (isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'));
}
int isupper(int c)
{
	return (c >= 'A' && c <= 'Z');
}
