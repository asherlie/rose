#include <stdio.h>
#include <regex.h>
#include <sys/types.h>
#include <string.h>

#include <vmem_access.h>

#define ROSE_VER "1.0.0"

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
      bool ints = false, reg_set = false;
      for(int i = 1; i < argc; ++i){
            if(!strncmp(argv[i], "-i", 3)){
                  ints = true;
                  continue;
            }
            if(!strncmp(argv[i], "-v", 3)){
                  printf("rose %s using %s\n", ROSE_VER, MEMCARVE_VER);
                  return 0;
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
      populate_mem_map(&m, BOTH, true, false, 4);
      for(unsigned int i = 0; i < m.size; ++i)
            if(!regexec(&reg, m.s_mmap[i].value, 0, NULL, 0))
                  printf("(%5s @ %p): \"%s\"\n", which_rgn(m.mapped_rgn, m.s_mmap[i].addr, NULL), m.s_mmap[i].addr, m.s_mmap[i].value);
      free_mem_rgn(&m.mapped_rgn);
      free_mem_map(&m);
      regfree(&reg);
}
