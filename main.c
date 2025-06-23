#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

#define SV_IMPLEMENTATION
#include "./sv.h"

typedef struct
{
  char data[32];
} Word;

Word sv_as_word(String_View sv)
{
  Word word = {0};
  assert(sv.count < sizeof(word.data));
  memcpy(word.data, sv.data, sv.count);
  return word;
}

Word norm_word(Word word)
{
  Word result = {0};
  char *in = word.data;
  char *out = result.data;

  while (*in)
  {
    char c = *in++;
    if (isalnum(c))
    {
      *out++ = toupper(c);
    }
  }
  return result;
}

// TODO: implement lru cache mechanism
size_t word_count(String_View content, Word needle)
{

  size_t count = 0;
  while (content.count > 0)
  {
    String_View line = sv_chop_by_delim(&content, '\n');
    while (line.count > 0)
    {
      String_View word_sv = sv_trim((sv_chop_by_delim(&line, ' ')));
      if (word_sv.count > 0)
      {
        Word word = norm_word(sv_as_word(word_sv));
        if (strcmp(word.data, needle.data) == 0)
        {
          count++;
        }
      }
    }
  }

  return count;
}

int main(int argc, char **argv)
{

  if (argc < 2)
  {
    fprintf(stderr, "Usage: ./lru <input.txt>\n");
    fprintf(stderr, "ERROR:input file is not provided\n");
    exit(1);
  }

  const char *file_path = argv[1];

  int fd = open(file_path, O_RDONLY);

  if (fd < 0)
  {
    fprintf(stderr, "ERROR: couldn't open the file %s: %s\n", file_path, strerror(errno));
    exit(1);
  }

  struct stat statbuf = {0};

  if (fstat(fd, &statbuf) < 0)
  {
    fprintf(stderr, "ERROR: couldn't open the file %s: %s\n", file_path, strerror(errno));
    exit(1);
  }

  size_t content_size = statbuf.st_size;

  char *content_data = mmap(NULL, content_size, PROT_READ, MAP_PRIVATE, fd, 0);

  if (content_data == NULL)
  {
    fprintf(stderr, "ERROR: couldn't map file %s: %s\n", file_path, strerror(errno));
    exit(1);
  }

  String_View content = sv_from_parts(content_data, content_size);
  while (content.count > 0)
  {
    String_View line = sv_chop_by_delim(&content, '\n');
    while (line.count > 0)
    {
      String_View word_origin = sv_trim(sv_chop_by_delim(&line, ' '));
      if (word_origin.count > 0)
      {
        Word needle = norm_word(sv_as_word(word_origin));
        size_t freq = word_count(sv_from_parts(content_data, content_size), needle);
        printf(SV_Fmt "(%zu) ", SV_Arg(word_origin), freq);
      }
    }
    printf("\n");
  }

  munmap(content_data, content_size);
  close(fd);

  return 0;
}
