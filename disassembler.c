#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define true  1
#define false 0

const char* registers[] = 
           {"zero",
     		"ra",
		    "sp",
		    "gp",
		    "tp",
		    "t0",
		    "t1",
		    "t2",
		    "s0",
		    "s1",
		    "a0",
		    "a1",
		    "a2",
		    "a3",
		    "a4",
		    "a5",
		    "a6",
		    "a7",
		    "s2",
		    "s3",
		    "s4",
		    "s5",
		    "s6",
		    "s7",
		    "s8",
		    "s9",
		    "s10",
		    "s11",
		    "t3",
		    "t4",
		    "t5",
		    "t6"};

enum Command {
	// RV32I
	LUI,
        AUIPC,
	JAL,
	JALR,
	BEQ,
	BNE,
	BLT,
	BGE,
	BLTU,
	BGEU,
	LB,
	LH,
	LW,
	LBU,
	LHU,
	SB,
	SH,
	SW,
	ADDI,
	SLTI,
	SLTIU,
	XORI,
	ORI,
	ANDI,
	SLLI,
	SRLI,
	SRAI,
	ADD,
	SUB,
	SLL,
	SLT,
	SLTU,
	XOR,
	SRL,
	SRA,
	OR,
	AND,
	FENCE,
	FENCE_I,
	ECALL,
	EBREAK,
	CSRRW,
	CSRRS,
	CSRRC,
	CSRRWI,
	CSRRSI,
	CSRRCI,

        // RV32M
	MUL,
	MULH,
	MULHSU,
	MULHU,
	DIV,
	DIVU,
	REM,
	REMU,

	UNKNOWN
} Command;

// command[a:b] inclusively i.e. [31:12]
uint32_t get_slice(uint32_t cmd, uint8_t a, uint8_t b) {
  return (cmd >> b) & ((1 << (a - b + 1)) - 1);
}

