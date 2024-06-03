// x86 codegen

#include "exe.c"
#include "elf.c"

#ifdef x86_64
const int word_size = 8; // generating for x86-64
#else
const int word_size = 4; // generating for i386
#endif

// Registers common to i386 and x86-64 (E and R prefixes are omitted).

const int AX = 0;
const int CX = 1;
const int DX = 2;
const int BX = 3;
const int SP = 4;
const int BP = 5;
const int SI = 6;
const int DI = 7;

const int reg_X = AX;
const int reg_Y = CX;
const int reg_SP = SP;
const int reg_glo = BX;

void rex_prefix() {
  if (word_size == 8) emit_i8(0x48); // REX.W
}

void mod_rm(int reg1, int reg2) {
  emit_i8(0xc0 + 8*reg1 + reg2); // ModR/M
}

void op_reg_reg(int opcode, int dst, int src) {
  rex_prefix();
  emit_i8(opcode);
  mod_rm(src, dst);
}

#ifdef SKIP

// deprecated... kept here in case it might be useful in the future

void inc_reg(int dst) { rex_prefix(); emit_2_i8(0xff, 0xc0 + dst); }
void dec_reg(int dst) { rex_prefix(); emit_2_i8(0xff, 0xc8 + dst); }
void xchg_reg_reg(int dst, int src) { op_reg_reg(0x87, dst, src); }
void not_reg(int dst) { rex_prefix(); emit_2_i8(0xf7, 0xd0 + dst); }
void neg_reg(int dst) { rex_prefix(); emit_2_i8(0xf7, 0xd8 + dst); }
void shr_reg_cl(int dst) { rex_prefix(); emit_2_i8(0xd3, 0xe8 + dst); }
void jump_cond_short(int cond, int n) { emit_2_i8(0x70 + cond, n); }

void test_reg_reg(int dst, int src) {

  // TEST dst_reg, src_reg ;; set Z condition flag based on result of dst_reg&src_reg
  // See: https://web.archive.org/web/20231004142335/https://www.felixcloutier.com/x86/test

  op_reg_reg(0x85, dst, src);
}

#endif

void add_reg_reg(int dst, int src) {

  // ADD dst_reg, src_reg ;; dst_reg = dst_reg + src_reg
  // See: https://web.archive.org/web/20240407051903/https://www.felixcloutier.com/x86/add

  op_reg_reg(0x01, dst, src);
}

void or_reg_reg (int dst, int src) {

  // OR dst_reg, src_reg ;; dst_reg = dst_reg | src_reg
  // See: https://web.archive.org/web/20231002205127/https://www.felixcloutier.com/x86/or

  op_reg_reg(0x09, dst, src);
}

void and_reg_reg(int dst, int src) {

  // AND dst_reg, src_reg ;; dst_reg = dst_reg & src_reg
  // See: https://web.archive.org/web/20240228122102/https://www.felixcloutier.com/x86/and

  op_reg_reg(0x21, dst, src);
}

void sub_reg_reg(int dst, int src) {

  // SUB dst_reg, src_reg ;; dst_reg = dst_reg - src_reg
  // See: https://web.archive.org/web/20240118202232/https://www.felixcloutier.com/x86/sub

  op_reg_reg(0x29, dst, src);
}

void xor_reg_reg(int dst, int src) {

  // XOR dst_reg, src_reg ;; dst_reg = dst_reg ^ src_reg
  // See: https://web.archive.org/web/20240323052259/https://www.felixcloutier.com/x86/xor

  op_reg_reg(0x31, dst, src);
}

void cmp_reg_reg(int dst, int src) {

  // CMP dst_reg, src_reg  ;; Set condition flags according to dst_reg-src_reg
  // See: https://web.archive.org/web/20240407051947/https://www.felixcloutier.com/x86/cmp

  op_reg_reg(0x39, dst, src);
}

void mov_reg_reg(int dst, int src) {

  // MOV dst_reg, src_reg  ;; dst_reg = src_reg
  // See: https://web.archive.org/web/20240407051903/https://www.felixcloutier.com/x86/mov

  op_reg_reg(0x89, dst, src);
}

void mov_reg_imm(int dst, int imm) {

  // MOV dst_reg, imm  ;; Move 32 bit immediate value to register
  // See: https://web.archive.org/web/20240407051903/https://www.felixcloutier.com/x86/mov

  rex_prefix();
  emit_i8(0xb8 + dst);
  emit_word_le(imm);
}

