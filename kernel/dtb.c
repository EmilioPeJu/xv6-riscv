#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "dtb.h"

#define U32B2L(num) ( \
    ((num) & 0x000000ff) << 24u | \
    ((num) & 0x0000ff00) << 8u  | \
    ((num) & 0x00ff0000) >> 8u  | \
    ((num) & 0xff000000) >> 24u)
#define FDT_U32(num) U32B2L(*(uint32 *) (num))

#define TOKEN_FDT_END 0x09000000
#define TOKEN_FDT_BEGIN_NODE 0x01000000
#define TOKEN_FDT_END_NODE 0x02000000
#define TOKEN_FDT_PROP 0x03000000

#define MAX_DEPTH 6

uint64 hw_mem_start = 0;
uint64 hw_mem_size = 0;

struct dtb_it_data {
    uint32 n_size_cells;
    uint32 n_address_cells;
    uint64 memory_start;
    uint64 memory_size;
};

typedef void (*dtb_it_callback)(
    struct dtb_it_data *data, char **nodes_stack,
    int level, char *prop_name, char *prop_val);

extern char _binary_dt_hw_dtb_start;

static int
is_memory_node(char *name)
{
    return memcmp(name, "memory", 6) == 0 && (name[6] == '@' || name[6] == 0);
}

static int
is_n_address_cells(char *name)
{
    return memcmp(name, "#address-cells\0", 15) == 0;
}

static int
is_n_size_cells(char *name)
{
    return memcmp(name, "#size-cells\0", 12) == 0;
}

static int
is_reg_prop(char *name)
{
    return memcmp(name, "reg\0", 4) == 0;
}

// this function is run before memory is setup, so, we iterate the dtb in
// a primitive way, later, dtb can be processed into a more friendly form
// to obtain data
static int
dtb_read_memory_info(uint64 *addr, uint64 *size)
{
    char *ptr = (char *) &_binary_dt_hw_dtb_start;
    uint32 struct_off = FDT_U32(ptr + 8);
    uint32 string_off = FDT_U32(ptr + 12);
    char *strings = ptr + string_off;
    ptr += struct_off;
    int level = 0;
    int in_memory = 0;
    uint32 len = 0;
    uint32 n_address_cells = 1;
    uint32 n_size_cells = 1;
    uint32 name_off = 0;
    int got_it = 0;
    for (;;) {
          switch (*(uint32 *) ptr) {
              case TOKEN_FDT_END:
                  return got_it;
                  break;
              case TOKEN_FDT_BEGIN_NODE:
                  level++;
                  ptr += 4;
                  if (is_memory_node(ptr) && level == 2) {
                      in_memory = 1;
                  }
                  while (ptr[3]) ptr += 4;
                  break;
              case TOKEN_FDT_END_NODE:
                  level--;
                  in_memory = 0;
                  ptr += 4;
                  break;
              case TOKEN_FDT_PROP:
                  len = FDT_U32(&ptr[4]);
                  name_off = FDT_U32(&ptr[8]);
                  char *name = strings + name_off;
                  if (in_memory && is_reg_prop(name)) {
                      uint64 tmp = 0;
                      for (int i=0; i < n_address_cells; i++)
                          tmp = (tmp << 32) | FDT_U32(ptr + 12 + i * 4);

                      *addr = tmp;
                      tmp = 0;
                      for (int i=0; i < n_size_cells; i++)
                          tmp = (tmp << 32) | FDT_U32(
                              ptr + 12 + (n_address_cells + i) * 4);

                      *size = tmp;
                      got_it = 1;
                  } else if (is_n_size_cells(name) && level == 1) {
                      n_size_cells = FDT_U32(ptr + 12);
                      n_size_cells = n_size_cells > 2 ? 2 : n_size_cells;
                  } else if (is_n_address_cells(name) && level == 1) {
                      n_address_cells = FDT_U32(ptr + 12);
                      n_address_cells = n_address_cells > 2 ? 2 : n_address_cells;
                  }
                  len = (len + 3) & 0xfffffffc;
                  ptr += (len + 4 + 8);
                  break;
              default:
                  ptr += 4;
          }

    }
    return got_it;
}

void
dtb_early_process()
{
    char *ptr = &_binary_dt_hw_dtb_start;
    printf("DTB size = %d bytes\n", (int) FDT_U32(ptr + 4));
    if (!dtb_read_memory_info(&hw_mem_start, &hw_mem_size)) {
        panic("Can't get memory information");
    }
    printf("Memory start = %p, size = %p bytes \n", hw_mem_start, hw_mem_size);
}
