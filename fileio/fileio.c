/*
 *  fileio - a simple file I/O sample
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>

__attribute__((constructor))
void ctor(void)
{
	printf("** constructor **\n");
}

__attribute__((destructor))
void dtor(void)
{
	printf("** destructor **\n");
}

int main(int argc, char **argv)
{
  FILE *fp;
  int size;
  int i;
  int x;
  char line[100];

  printf("=== file I/O test ===\n");

  printf("\n** file open/seek/close **\n");
  fp = fopen("fileio.x", "r");
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  printf("fileio.x size = %dbytes\r\n", size);
  fclose(fp);

  printf("\n** write file (text/binary) **\n");

  fp = fopen("testfile", "w");
  for (i = 0; i < 3; i++) {
    fprintf(fp, "line %d\n", i);
  }
  fclose(fp);

  fp = fopen("testfile2", "wb");
  for (i = 0; i < 3; i++) {
    fprintf(fp, "line %d\n", i);
  }
  fclose(fp);

  printf("\n** read text mode file **\n");
  fp = fopen("testfile", "r");
  while (fgets(line, sizeof(line), fp) != NULL) {
    for (x = 0; x < strlen(line); x++)
      printf("0x%02x ", line[x]);
    printf("\n");
  }
  fclose(fp);

  printf("\n** read text mode file with binary mode **\n");
  fp = fopen("testfile", "rb");
  while (fgets(line, sizeof(line), fp) != NULL) {
    for (x = 0; x < strlen(line); x++)
      printf("0x%02x ", line[x]);
    printf("\n");
  }
  fclose(fp);

  printf("\n** read binary mode file with binary mode **\n");
  fp = fopen("testfile2", "rb");
  while (fgets(line, sizeof(line), fp) != NULL) {
    for (x = 0; x < strlen(line); x++)
      printf("0x%02x ", line[x]);
    printf("\n");
  }
  fclose(fp);

  unlink("testfile");
  unlink("testfile2");

  DIR *dir;
  struct dirent *d;

  printf("\n** opendir/readdir/closedir **\n");

  dir = opendir("/");
  while (1) {
    long p = telldir(dir);
    if (!(d = readdir(dir)))
      break;
	  printf("pos=%d off=%d reclen=%d type=%d name=%s\n", p, d->d_off, d->d_reclen, d->d_type, d->d_name);
  }
  rewinddir(dir);
  d = readdir(dir);
  printf("first entry: %s\n", d->d_name);

  closedir(dir);

  printf("=== end ===\n");

  return 0;
}