void add_reg_imm(int dst, int imm) {

  // ADD dst_reg, imm  ;; Add 32 bit immediate value to register
  // See: https://web.archive.org/web/20240407051903/https://www.felixcloutier.com/x86/add
  rex_prefix();
  emit_i8(0x81);
  mod_rm(0, dst);
  emit_i32_le(imm);
}

void mov_memory(int op, int reg, int base, int offset) {

  // Move word between register and memory
  // See: https://web.archive.org/web/20240407051903/https://www.felixcloutier.com/x86/mov

  rex_prefix();
  emit_i8(op);
  emit_i8(0x80 + reg * 8 + base);
  if (base == SP) emit_i8(0x24);
  emit_i32_le(offset);
}

void mov_mem_reg(int base, int offset, int src) {

  // MOV [base_reg + offset], src_reg  ;; Move word from register to memory
  // See: https://web.archive.org/web/20240407051903/https://www.felixcloutier.com/x86/mov

  mov_memory(0x89, src, base, offset);
}

void mov_mem8_reg(int base, int offset, int src) {

  // MOVB [base_reg + offset], src_reg  ;; Move byte from register to memory
  // See: https://web.archive.org/web/20240407051903/https://www.felixcloutier.com/x86/mov

  mov_memory(0x88, src, base, offset);
}

void mov_reg_mem(int dst, int base, int offset) {

  // MOV dst_reg, [base_reg + offset]  ;; Move word from memory to register
  // See: https://web.archive.org/web/20240407051903/https://www.felixcloutier.com/x86/mov

  mov_memory(0x8b, dst, base, offset);
}

void mov_reg_mem8(int dst, int base, int offset) {

  // MOVB dst_reg, [base_reg + offset]  ;; Move byte from memory to register
  // See: https://web.archive.org/web/20240407051903/https://www.felixcloutier.com/x86/mov

  mov_memory(0x8a, dst, base, offset);
  mov_reg_imm(DI, 0xff); // mask off the upper bits
  and_reg_reg(dst, DI);
}

void imul_reg_reg(int dst, int src) {

  // IMUL dst_reg, src_reg ;; dst_reg = dst_reg * src_reg
  // See: https://web.archive.org/web/20240228122220/https://www.felixcloutier.com/x86/imul

  rex_prefix();
  emit_2_i8(0x0f, 0xaf);
  mod_rm(dst, src);
}

void mul_reg_reg(int dst, int src) {
  imul_reg_reg(dst, src);
}

void idiv_reg(int src) {

  // IDIV src_reg ;; AX = DX:AX / src_reg ; DX = DX:AX % src_reg
  // See: https://web.archive.org/web/20240407195950/https://www.felixcloutier.com/x86/idiv

  rex_prefix();
  emit_2_i8(0xf7, 0xf8 + src);
}

void cdq_cqo() {

  // CDQ ;; Convert Doubleword to Quadword (EDX:EAX = sign-extend EAX)
  // x86-64: CQO ;; Convert Quadword to Octoword (RDX:RAX = sign-extend RAX)
  // See: https://web.archive.org/web/20240118201956/https://www.felixcloutier.com/x86/cwd:cdq:cqo

  rex_prefix();
  emit_i8(0x99);
}

void div_reg_reg(int dst, int src) {

  // Computes dst_reg = dst_reg / src_reg
  // This is not an actual instruction on x86. The operation
  // is emulated with a sequence of instructions that will clobber the
  // registers AX and DX.

  mov_reg_reg(AX, dst);
  cdq_cqo(); // sign extend AX to DX:AX
  idiv_reg(src);
  mov_reg_reg(dst, AX);
}

void rem_reg_reg(int dst, int src) {

  // Computes dst_reg = dst_reg % src_reg
  // This is not an actual instruction on x86. The operation
  // is emulated with a sequence of instructions that will clobber the
  // registers AX and DX.

  mov_reg_reg(AX, dst);
  cdq_cqo(); // sign extend AX to DX:AX
  idiv_reg(src);
  mov_reg_reg(dst, DX);
}

void shl_reg_cl(int dst) {

  // SHL dst_reg, cl ;; dst_reg = dst_reg << cl
  // See: https://web.archive.org/web/20240405194323/https://www.felixcloutier.com/x86/sal:sar:shl:shr

  rex_prefix();
  emit_2_i8(0xd3, 0xe0 + dst);
}

