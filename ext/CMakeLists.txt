
# EASTL C++ library
if (EASTL)
	add_library(eastl STATIC
		EASTL/source/allocator_eastl.cpp EASTL/source/assert.cpp
		EASTL/source/fixed_pool.cpp EASTL/source/hashtable.cpp
		EASTL/source/intrusive_list.cpp EASTL/source/numeric_limits.cpp
		EASTL/source/red_black_tree.cpp EASTL/source/string.cpp
	)
	target_include_directories(eastl PUBLIC
		EASTL/include
		EASTL/test/packages/EABase/include/Common
	)
endif()

# Tiny-printf library
add_library(tinyprintf STATIC
		tinyprintf/tinyprintf.c
	)
target_include_directories(tinyprintf PUBLIC
		tinyprintf
	)