enum Command get_command(uint32_t command){
  uint32_t com6_0   = get_slice(command, 6, 0);                                         // ____ ____ ____ ____ ____ ____ _xxx xxxx
  uint32_t com11_7  = get_slice(command, 11, 7);                                        // ____ ____ ____ ____ ____ xxxx x___ ____
  uint32_t com14_12 = get_slice(command, 14, 12);                                       // ____ ____ ____ ____ _xxx ____ ____ ____
  uint32_t com19_15 = get_slice(command, 19, 15);                                       // ____ ____ ____ xxxx x___ ____ ____ ____
  uint32_t com24_20 = get_slice(command, 24, 20);                                       // ____ ___x xxxx ____ ____ ____ ____ ____
  uint32_t com31_25 = get_slice(command, 31, 25);                                       // xxxx xxx_ ____ ____ ____ ____ ____ ____
  uint32_t com31_20 = get_slice(command, 31, 20);                                       // xxxx xxxx xxxx ____ ____ ____ ____ ____
  uint32_t com31_28 = get_slice(command, 31, 28);                                       // xxxx ____ ____ ____ ____ ____ ____ ____
  uint32_t com27_25 = get_slice(command, 27, 25);                                       // ____ xxxx ____ ____ ____ ____ ____ ____
  uint32_t com24_21 = get_slice(command, 24, 21);                                       // ____ ____ xxxx ____ ____ ____ ____ ____
  uint32_t com19_0  = get_slice(command, 19, 0);                                        // ____ ____ ____ ____ xxxx xxxx xxxx xxxx
  if (com6_0 == 0b0110111) return LUI;                                                  //                                011 0111
  if (com6_0 == 0b0010111) return AUIPC;                                                //                                001 0111
  if (com6_0 == 0b1101111) return JAL;                                                  //                                110 1111
  if (com6_0 == 0b1100111 && com14_12 == 0b000) return JALR;                            //                      000       110 0111
  if (com6_0 == 0b1100011 && com14_12 == 0b000) return BEQ;                             //                      000       110 0011
  if (com6_0 == 0b1100011 && com14_12 == 0b001) return BNE;                             //                      001       110 0011
  if (com6_0 == 0b1100011 && com14_12 == 0b100) return BLT;                             //                      100       110 0011
  if (com6_0 == 0b1100011 && com14_12 == 0b101) return BGE;                             //                      101       110 0011
  if (com6_0 == 0b1100011 && com14_12 == 0b110) return BLTU;                            //                      110       110 0011
  if (com6_0 == 0b1100011 && com14_12 == 0b111) return BGEU;                            //                      111       110 0011
  if (com6_0 == 0b0000011 && com14_12 == 0b000) return LB;                              //                      000       000 0011
  if (com6_0 == 0b0000011 && com14_12 == 0b001) return LH;                              //                      001       000 0011
  if (com6_0 == 0b0000011 && com14_12 == 0b010) return LW;                              //                      010       000 0011
  if (com6_0 == 0b0000011 && com14_12 == 0b100) return LBU;                             //                      100       000 0011
  if (com6_0 == 0b0000011 && com14_12 == 0b101) return LHU;                             //                      101       000 0011
  if (com6_0 == 0b0100011 && com14_12 == 0b000) return SB;                              //                      000       010 0011
  if (com6_0 == 0b0100011 && com14_12 == 0b001) return SH;                              //                      001       010 0011
  if (com6_0 == 0b0100011 && com14_12 == 0b010) return SW;                              //                      010       010 0011
  if (com6_0 == 0b0010011 && com14_12 == 0b000) return ADDI;                            //                      000       001 0011
  if (com6_0 == 0b0010011 && com14_12 == 0b010) return SLTI;                            //                      010       001 0011
  if (com6_0 == 0b0010011 && com14_12 == 0b011) return SLTIU;                           //                      011       001 0011
  if (com6_0 == 0b0010011 && com14_12 == 0b100) return XORI;                            //                      100       001 0011
  if (com6_0 == 0b0010011 && com14_12 == 0b110) return ORI;                             //                      110       001 0011
  if (com6_0 == 0b0010011 && com14_12 == 0b111) return ANDI;                            //                      111       001 0011
  if (com6_0 == 0b0010011 && com14_12 == 0b001 && com31_25 == 0b0000000) return SLLI;   // 0000 0000            001       001 0011
  if (com6_0 == 0b0010011 && com14_12 == 0b101 && com31_25 == 0b0000000) return SRLI;   // 0000 0000            101       001 0011
  if (com6_0 == 0b0010011 && com14_12 == 0b101 && com31_25 == 0b0100000) return SRAI;   // 0100 0000            101       001 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b000 && com31_25 == 0b0000000) return ADD;    // 0000 0000            000       011 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b000 && com31_25 == 0b0100000) return SUB;    // 0100 0000            000       011 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b001 && com31_25 == 0b0000000) return SLL;    // 0000 0000            001       011 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b010 && com31_25 == 0b0000000) return SLT;    // 0000 0000            010       011 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b011 && com31_25 == 0b0000000) return SLTU;   // 0000 0000            011       011 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b100 && com31_25 == 0b0000000) return XOR;    // 0000 0000            100       011 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b101 && com31_25 == 0b0000000) return SRL;    // 0000 0000            101       011 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b101 && com31_25 == 0b0100000) return SRA;    // 0100 0000            101       011 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b110 && com31_25 == 0b0000000) return OR;     // 0000 0000            110       011 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b111 && com31_25 == 0b0000000) return AND;    // 0000 0000            111       011 0011
  if (com19_0 == 0b00000000000000001111 && com31_28 == 0b0000) return FENCE;            // 0000           0000 0000 0000 0000 1111
  if (command == 0b00000000000000000001000000001111) return FENCE_I;                    // 0000 0000 0000 0000 0001 0000 0000 1111
  if (command == 0b00000000000000000000000001110011) return ECALL;                      // 0000 0000 0000 0000 0000 0000 0111 0011
  if (command == 0b00000000000100000000000001110011) return EBREAK;                     // 0000 0000 0001 0000 0000 0000 0111 0011
  if (com6_0 == 0b1110011 && com14_12 == 0b001) return CSRRW;                           //                      001       111 0011
  if (com6_0 == 0b1110011 && com14_12 == 0b010) return CSRRS;                           //                      010       111 0011
  if (com6_0 == 0b1110011 && com14_12 == 0b011) return CSRRC;                           //                      011       111 0011
  if (com6_0 == 0b1110011 && com14_12 == 0b101) return CSRRWI;                          //                      101       111 0011
  if (com6_0 == 0b1110011 && com14_12 == 0b110) return CSRRSI;                          //                      110       111 0011
  if (com6_0 == 0b1110011 && com14_12 == 0b111) return CSRRCI;                          //                      111       111 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b000 && com31_25 == 0b0000001) return MUL;    // 0000 001             000       011 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b001 && com31_25 == 0b0000001) return MULH;   // 0000 001             001       011 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b010 && com31_25 == 0b0000001) return MULHSU; // 0000 001             010       011 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b011 && com31_25 == 0b0000001) return MULHU;  // 0000 001             011       011 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b100 && com31_25 == 0b0000001) return DIV;    // 0000 001             100       011 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b101 && com31_25 == 0b0000001) return DIVU;   // 0000 001             101       011 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b110 && com31_25 == 0b0000001) return REM;    // 0000 001             110       011 0011
  if (com6_0 == 0b0110011 && com14_12 == 0b111 && com31_25 == 0b0000001) return REMU;   // 0000 001             111       011 0011
  return UNKNOWN;
}