void shl_reg_reg(int dst, int src) {

  // Computes dst_reg = dst_reg << src_reg
  // This is not an actual instruction on x86. The operation
  // is emulated with a sequence of instructions that clobbers the
  // register CX, and does not work if dst = CX.

  mov_reg_reg(CX, src);
  shl_reg_cl(dst);
}

void sar_reg_cl(int dst) {

  // SAR dst_reg, cl ;; dst_reg = dst_reg >> cl
  // See: https://web.archive.org/web/20240405194323/https://www.felixcloutier.com/x86/sal:sar:shl:shr

  rex_prefix();
  emit_2_i8(0xd3, 0xf8 + dst);
}

void sar_reg_reg(int dst, int src) {

  // Computes dst_reg = dst_reg >> src_reg
  // This is not an actual instruction on x86. The operation
  // is emulated with a sequence of instructions that clobbers the
  // register CX, and does not work if dst = CX.

  mov_reg_reg(CX, src);
  sar_reg_cl(dst);
}

void push_reg(int src) {

  // PUSH src_reg  ;; Push word from register to stack
  // See: https://web.archive.org/web/20240407051929/https://www.felixcloutier.com/x86/push

  emit_i8(0x50 + src);
}

void push_imm32_le(int imm) {

  // PUSH imm32  ;; Push 32 bit immediate value to stack
  // See: https://web.archive.org/web/20240407051929/https://www.felixcloutier.com/x86/push

  emit_i8(0x68);
  emit_i32_le(imm);
}


void pop_reg (int dst) {

  // POP dst_reg  ;; Pop word from stack to register
  // See: https://web.archive.org/web/20240204122208/https://www.felixcloutier.com/x86/pop

  emit_i8(0x58 + dst);
}

void jump(int lbl) {

  // JMP rel32  ;; Jump to 32 bit displacement relative to next instruction
  // See: https://web.archive.org/web/20240407051904/https://www.felixcloutier.com/x86/jmp

  emit_i8(0xe9);
  use_label(lbl);
}

void jump_rel(int offset) {

  // JMP rel32  ;; Jump to 32 bit displacement relative to next instruction
  // See: https://web.archive.org/web/20240407051904/https://www.felixcloutier.com/x86/jmp

  emit_i8(0xe9);
  emit_i32_le(offset);
}

void call(int lbl) {

  // CALL rel32  ;; Call to 32 bit displacement relative to next instruction
  // See: https://web.archive.org/web/20240323052931/https://www.felixcloutier.com/x86/call

  emit_i8(0xe8);
  use_label(lbl);
}

void ret() {

  // RET  ;; Return to calling procedure
  // See: https://web.archive.org/web/20240302232015/https://www.felixcloutier.com/x86/ret

  emit_i8(0xc3);
}

// Conditions for use by jump_cond:

const int EQ = 0x4; // x == y
const int NE = 0x5; // x != y
const int LT = 0xc; // x < y
const int GE = 0xd; // x >= y
const int LE = 0xe; // x <= y
const int GT = 0xf; // x > y

void jump_cond(int cond, int lbl) {

  // JE rel32, JNE rel32, JL rel32, JLE rel32, JG rel32, JGE rel32, ...
  // Jump conditionally to 32 bit displacement relative to next instruction
  // See: https://web.archive.org/web/20231007122614/https://www.felixcloutier.com/x86/jcc

  emit_2_i8(0x0f, 0x80 + cond);
  use_label(lbl);
}

void jump_cond_reg_reg(int cond, int lbl, int reg1, int reg2) {
  cmp_reg_reg(reg1, reg2);
  jump_cond(cond, lbl);
}

void int_i8(int n) {

  // INT imm8 ;; Software interrupt with vector specified by immediate byte
  // See: https://web.archive.org/web/20240407051903/https://www.felixcloutier.com/x86/intn:into:int3:int1

  emit_2_i8(0xcd, n);
}

