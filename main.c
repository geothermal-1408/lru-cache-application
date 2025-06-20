#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>

#define SV_IMPLEMENTATION
#include "./sv.h"

int main(int argc,char **argv){

  if(argc < 2){
    fprintf(stderr, "Usage: ./lru <input.txt>\n");
    fprintf(stderr,"ERROR:input file is not provided\n");
    exit(1);
  }

  const char *file_path = argv[1];

  int fd = open(file_path,O_RDONLY);
  
  if(fd < 0){ 
    fprintf(stderr, "ERROR: couldn't open the file %s: %s\n",file_path,strerror(errno));
    exit(1);
  }
  
  struct stat statbuf = {0};
  
  if(fstat(fd,&statbuf)< 0){ 
    fprintf(stderr, "ERROR: couldn't open the file %s: %s\n",file_path,strerror(errno));
    exit(1);
  }
  
  size_t content_size =  statbuf.st_size;
  
  char *content_data =  mmap(NULL, content_size, PROT_READ,MAP_PRIVATE, fd,0);
  
  if(content_data == NULL){
    fprintf(stderr, "ERROR: couldn't map file %s: %s\n", file_path,strerror(errno));
    exit(1);
  }

  String_View content = sv_from_parts(content_data, content_size);
  while(content.count > 0){
    String_View line = sv_chop_by_delim(&content, '\n');
    printf("(" SV_Fmt ")",SV_Arg(line));
  }
  munmap(content_data,content_size);
  close(fd);
  
  return 0;
}
