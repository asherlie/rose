#include <stdio.h>
#include <regex.h>
#include <sys/types.h>

#include <vmem_access.h>

bool strtoi(const char* str, int* i){
      char* res;
      unsigned int r = (unsigned int)strtol(str, &res, 10);
      if(*res)return false;
      if(i)*i = (int)r;
      return true;
}

int main(int argc, char** argv){
      pid_t pid;
      if(argc < 3 || !strtoi(argv[1], &pid)){
            puts("enter: pid regex");
            return 1;
      }
      regex_t reg;
      regcomp(&reg, argv[2], 0);
      struct mem_map m;
      mem_map_init(&m, pid, true);
      populate_mem_map(&m, BOTH, true, false, 4);
      for(unsigned int i = 0; i < m.size; ++i)
            if(!regexec(&reg, m.s_mmap[i].value, 0, NULL, 0))
                  printf("(%5s @ %p): \"%s\"\n", which_rgn(m.mapped_rgn, m.s_mmap[i].addr, NULL), m.s_mmap[i].addr, m.s_mmap[i].value);
      free_mem_rgn(&m.mapped_rgn);
      free_mem_map(&m);
}
