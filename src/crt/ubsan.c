#include <stdint.h>
#include <stdio.h>
#include <kprint.h>
extern void panic(const char*) __attribute__((noreturn));
extern void print_backtrace();

struct source_location {
	const char *file_name;
	struct {
		uint32_t line;
		uint32_t column;
	};
};
struct type_descriptor {
   uint16_t type_kind;
   uint16_t type_info;
   char type_name[1];
};
const char* type_kind_string[] = {
  "load of", "store to", "reference binding to", "member access within",
  "member call on", "constructor call on", "downcast of", "downcast of"
};

struct out_of_bounds {
   struct source_location  src;
   struct type_descriptor* array_type;
   struct type_descriptor* index_type;
};
struct overflow {
  struct source_location src;
};
struct mismatch {
  struct source_location  src;
  struct type_descriptor* type;
  unsigned char    log_align;
  unsigned char    type_kind;
};
struct function_type_mismatch {
  const struct source_location  src;
  const struct type_descriptor* type;
};
struct nonnull_return {
  struct source_location src;
  struct source_location attr;
};
struct unreachable {
  struct source_location src;
};

static void print_src_location(const struct source_location* src) {
  kprintf("ubsan: %s at line %u col %u\n",
          src->file_name, src->line, src->column);
}

static void undefined_throw(const char* error) {
  kprintf("ubsan: %s", error);
  print_backtrace();
  kprintf("\n");
}

/// Undefined-behavior sanitizer
void __ubsan_handle_out_of_bounds(struct out_of_bounds* data)
{
  print_src_location(&data->src);
  undefined_throw("Out-of-bounds access");
}
void __ubsan_handle_missing_return()
{
  undefined_throw("Missing return");
}
void __ubsan_handle_nonnull_return(struct nonnull_return* data)
{
  print_src_location(&data->src);
  undefined_throw("Non-null return");
}

void __ubsan_handle_add_overflow()
{
  undefined_throw("Overflow on addition");
}
void __ubsan_handle_sub_overflow()
{
  undefined_throw("Overflow on subtraction");
}
void __ubsan_handle_mul_overflow()
{
  undefined_throw("Overflow on multiplication");
}
void __ubsan_handle_negate_overflow()
{
  undefined_throw("Overflow on negation");
}
void __ubsan_handle_pointer_overflow()
{
  undefined_throw("Pointer overflow");
}
void __ubsan_handle_divrem_overflow(struct overflow* data,
                                    unsigned long lhs,
                                    unsigned long rhs)
{
  print_src_location(&data->src);
  kprintf("ubsan: LHS %lu / RHS %lu\n", lhs, rhs);
  undefined_throw("Division remainder overflow");
}
void __ubsan_handle_float_cast_overflow()
{
  undefined_throw("Float-cast overflow");
}
void __ubsan_handle_shift_out_of_bounds()
{
  undefined_throw("Shift out-of-bounds");
}

void __ubsan_handle_type_mismatch_v1(struct mismatch* data, uintptr_t ptr)
{
  print_src_location(&data->src);

  const char* reason = "Type mismatch";
  const long alignment = 1 << data->log_align;

  if (alignment && (ptr & (alignment-1)) != 0) {
    reason = "Misaligned access";
  }
  else if (ptr == 0) {
    reason = "Null-pointer access";
  }
  char buffer[2048];
  // TODO: resolve symbol name
  snprintf(buffer, sizeof(buffer),
          "%s on ptr %p  (aligned %lu)\n"
          "ubsan: type name %s\n",
          reason,
          (void*) ptr,
          alignment,
          data->type->type_name);
  undefined_throw(buffer);
}
void __ubsan_handle_function_type_mismatch(
        struct function_type_mismatch* data,
        unsigned long ptr)
{
  print_src_location(&data->src);

  char buffer[2048];
  snprintf(buffer, sizeof(buffer),
          "Function type mismatch on ptr %p\n"
          "ubsan: type name %s\n"
          "ubsan: function  %p",
          (void*) ptr,
          data->type->type_name,
          (void*) ptr); // TODO: resolve symbol name
  undefined_throw(buffer);
}
void __ubsan_handle_nonnull_arg()
{
  undefined_throw("Non-null argument violated");
}

void __ubsan_handle_invalid_builtin()
{
  undefined_throw("Invalid built-in function");
}
void __ubsan_handle_load_invalid_value()
{
  undefined_throw("Load of invalid value");
}
__attribute__((noreturn))
void __ubsan_handle_builtin_unreachable(struct unreachable* data)
{
  print_src_location(&data->src);
  panic("Unreachable code reached");
}