const char section_name[] = {0x2e, 0x74, 0x65, 0x78, 0x74, 0x00}; // .text
const unsigned long int section_name_length = 6;

FILE *input;

typedef struct {
  uint8_t     e_ident[16];
  uint16_t    e_type;  
  uint16_t    e_machine;
  uint32_t    e_version;
  uint32_t    e_entry;
  uint32_t    e_phoff;
  uint32_t    e_shoff;
  uint32_t    e_flags;
  uint16_t    e_ehsize;
  uint16_t    e_phentsize;
  uint16_t    e_phnum;
  uint16_t    e_shentsize;
  uint16_t    e_shnum;  
  uint16_t    e_shstrndx;
} elf_header;


typedef struct {
  uint32_t    sh_name;
  uint32_t    sh_type;
  uint32_t    sh_flags;
  uint32_t    sh_addr;
  uint32_t    sh_offset;
  uint32_t    sh_size;
  uint32_t    sh_link;
  uint32_t    sh_info;
  uint32_t    sh_addralign;
  uint32_t    sh_entsize;
} section_header;


typedef struct {
  uint32_t    st_name;
  uint32_t    st_value;
  uint32_t    st_size;
  uint8_t     st_info;
  uint8_t     st_other;
  uint16_t    st_shndx;
} symtab_entry;

int check_header(elf_header *header) {
  if (header->e_ident[0] != 0x7f || 
      header->e_ident[1] != 'E'  ||
      header->e_ident[2] != 'L'  ||
      header->e_ident[3] != 'F') {
      printf("Magic numbers are wrong for ELF file format");
      return 0xe1f;
  } else if (header->e_ident[4] != 1) {
      printf("ELF format should be 32bit");
      return 0x32b17;
  } else if (header->e_ident[5] != 1) {
      printf("ELF format should be Little Endian");
      return 0x11771e;
  } else if (header->e_machine != 0xf3) {
      printf("ELF format should be RISC-V");
      return 0x415c;
  }
  return 0;
}

int extend_sign(uint32_t num, int bits) {
  return - (num & (1 << bits)) + (num & ((1 << bits) - 1));
}

int close_files(){
  if (input) {
    fclose(input);
  }
  return 1;
}

int open_files(int argc, char **argv) {
  if (argc == 2 || argc == 3) {
    char* input_name = argv[1];
    input = fopen(input_name, "rb");
    if (!input) {
        printf("Input file is unreachable");
        return 0x1f;
    }
    if (argc == 3) {
        char* output_name = argv[2];
        freopen(output_name, "w", stdout);
    }
    return 0;
  }
  else{
    printf("usage: %s <input> [output]\n", argv[0]);
    return 0xdead;
  }
}