void setup_proc_args(int global_vars_size) {
  // See page 54 of Intel386 System V ABI document:
  // https://web.archive.org/web/20240107061436/https://www.sco.com/developers/devspecs/abi386-4.pdf
  // See page 29 of AMD64 System V ABI document:
  // https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf
  //
  // On x86-32, argc is at [esp+0] and the content of argv directly follows at program start.
  // At this point, we've initialized the globals table so the stack looks like this:
  // [esp + 0]: global table start (global_vars_size bytes long)
  // ...
  // [esp + global_vars_size]     : argc
  // [esp + global_vars_size + 4] : argv[0]
  // [esp + global_vars_size + 8] : argv[1]
  // ...
  // The main function expects argv to be a char**, so it's missing an indirection, which is added here.
  // The stack will then look like this:
  // [esp + 0] : argc
  // [esp + 4] : argv
  // [esp + 8] : global table start (global_vars_size bytes long)
  // ...
  // For x86-64, it works similarly with [rsp + 0] for argc and [rsp + 8] for argv.

  mov_reg_reg(reg_X, SP);
  add_reg_imm(reg_X, global_vars_size + word_size); // compute address of argv
  push_reg(reg_X); // push argv address

  mov_reg_mem(reg_Y, reg_X, -word_size); // load argc
  push_reg(reg_Y); // push argc
}

// For 32 bit linux.
#ifdef i386

void os_getchar() {
  int lbl = alloc_label();
  push_reg(BX);           // save address of global variables table
  mov_reg_imm(AX, 0);     // mov  eax, 0
  push_reg(AX);           // push eax      # buffer to read byte
  mov_reg_imm(BX, 0);     // mov  ebx, 0   # ebx = 0 = STDIN
  mov_reg_imm(DX, 1);     // mov  edx, 1   # edx = 1 = number of bytes to read
  mov_reg_reg(CX, SP);    // mov  ecx, esp # to the stack
  mov_reg_imm(AX, 3);     // mov  eax, 3   # SYS_READ
  int_i8(0x80);           // int  0x80     # system call
  xor_reg_reg(DX, DX);    // edx = 0
  cmp_reg_reg(AX, DX);    // cmp  eax, edx
  pop_reg(AX);            // pop  eax
  jump_cond(NE, lbl);     // jne  lbl      # if byte was read don't return EOF
  mov_reg_imm(AX, -1);    // mov  eax, -1  # -1 on EOF
  def_label(lbl);         // end label
  pop_reg(BX);            // restore address of global variables table
}

void os_putchar() {
  push_reg(BX);           // save address of global variables table
  push_reg(AX);           // push eax      # buffer to write byte
  mov_reg_imm(BX, 1);     // mov  ebx, 1   # ebx = 1 = STDOUT
  mov_reg_imm(DX, 1);     // mov  edx, 1   # edx = 1 = number of bytes to write
  mov_reg_reg(CX, SP);    // mov  ecx, esp # from the stack
  mov_reg_imm(AX, 4);     // mov  eax, 4   # SYS_WRITE
  int_i8(0x80);           // int  0x80     # system call
  pop_reg(AX);            // pop  eax
  pop_reg(BX);            // restore address of global variables table
}

void os_exit() {
  push_reg(BX);           // save address of global variables table
  mov_reg_reg(BX, AX);    // mov  ebx, eax
  mov_reg_imm(AX, 1);     // mov  eax, 1   # SYS_EXIT
  int_i8(0x80);           // int  0x80     # system call
  pop_reg(BX);            // restore address of global variables table
}

void os_fopen() {
  push_reg(BX);           // save address of global variables table
  mov_reg_reg(BX, AX);    // mov ebx, ax | file name
  mov_reg_imm(AX, 5);     // mov eax, 5 == SYS_OPEN
  mov_reg_imm(CX, 0);     // mov ecx, 0 | flags
  mov_reg_imm(DX, 0);     // mov edx, 0 | mode
  int_i8(0x80);           // system call
  pop_reg(BX);            // restore address of global variables table
}

void os_fclose() {
  push_reg(BX);           // save address of global variables table
  mov_reg_reg(BX, reg_X); // mov  ebx, file descriptor
  mov_reg_imm(AX, 6);     // mov  eax, 6 == SYS_CLOSE
  int_i8(0x80);           // system call
  pop_reg(BX);            // restore address of global variables table
}

void os_fgetc() {
  int lbl = alloc_label();  // label for EOF
  push_reg(BX);             // save address of global variables table
  mov_reg_reg(BX, reg_X);   // mov  ebx, file descriptor
  mov_reg_imm(AX, 3);       // mov  eax, 3 == SYS_READ
  push_reg(AX);             // push eax      # buffer to read byte
  mov_reg_imm(DX, 1);       // mov  edx, 1   # edx = 1 = number of bytes to read
  mov_reg_reg(CX, SP);      // mov  ecx, esp # to the stack
  int_i8(0x80);             // int  0x80     # system call
  xor_reg_reg(BX, BX);      // xor  ebx, ebx
  cmp_reg_reg(AX, BX);      // cmp  eax, ebx
  pop_reg(AX);              // pop  eax
  jump_cond(NE, lbl);       // jne  lbl      # if byte was read don't return EOF
  mov_reg_imm(AX, -1);      // mov  eax, -1  # -1 on EOF
  def_label(lbl);           // end label
  pop_reg(BX);              // restore address of global variables table
}

