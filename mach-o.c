// Minimal Mach-O header for 32-bit and 64-bit architectures.

// #ifdef __osx86__

// void write_mach_o_header_32() {
//   write_4_i8(0xce, 0xfa, 0xed, 0xfe); // magic number (MH_MAGIC)
//   write_4_i8(0x07, 0x00, 0x00, 0x00); // CPU type (CPU_TYPE_I386)
//   write_4_i8(0x03, 0x00, 0x00, 0x00); // CPU subtype (CPU_SUBTYPE_I386_ALL)
//   write_4_i8(0x02, 0x00, 0x00, 0x00); // file type (MH_EXECUTE)
//   write_4_i8(0x01, 0x00, 0x00, 0x00); // number of load commands
//   write_4_i8(0x28, 0x00, 0x00, 0x00); // size of all load commands
//   write_4_i8(0x00, 0x00, 0x00, 0x00); // flags
//   write_4_i8(0x00, 0x00, 0x00, 0x00); // reserved (only for 64-bit)
// }

// void write_mach_o_load_command_32() {
//   write_4_i8(0x01, 0x00, 0x00, 0x00); // LC_SEGMENT
//   write_4_i8(0x24, 0x00, 0x00, 0x00); // command size
//   write_str("TEXT");                  // segment name
//   write_4_i8(0x00, 0x00, 0x00, 0x00); // VM address
//   write_4_i8(0x54 + code_alloc, 0x00, 0x00, 0x00); // VM size
//   write_4_i8(0x00, 0x00, 0x00, 0x00); // file offset
//   write_4_i8(0x54 + code_alloc, 0x00, 0x00, 0x00); // file size
//   write_4_i8(0x07, 0x00, 0x00, 0x00); // max VM protection
//   write_4_i8(0x05, 0x00, 0x00, 0x00); // initial VM protection
//   write_4_i8(0x01, 0x00, 0x00, 0x00); // number of sections
//   write_4_i8(0x00, 0x00, 0x00, 0x00); // flags
// }

// #endif

// #ifdef __osx__

void write_mach_o_header_64() {
  write_4_i8(0xcf, 0xfa, 0xed, 0xfe); // magic number (MH_MAGIC_64)
  write_4_i8(0x07, 0x00, 0x00, 0x01); // CPU type (CPU_TYPE_X86_64)
  write_4_i8(0x03, 0x00, 0x00, 0x80); // CPU subtype (CPU_SUBTYPE_X86_64_ALL)
  write_4_i8(0x02, 0x00, 0x00, 0x00); // file type (MH_EXECUTE)
  write_4_i8(0x01, 0x00, 0x00, 0x00); // number of load commands
  write_4_i8(0x32, 0x00, 0x00, 0x00); // size of all load commands
  write_4_i8(0x00, 0x00, 0x00, 0x00); // flags
  write_4_i8(0x00, 0x00, 0x00, 0x00); // reserved
}

void write_mach_o_load_command_64() {
  write_4_i8(0x19, 0x00, 0x00, 0x00); // LC_SEGMENT_64
  write_4_i8(0x38, 0x00, 0x00, 0x00); // command size
  write_str("TEXT");                  // segment name
  write_4_i8(0x00, 0x00, 0x00, 0x00); // VM address
  write_4_i8(0x00, 0x00, 0x00, 0x00); // VM address (cont.)
  write_4_i8(0x58 + code_alloc, 0x00, 0x00, 0x00); // VM size (0x58 = 32 bytes header + 56 bytes load command)
  write_4_i8(0x00, 0x00, 0x00, 0x00); // file offset
  write_4_i8(0x58 + code_alloc, 0x00, 0x00, 0x00); // file size (0x58 = 32 bytes header + 56 bytes load command)
  write_4_i8(0x07, 0x00, 0x00, 0x00); // max VM protection
  write_4_i8(0x05, 0x00, 0x00, 0x00); // initial VM protection
  write_4_i8(0x01, 0x00, 0x00, 0x00); // number of sections
  write_4_i8(0x00, 0x00, 0x00, 0x00); // flags
}

// #endif

void generate_mach_o_exe() {
  int i = 0;

// #ifdef __osx32__
  // write_mach_o_header_32();
  // write_mach_o_load_command_32();
// #elif defined(__osx__)
  write_mach_o_header_64();
  write_mach_o_load_command_64();
// #endif

  while (i < code_alloc) {
    write_i8(code[i]);
    i += 1;
  }
}