int show_u_type(char* name, int command, int current_offset, char* label, int sign) {
   int64_t imm = get_slice(command, 31, 12);                       // xxxx xxxx xxxx xxxx xxxx ____ ____ ____
   uint32_t rd  = get_slice(command, 11, 7);                       // ____ ____ ____ ____ ____ xxxx x___ ____
   if (sign) imm = extend_sign(imm, 11);
   printf("%08x: %s\t\t%s \t%s, %ld\n", current_offset, label, name, registers[rd], imm);
}

int show_j_type(char* name, int command, int current_offset, char* label, int sign) {
   uint32_t imm20    = get_slice(command, 31, 31);                 // x___ ____ ____ ____ ____ ____ ____ ____
   uint32_t imm10_1  = get_slice(command, 30, 21);                 // _xxx xxxx xxx_ ____ ____ ____ ____ ____
   uint32_t imm11    = get_slice(command, 20, 20) ;                // ____ ____ ___x ____ ____ ____ ____ ____
   uint32_t imm19_12 = get_slice(command, 19, 12);                 // ____ ____ ____ xxxx xxxx ____ ____ ____ 
   int64_t imm = (imm20 << 20) + (imm19_12 << 12) + (imm11 << 11) + (imm10_1 << 1);
   uint32_t rd       = get_slice(command, 11, 7);                  // ____ ____ ____ ____ ____ xxxx x___ ____
   if (sign) imm = extend_sign(imm, 11);
   printf("%08x: %s\t\t%s \t%s, %ld\n", current_offset, label, name, registers[rd], imm);
}

int show_i_type(char* name, int command, int current_offset, char* label, int brackets, int sign) {
   int64_t imm  = get_slice(command, 31, 20);                      // xxxx xxxx xxxx ____ ____ ____ ____ ____
   uint32_t rs1 = get_slice(command, 19, 15);                      // ____ ____ ____ xxxx x___ ____ ____ ____
   uint32_t rd  = get_slice(command, 11, 7);                       // ____ ____ ____ ____ ____ xxxx x___ ____
   if (sign) imm = extend_sign(imm, 11);
   if (brackets) {
   	printf("%08x: %s\t\t%s \t%s, %ld(%s)\n", current_offset, label, name, registers[rd], imm, registers[rs1]);
   } else {
	printf("%08x: %s\t\t%s \t%s, %s, %ld\n", current_offset, label, name, registers[rd], registers[rs1], imm);
   }
}

int show_b_type(char* name, int command, int current_offset, char* label, int sign) {
   uint32_t imm12   = get_slice(command, 31, 31);                  // x___ ____ ____ ____ ____ ____ ____ ____
   uint32_t imm10_5 = get_slice(command, 30, 25);                  // _xxx xxx_ ____ ____ ____ ____ ____ ____
   uint32_t rs2     = get_slice(command, 24, 20);                  // ____ ___x xxxx ____ ____ ____ ____ ____
   uint32_t rs1     = get_slice(command, 19, 15);                  // ____ ____ ____ xxxx x___ ____ ____ ____
   uint32_t imm4_1  = get_slice(command, 11, 8);                   // ____ ____ ____ ____ ____ xxxx ____ ____
   uint32_t imm11   = get_slice(command, 7, 7);                    // ____ ____ ____ ____ ____ ____ x___ ____
   int64_t imm      = (imm12 << 12) + (imm11 << 11) + (imm10_5 << 5) + (imm4_1 << 1);
   if (sign) imm = extend_sign(imm, 11);
   printf("%08x: %s\t\t%s \t%s, %s, %ld\n", current_offset, label, name, registers[rs1], registers[rs2], imm);
}

