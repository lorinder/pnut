int main() {
  int f;
  char c;
  f = fopen("tests/_all/six-cc-tests/fgetc.c", "r");
  while ((c = fgetc(f)) != -1) {
    putchar(c);
  }
  fclose(f);
}
