#include <stdio.h>
#include <regex.h>
#include <sys/types.h>

#include <vmem_access.h>

bool strtoi(const char* str, unsigned int* ui, int* i){
      char* res;
      unsigned int r = (unsigned int)strtol(str, &res, 10);
      if(*res)return false;
      if(i)*i = (int)r;
      if(ui)*ui = r;
      return true;
}

int main(int argc, char** argv){
      pid_t pid;
      if(argc < 3 || !strtoi(argv[1], NULL, &pid)){
            puts("enter: pid regex");
            return 1;
      }
      regex_t reg;
      regcomp(&reg, argv[2], 0);
      struct mem_map m;
      mem_map_init(&m, pid, true);
      set_mode_mem_map(&m, false);
      populate_mem_map(&m, BOTH, true, false, 4);
      int rgn;
      for(unsigned int i = 0; i < m.size; ++i){
            if(!regexec(&reg, m.s_mmap[i].value, 0, NULL, 0))
                  printf("(%s @ %p): \"%s\"\n", which_rgn(m.mapped_rgn, m.s_mmap[i].addr, &rgn), m.s_mmap[i].addr, m.s_mmap[i].value);
      }
      free_mem_rgn(&m.mapped_rgn);
      free_mem_map(&m);
}
