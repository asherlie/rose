#include <stdio.h>
#include <regex.h>
#include <sys/types.h>
#include <string.h>

#include <vmem_access.h>

#define ROSE_VER "1.2.1"

bool strtoi(const char* str, int* i){
      char* res;
      int r = (int)strtol(str, &res, 10);
      if(*res)return false;
      *i = r;
      return true;
}

void p_help(char* name){
      printf("usage: %s <pid> <regex> {[-S] [-H] [-A] [-i] [-v] [-n] [-h]}\n"
            "    -S : stack only mode\n    -H : heap only mode\n    -A : use unmarked"
            " memory in stack/heap only mode\n    -i : search "
             "through integers\n    -v : print version\n    -n : number each "
             "match as it's printed\n    -h : print this help\n", name);
}

int main(int argc, char* argv[]){
      pid_t pid = 0;
      char* rstr = NULL;
      /* adtnl is only used if stack or heap is set manually */
      bool ints = false, num = false, reg_set = false, adtnl = false;
      int rgn = BOTH;
      for(int i = 1; i < argc; ++i){
            if(*argv[i] == '-' && argv[i][1]){
                  switch(argv[i][1]){
                        case 'S': rgn = STACK; continue;
                        case 'H': rgn = HEAP; continue;
                        case 'A': adtnl = true; continue;
                        case 'i': ints = true; continue;
                        case 'v': printf("rose %s using %s\n", ROSE_VER, MEMCARVE_VER); return 0;
                        case 'n': num = true; continue;
                        case 'h': p_help(*argv); return 0;
                  }
            }
            if(!pid && strtoi(argv[i], &pid))continue;
            if(!reg_set){
                  rstr = argv[i];
                  reg_set = true;
            }
      }
      if(!pid || !reg_set){
            fprintf(stderr, "ENTER PID FOLLOWED BY REGEX\n");
            p_help(*argv);
            return 1;
      }
      regex_t reg;
      regcomp(&reg, rstr, 0);
      struct mem_map m;
      mem_map_init(&m, pid, false);
      /* if rgn == BOTH, rgn hasn't been set manually and we can ignore adtnl */
      populate_mem_map(&m, rgn, (rgn == BOTH || adtnl), ints, 4);

      /* TODO: iterate through i_mmap_hash struct for faster search */
      if(ints){
            flatten_i_mmap_hash(&m);
            regularize_i_mmap_hash(&m);
      }

      char int_buf[12];
      char* cmp_str = NULL;
      void* addr;
      unsigned int n = 0;
      for(unsigned int i = 0; i < m.size; ++i){
            if(ints){
                  snprintf(int_buf, 11, "%i", *m.i_mmap[i].value);
                  cmp_str = int_buf;
            }
            else cmp_str = m.s_mmap[i].value;
            addr = ints ? m.i_mmap[i].addr : m.s_mmap[i].addr;
            if(!regexec(&reg, cmp_str, 0, NULL, 0)){
                  if(num)printf("%i ", n);
                  printf("[%5s @ %p]: ", which_rgn(m.mapped_rgn, addr, NULL), addr);
                  ints ? printf("%i\n", *m.i_mmap[i].value) : printf("\"%s\"\n", cmp_str);
                  ++n;
            }
      }
      printf("%i matches found\n", n);
      free_mem_rgn(&m.mapped_rgn);
      free_mem_map(&m);
      regfree(&reg);
}