#endif

// For 64 bit linux.
#ifdef x86_64

void os_getchar() {
  int lbl = alloc_label();
  mov_reg_imm(AX, 0);    // mov  eax, 0
  push_reg(AX);          // push eax      # buffer to read byte
  mov_reg_imm(DI, 0);    // mov  edi, 0   # edi = 0 = STDIN
  mov_reg_imm(DX, 1);    // mov  rdx, 1   # rdx = 1 = number of bytes to read
  mov_reg_reg(SI, SP);   // mov  rsi, rsp # to the stack
  mov_reg_imm(AX, 0);    // mov  rax, 1   # SYS_READ
  emit_2_i8(0x0F, 0x05); // system call 64 bit
  xor_reg_reg(DX, DX);   // rdx = 0
  cmp_reg_reg(AX, DX);   // cmp  eax, ebx
  pop_reg(AX);           // pop  eax
  jump_cond(NE, lbl);    // jne  lbl      # if byte was read don't return EOF
  mov_reg_imm(AX, -1);   // mov  eax, -1  # -1 on EOF
  def_label(lbl);        // end label
}

void os_putchar() {
  push_reg(AX);          // push rax      # buffer to write byte
  mov_reg_imm(AX, 1);    // mov eax, 1 == SYS_WRITE
  mov_reg_imm(DI, 1);    // mov edi, 1 == STDOUT
  mov_reg_imm(DX, 1);    // mov edx, 1 | length of string
  mov_reg_reg(SI, SP);   // mov esi, [esp] | get the value from stack
  emit_2_i8(0x0F, 0x05); // system call 64 bit
  pop_reg(AX);           // pop rax
}

void os_exit() {
  mov_reg_reg(DI, AX);    // mov edi, eax
  mov_reg_imm(AX, 60);    // mov eax, 60 == SYS_EXIT
  emit_2_i8(0x0F, 0x05);  // system call 64 bit
}

void os_fopen(){
  mov_reg_reg(DI, AX);    // mov rdi, rax | file name
  mov_reg_imm(SI, 0);     // mov rsi, 0 | flags
  mov_reg_imm(DX, 0);     // mov rdx, 0 | mode
  mov_reg_imm(AX, 2);     // mov rax, 2 == SYS_OPEN
  emit_2_i8(0x0F, 0x05);  // system call 64 bit (file descriptor is in rax)
}

void os_fclose(){
  mov_reg_reg(DI, reg_X); // mov  rdi, file descriptor
  mov_reg_imm(AX, 3);     // mov rax, 3 == SYS_CLOSE
  emit_2_i8(0x0F, 0x05);  // system call 64 bit
}

void os_fgetc(){
  int lbl = alloc_label(); // label for EOF
  mov_reg_reg(DI, reg_X);  // mov  edi, file descriptor
  mov_reg_imm(AX, 0);      // mov  eax, 0
  push_reg(AX);            // push eax      # buffer to read byte
  mov_reg_imm(DX, 1);      // mov  rdx, 1   # rdx = 1 = number of bytes to read
  mov_reg_reg(SI, SP);     // mov  rsi, rsp # to the stack
  mov_reg_imm(AX, 0);      // mov  rax, 1   # SYS_READ
  emit_2_i8(0x0F, 0x05);   // system call 64 bit
  xor_reg_reg(DX, DX);     // rdx = 0
  cmp_reg_reg(AX, DX);     // cmp  eax, rdx
  pop_reg(AX);             // pop  eax
  jump_cond(NE, lbl);      // jne  lbl      # if byte was read don't return EOF
  mov_reg_imm(AX, -1);     // mov  eax, -1  # -1 on EOF
  def_label(lbl);          // end label
}

#endif

#ifdef SKIP
void os_print_msg(char_ptr msg) {
  int i = 0;
  while (msg[i] != 0) {
    mov_reg_imm(AX, msg[i]);  // mov  eax, c
    os_putchar();             // putchar
    i += 1;
  }
}

#endif