int show_s_type(char* name, int command, int current_offset, char* label, int brackets, int sign) {
   uint32_t imm11_5 = get_slice(command, 31, 25);                  // xxxx xxx_ ____ ____ ____ ____ ____ ____
   uint32_t rs2     = get_slice(command, 24, 20);                  // ____ ___x xxxx ____ ____ ____ ____ ____
   uint32_t rs1     = get_slice(command, 19, 15);                  // ____ ____ ____ xxxx x___ ____ ____ ____
   uint32_t imm4_0  = get_slice(command, 11, 7);                   // ____ ____ ____ ____ ____ xxxx x___ ____
   int64_t imm      = (imm11_5 << 5) + imm4_0;
   if (sign) imm = extend_sign(imm, 11);
   if (brackets) {
   	printf("%08x: %s\t\t%s \t%s, %ld(%s)\n", current_offset, label, name, registers[rs2], imm, registers[rs1]);
   } else {
	printf("%08x: %s\t\t%s \t%s, %s, %ld\n", current_offset, label, name, registers[rs1], registers[rs2], imm);
   }
   
}

int show_r_type(char* name, int command, int current_offset, char* label) {
   uint32_t rs2     = get_slice(command, 24, 20);                  // ____ ___x xxxx ____ ____ ____ ____ ____
   uint32_t rs1     = get_slice(command, 19, 15);                  // ____ ____ ____ xxxx x___ ____ ____ ____
   uint32_t rd  = get_slice(command, 11, 7);                       // ____ ____ ____ ____ ____ xxxx x___ ____
   printf("%08x: %s\t\t%s \t%s, %s, %s\n", current_offset, label, name, registers[rd], registers[rs1], registers[rs2]);
}

int show_shamt_type(char* name, int command, int current_offset, char* label, int sign) {
   int64_t shamt = get_slice(command, 24, 20);                   // ____ ___x xxxx ____ ____ ____ ____ ____
   uint32_t rs1  = get_slice(command, 19, 15);                   // ____ ____ ____ xxxx x___ ____ ____ ____
   uint32_t rd   = get_slice(command, 11, 7);                    // ____ ____ ____ ____ ____ xxxx x___ ____
   if (sign) shamt = extend_sign(shamt, 11);
   printf("%08x: %s\t\t%s \t%s, %s, %ld\n", current_offset, label, name, registers[rd], registers[rs1], shamt);
}

int show_fence_type(char* name, int command, int current_offset, char* label) {
   uint32_t pred = get_slice(command, 27, 24);                     // ____ xxxx ____ ____ ____ ____ ____ ____ 
   uint32_t succ = get_slice(command, 23, 20);                     // ____ ____ xxxx ____ ____ ____ ____ ____
   printf("%08x: %s\t\t%s \t%u, %u", current_offset, label,  name, pred, succ);
}

int show_csr_type(char* name, int command, int current_offset, char* label) {
   uint32_t rs1 = get_slice(command, 19, 15);                  // ____ ____ ____ xxxx x___ ____ ____ ____
   uint32_t rd  = get_slice(command, 11, 7);                   // ____ ____ ____ ____ ____ xxxx x___ ____
   uint32_t csr = get_slice(command, 31, 20);                  // xxxx xxxx xxxx ____ ____ ____ ____ ____
   printf("%08x: %s\t\t%s \t%s, %u, %s", current_offset, label, name, registers[rd], csr, registers[rs1]);
}

int show_csr_zimm_type(char* name, int command, int current_offset, char* label) {
   uint32_t zimm = get_slice(command, 19, 15);                 // ____ ____ ____ xxxx x___ ____ ____ ____
   uint32_t rd  = get_slice(command, 11, 7);                   // ____ ____ ____ ____ ____ xxxx x___ ____
   uint32_t csr = get_slice(command, 31, 20);                  // xxxx xxxx xxxx ____ ____ ____ ____ ____
   printf("%08x: %s\t\t%s \t%s, %u, %u", current_offset, label, name, registers[rd], csr, zimm);
}

