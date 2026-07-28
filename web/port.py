import re, io, os

# Paths are relative to the repo root (this script lives in web/).
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
SRC = os.path.join(ROOT, "LostCult.c")
OUT = os.path.join(HERE, "LostCult_web.c")

with io.open(SRC, "r", encoding="utf-8", errors="replace") as f:
    txt = f.read()

# 1. Remove Windows-only headers
txt = txt.replace("#include <windows.h>\n", "")
txt = txt.replace("#include <conio.h>\n", "")

PRELUDE = r'''/* ============================================================
   Web / WebAssembly compatibility layer.
   This file is AUTO-GENERATED from LostCult.c for the browser
   build (Emscripten). Do not edit by hand: edit LostCult.c and
   re-run web/port.py. The game logic below is unchanged.
   ============================================================ */
#include <emscripten.h>

/* Blocking single keystroke, raw / no echo (replaces conio getch). */
EM_ASYNC_JS(int, em_input_getch, (void), {
  return await Module.inputGetch();
});

/* Blocking read of one character from line-buffered (echoed) stdin. */
EM_ASYNC_JS(int, em_input_getchar, (void), {
  return await Module.inputGetChar();
});

/* Push one character back onto the stdin stream. */
EM_JS(void, em_unget_char, (int c), {
  Module.inputUnget(c);
});

/* Emulate the handful of Windows console commands passed to system(). */
static void em_system(const char *cmd) {
  if (!cmd) return;
  if (strcmp(cmd, "cls") == 0) {
    printf("\x1b[2J\x1b[3J\x1b[H");
    fflush(stdout);
  } else if (strncmp(cmd, "color ", 6) == 0) {
    const char *c = cmd + 6;                 /* Windows: BG then FG nibble  */
    if      (strstr(c, "7F")) printf("\x1b[?5h");            /* flash / reverse screen */
    else if (strstr(c, "0F")) printf("\x1b[?5l\x1b[0m");    /* normal white on black  */
    else if (strstr(c, "4F")) printf("\x1b[?5l\x1b[41;97m");/* red background         */
    else if (strstr(c, "2F")) printf("\x1b[?5l\x1b[42;97m");/* green background       */
    fflush(stdout);
  }
}

/* scanf("%d") replacement: skip leading whitespace, read an integer,
   and leave the terminating character in the stream (matches libc so
   that the game's follow-up getchar() calls still consume the newline). */
static int em_read_int(void) {
  int c;
  do { c = em_input_getchar(); } while (c==' '||c=='\n'||c=='\r'||c=='\t');
  int neg = 0;
  if (c == '-') { neg = 1; c = em_input_getchar(); }
  else if (c == '+') { c = em_input_getchar(); }
  int val = 0;
  while (c >= '0' && c <= '9') { val = val*10 + (c-'0'); c = em_input_getchar(); }
  em_unget_char(c);
  return neg ? -val : val;
}

/* scanf("%s") replacement: skip leading whitespace, read a whitespace-
   delimited word, leave the terminating whitespace in the stream. */
static void em_read_word(char *buf, int max) {
  int c, i = 0;
  do { c = em_input_getchar(); } while (c==' '||c=='\n'||c=='\r'||c=='\t');
  while (c != -1 && c!=' ' && c!='\n' && c!='\r' && c!='\t') {
    if (i < max - 1) buf[i++] = (char)c;
    c = em_input_getchar();
  }
  buf[i] = '\0';
  em_unget_char(c);
}

/* Route the Windows-specific calls used throughout the game to the layer above. */
#define Sleep(ms)   emscripten_sleep((ms))
#define system(cmd) em_system((cmd))
#define getch()     em_input_getch()
#undef  getchar
#define getchar()   em_input_getchar()
/* ==================== end web compatibility layer ==================== */

'''

# 2. Insert prelude right after the <ctype.h> include (last of the system includes)
marker = "#include <ctype.h>\n"
idx = txt.index(marker) + len(marker)
txt = txt[:idx] + "\n" + PRELUDE + txt[idx:]

# 3. Replace scanf("%d", &X)  ->  X = em_read_int();
txt = re.sub(r'scanf\("%d",\s*&([^)]+?)\)\s*;',
             r'\1 = em_read_int();', txt)

# 4. Replace scanf("%s", (&)?X)  ->  em_read_word(X, sizeof(X));
txt = re.sub(r'scanf\("%s",\s*&?([A-Za-z_][A-Za-z0-9_]*)\)\s*;',
             r'em_read_word(\1, sizeof(\1));', txt)

# 5. Make stdout unbuffered so the char-by-char typing effect renders live.
main_sig = "int main(int argc, char const *argv[]) {"
assert main_sig in txt, "main signature not found"
txt = txt.replace(main_sig,
                  main_sig + "\n  setvbuf(stdout, NULL, _IONBF, 0); /* live output for the typing effect */",
                  1)

# sanity: no stray scanf left
leftover = re.findall(r'scanf\(', txt)
with io.open(OUT, "w", encoding="utf-8", newline="\n") as f:
    f.write(txt)

print("Wrote", OUT)
print("Remaining scanf(:", len(leftover))
