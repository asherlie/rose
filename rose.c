#include <stdio.h>
#include <regex.h>
#include <sys/types.h>
#include <string.h>

#include <vmem_access.h>

#define ROSE_VER "1.1.0"

bool strtoi(const char* str, int* i){
      char* res;
      int r = (int)strtol(str, &res, 10);
      if(*res)return false;
      *i = r;
      return true;
}

int main(int argc, char* argv[]){
      pid_t pid = 0;
      char* rstr = NULL;
      bool ints = false, num = false, reg_set = false;
      for(int i = 1; i < argc; ++i){
            if(*argv[i] == '-' && argv[i][1]){
                  switch(argv[i][1]){
                        case 'i': ints = true; continue;
                        case 'v': printf("rose %s using %s\n", ROSE_VER, MEMCARVE_VER); return 0;
                        case 'n': num = true; continue;
                        case 'h': continue;
                  }
            }
            if(!pid && strtoi(argv[i], &pid))continue;
            if(!reg_set){
                  rstr = argv[i];
                  reg_set = true;
            }
      }
      if(!pid || !reg_set){
            puts("enter: pid regex");
            return 1;
      }
      regex_t reg;
      regcomp(&reg, rstr, 0);
      struct mem_map m;
      mem_map_init(&m, pid, false);
      populate_mem_map(&m, BOTH, true, ints, 4);
      char int_buf[12];
      char* cmp_str = NULL;
      void* addr;
      unsigned int n = 0;
      for(unsigned int i = 0; i < m.size; ++i){
            if(ints){
                  snprintf(int_buf, 11, "%i", m.i_mmap[i].value);
                  cmp_str = int_buf;
            }
            else cmp_str = m.s_mmap[i].value;
            addr = (ints) ? m.i_mmap[i].addr : m.s_mmap[i].addr;
            if(!regexec(&reg, cmp_str, 0, NULL, 0)){
                  if(num)printf("%i ", n);
                  printf("(%5s @ %p): ", which_rgn(m.mapped_rgn, addr, NULL), addr);
                  (ints) ? printf("%i\n", m.i_mmap[i].value) : printf("\"%s\"\n", cmp_str);
                  ++n;
            }
      }
      printf("%i matches found\n", n);
      free_mem_rgn(&m.mapped_rgn);
      free_mem_map(&m);
      regfree(&reg);
}