int main(int argc, char **argv) {
  if (open_files(argc, argv) != 0){
     return close_files();
  }

  elf_header header;
  fread(&header, 1, sizeof(header), input);
  if (check_header(&header) != 0){
     return close_files();
  }
  
  fseek(input, header.e_shoff, SEEK_SET);
  section_header section_headers[header.e_shnum];
  fread(section_headers, header.e_shnum, sizeof(section_header), input);

  fseek(input, section_headers[header.e_shstrndx].sh_offset, SEEK_SET);
  uint8_t section_names[section_headers[header.e_shstrndx].sh_size];
  fread(section_names, section_headers[header.e_shstrndx].sh_size, sizeof(uint8_t), input);
  
  int section_index;
  int symtab_index;
  int strtab_index;
  for (int i = 0; i < header.e_shnum; i++) {
     int link = section_headers[i].sh_name;
     char buffer[section_name_length];
     for(int p = 0; p < section_name_length; p++){
        buffer[p] = 0;
     }

     int j = 0;
     while(section_names[link] != 0x00) {
        buffer[j++] = section_names[link++];
	if (j == section_name_length){
	  break;
	}
     }
     
     if (!strncmp(section_name, buffer, section_name_length)) {
       section_index = i;  
     }
     if (section_headers[i].sh_type == 0x2) {
       symtab_index = i;
     }
     if (section_headers[i].sh_type == 0x3) {
       strtab_index = i;
     }
  }

  uint32_t section_offset = section_headers[section_index].sh_offset;
  uint32_t section_size = section_headers[section_index].sh_size / 4;
  uint32_t section_address = section_headers[section_index].sh_addr;
  fseek(input, section_offset, SEEK_SET);
  uint32_t assembler_commands[section_size];
  fread(assembler_commands, section_size, sizeof(uint32_t), input);
  
  uint32_t symtab_offset = section_headers[symtab_index].sh_offset;
  uint32_t symtab_size = section_headers[symtab_index].sh_size / 16;
  uint32_t symtab_address = section_headers[symtab_index].sh_addr;
  fseek(input, symtab_offset, SEEK_SET);
  symtab_entry symtab_entries[symtab_size];
  fread(symtab_entries, symtab_size, sizeof(symtab_entry), input);

  fseek(input, section_headers[strtab_index].sh_offset, SEEK_SET);
  uint8_t strtab[section_headers[strtab_index].sh_size];
  fread(strtab, section_headers[strtab_index].sh_size, sizeof(uint8_t), input);

  for (int i = 0; i < section_size; i++) {
    enum Command command = get_command(assembler_commands[i]);
    int cmd = assembler_commands[i];
    uint32_t current_offset = section_address + i * 4; 
    int length = -1;
    char* label;
    for (int k = 0; k < symtab_size; k++) {
		if ((current_offset == symtab_entries[k].st_value) && ((symtab_entries[k].st_info & 0xf) == 2)) {     
		  int link = symtab_entries[k].st_name;
		  int j = 1;
		  while(strtab[link++] != 0x00);
		  length = link - symtab_entries[k].st_name;
		  label = malloc((length + 2) * sizeof(char));
		  link = symtab_entries[k].st_name;
		  label[0] = '<';
		  while(strtab[link] != 0x00) {	
			 label[j++] = strtab[link++];
		  }
		  label[j] = '>';
		}
    }
     if (length == -1) {
        label = "";
     }
     if (command == LUI) {
       show_u_type("lui", cmd, current_offset, label, true);	
     } else if (command == AUIPC) {
       show_u_type("auipc", cmd, current_offset, label, true);
     } else if (command == JAL) {
       show_j_type("jal", cmd, current_offset, label, true);
     } else if (command == JALR) {
       show_i_type("jalr", cmd, current_offset, label, false, true);
     } else if (command == BEQ) {
       show_b_type("beq", cmd, current_offset, label, true);
     } else if (command == BNE) {
       show_b_type("bne", cmd, current_offset, label, true);
     } else if (command == BLT) {
       show_b_type("blt", cmd, current_offset, label, true);
     } else if (command == BGE) {
       show_b_type("bge", cmd, current_offset, label, true);
     } else if (command == BLTU) {
       show_b_type("bltu", cmd, current_offset, label, true);
     } else if (command == BGEU) {
       show_b_type("bgeu", cmd, current_offset, label, true);
     } else if (command == LB) {
       show_i_type("lb", cmd, current_offset, label, true, true);
     } else if (command == LH) {
       show_i_type("lh", cmd, current_offset, label, true, true);
     } else if (command == LW) {
       show_i_type("lw", cmd, current_offset, label, true, true);
     } else if (command == LBU) {
       show_i_type("lbu", cmd, current_offset, label, true, true);
     } else if (command == LHU) {
       show_i_type("lhu", cmd, current_offset, label, true, true);
     } else if (command == SB) {
       show_s_type("sb", cmd, current_offset, label, true, true);
     } else if (command == SH) {
       show_s_type("sh", cmd, current_offset, label, true, true);
     } else if (command == SW) {
       show_s_type("sw", cmd, current_offset, label, true, true);
     } else if (command == ADDI) {
       show_i_type("addi", cmd, current_offset, label, false, true);
     } else if (command == SLTI) {
       show_i_type("slti", cmd, current_offset, label, false, true);
     } else if (command == SLTIU) {
       show_i_type("sltiu", cmd, current_offset, label, false, false);
     } else if (command == XORI) {
       show_i_type("xori", cmd, current_offset, label, false, true);
     } else if (command == ORI) {
       show_i_type("ori", cmd, current_offset, label, false, true);
     } else if (command == ANDI) {
       show_i_type("andi", cmd, current_offset, label, false, true);
     } else if (command == SLLI) {
       show_shamt_type("slli", cmd, current_offset, label, false);
     } else if (command == SRLI) {
       show_shamt_type("srli", cmd, current_offset, label, false);
     } else if (command == SRAI) {
       show_shamt_type("srai", cmd, current_offset, label, true);
     } else if (command == ADD) {
       show_r_type("add", cmd, current_offset, label);
     } else if (command == SUB) {
       show_r_type("sub", cmd, current_offset, label);
     } else if (command == SLL) {
       show_r_type("sll", cmd, current_offset, label);
     } else if (command == SLT) {
       show_r_type("slt", cmd, current_offset, label);
     } else if (command == SLTU) {
       show_r_type("sltu", cmd, current_offset, label);
     } else if (command == XOR) {
       show_r_type("xor", cmd, current_offset, label);
     } else if (command == SRL) {
       show_r_type("srl", cmd, current_offset, label);
     } else if (command == SRA) {
       show_r_type("sra", cmd, current_offset, label);
     } else if (command == OR) {
       show_r_type("or", cmd, current_offset, label);
     } else if (command == AND) {
       show_r_type("and", cmd, current_offset, label);
     } else if (command == FENCE) {
       show_fence_type("fence", cmd, current_offset, label);
     } else if (command == FENCE_I) {
       printf("%08x: %s\t\tfence.i\n", current_offset, label);
     } else if (command == ECALL) {
       printf("%08x: %s\t\tecall\n", current_offset, label);
     } else if (command == EBREAK) {
       printf("%08x: %s\t\tebreak\n", current_offset, label);
     } else if (command == CSRRW) {
       show_csr_type("csrrw", cmd, current_offset, label);
     } else if (command == CSRRS) {
       show_csr_type("csrrs", cmd, current_offset, label);
     } else if (command == CSRRC) {
       show_csr_type("csrrc", cmd, current_offset, label);
     } else if (command == CSRRWI) {
       show_csr_zimm_type("csrrwi", cmd, current_offset, label);
     } else if (command == CSRRSI) {
       show_csr_zimm_type("csrrsi", cmd, current_offset, label);
     } else if (command == CSRRCI) {
       show_csr_zimm_type("csrrci", cmd, current_offset, label);
     } else if (command == MUL) {
       show_r_type("mul", cmd, current_offset, label);
     } else if (command == MULH) {
       show_r_type("mulh", cmd, current_offset, label);
     } else if (command == MULHSU) {
       show_r_type("mulhsu", cmd, current_offset, label);
     } else if (command == MULHU) {
       show_r_type("mulhu", cmd, current_offset, label);
     } else if (command == DIV) {
       show_r_type("div", cmd, current_offset, label);
     } else if (command == DIVU) {
       show_r_type("divu", cmd, current_offset, label);
     } else if (command == REM) {
       show_r_type("rem", cmd, current_offset, label);
     } else if (command == REMU) {
       show_r_type("remu", cmd,  current_offset, label);
     } else {
       printf("%08x: <%s>\tUNKNOWN\n", current_offset, label);
     }
  }
  close_files();
  return 0;
}

