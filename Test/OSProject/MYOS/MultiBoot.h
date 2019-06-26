#pragma once
#include "stdint.h"
#include "windef.h"

/*   _emit is DB(Define Byte - 8bit) equivalent but not DD(Define double word - 4Bytes) equivalent exist
so we define it ourself */
// _emit �� DB(Define Byte - 8bit)�� ���������� DD(Define double word - 4Byte(32bit)�� �������� �����Ƿ� ���� ���� 
// _emit [Byte] ���ڷ� �Ѱ��� ����Ʈ���� ���� ���� �ڵ�� ����
#define dd(x)                            \
        __asm _emit     (x)       & 0xff \
        __asm _emit     (x) >> 8  & 0xff \
        __asm _emit     (x) >> 16 & 0xff \
        __asm _emit     (x) >> 24 & 0xff

#define KERNEL_STACK			0x0040000

// ALIGN �� GRUB���� PEĿ���� �ε��� �� �ִ� ���� �߿��� �� �� �ϳ��̴�.
// �ֳ��ϸ� PE header�� ���� ���� �κп� �����Ƿ� �ڵ弽���� �̵��Ǳ� �����̴�.
// �ڵ弽���� �̵���Ű�µ� ���Ǵ� �� (0x400)�� linker ���� �ɼ��� /ALIGN:value�̴�.
// ��� �������� �հ谡 512���� ũ�Ƿ�, ALIGN ���� �׺��� �� Ŀ�� �Ѵ�.
/*  This is the one of most important thing to be able to load a PE kernel
with GRUB. Because PE header are in the begining of the file, code section
will be shifted. The value used to shift the code section is the linker
align option /ALIGN:value. Note the header size sum is larger than 512,
so ALIGN value must be greater */
#define   ALIGN               0x400

/*
16���� �뷮 ������ ����ϱ�
1KB : 2�� 10����
1MB : 2�� 20����
1GB : 2�� 30����
1TB : 2�� 40����...

16���� digit �ϳ��� 2�� 4����

2�� 20������ ����� ������ ��������.
1MB : 0x100000 ( 0x0�� 5�� ��, 2�� 4���� X 5)
1TB : 0x100000 00000 (0x0�� 10��)

6������ �ڸ��� ������ ������ 16�� ��������
�׷��ٸ� �ڸ��� ������ ������ ���� ������ ������?
0x1          => 1
0x10        => 16
0x100       => 256
0x1000      => 4K
0x10000     => 64KB
0x100000  => 1MB(First Pattern���� �����ϰ� �� �� ����)
�ڸ����� �þ ������
1=>16=>256=>4(Kilo��ŭ ���� ����)=>64=>1(Kilo��ŭ ���� ����)
1MB �������� �ٽ� 1�� �������Ƿ� MB�� ���� 1=>16=>256...������ �ǰڱ�.

- ���뼺 �׽�Ʈ
0x10000000000(0x0�� 10��) : First Patten���� �ٷ� 1GB

0x4000
Second Pattern ����Ͽ� ���� ����Ͽ� ���� 0x1000�� ���غ��� 1=>16=>256=>4K�̰�
���ϱ� 4�� ���ָ� 16K
*/

// GRUB�� 0~1Mb(0x100000)�� ���
/*   Must be >= 1Mb(0x100000) for GRUB
Base adress from advanced linker option
*/
#define KERNEL_LOAD_ADDRESS            0x100000


#define   HEADER_ADRESS         KERNEL_LOAD_ADDRESS+ALIGN

#define MULTIBOOT_HEADER_MAGIC         0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC      0x2BADB002
#define MULTIBOOT_HEADER_FLAGS         0x00010003 
#define STACK_SIZE              0x4000    
#define CHECKSUM            -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

#pragma pack(push,1)// ����ü ���� ũ�� ���� (1����Ʈ��)
struct MULTIBOOT_HEADER {
	uint32_t magic;
	uint32_t flags;
	uint32_t checksum;
	uint32_t header_addr;
	uint32_t load_addr;
	uint32_t load_end_addr;
	uint32_t bss_end_addr;
	uint32_t entry_addr;
};

/*
ELF = Excutable and Linkable Format
��������, ��������, ���� ���̺귯�� �׸��� �ھ� ����(�۾����� �޸𸮻��¸� ���)�� ���� ǥ�� ���� ����
*/
struct ELFHeaderTable
{
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
};
typedef struct multiboot_elf_section_header_table multiboot_elf_section_header_table_t;

struct AOUTSymbolTable
{
	unsigned int tabsize;
	unsigned int strsize;
	unsigned int addr;
	unsigned int reserved;
};

//Bochs 2.4 doesn't provide a ROM configuration table. Virtual PC does.
struct ROMConfigurationTable
{
	unsigned short Length;
	unsigned char Model;
	unsigned char Submodel;
	unsigned char BiosRevision;
	bool DualBus : 1;
	bool MicroChannelBus : 1;
	bool EBDAExists : 1;
	bool WaitForExternalEventSupported : 1;
	bool Reserved0 : 1;
	bool HasRTC : 1;
	bool MultipleInterruptControllers : 1;
	bool BiosUsesDMA3 : 1;
	bool Reserved1 : 1;
	bool DataStreamingSupported : 1;
	bool NonStandardKeyboard : 1;
	bool BiosControlCpu : 1;
	bool BiosGetMemoryMap : 1;
	bool BiosGetPosData : 1;
	bool BiosKeyboard : 1;
	bool DMA32Supported : 1;
	bool ImlSCSISupported : 1;
	bool ImlLoad : 1;
	bool InformationPanelInstalled : 1;
	bool SCSISupported : 1;
	bool RomToRamSupported : 1;
	bool Reserved2 : 3;
	bool ExtendedPOST : 1;
	bool MemorySplit16MB : 1;
	unsigned char Reserved3 : 1;
	unsigned char AdvancedBIOSPresence : 3;
	bool EEPROMPresent : 1;
	bool Reserved4 : 1;
	bool FlashEPROMPresent : 1;
	bool EnhancedMouseMode : 1;
	unsigned char Reserved5 : 6;
};

struct Module
{
	void *ModuleStart;
	void *ModuleEnd;
	char *Name;
	unsigned int Reserved;
};

struct multiboot_mmap_entry
{
	uint32_t size;
	uint64_t addr;
	uint64_t len;
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
	uint32_t type;
};
typedef struct multiboot_mmap_entry multiboot_memory_map_t;

/* Drive Info structure.  */
struct drive_info
{
	/* The size of this structure.  */
	unsigned long size;

	/* The BIOS drive number.  */
	unsigned char drive_number;

	/* The access mode (see below).  */
	unsigned char drive_mode;

	/* The BIOS geometry.  */
	unsigned short drive_cylinders;
	unsigned char drive_heads;
	unsigned char drive_sectors;

	/* The array of I/O ports used for the drive.  */
	unsigned short drive_ports;
};

struct APMTable
{
	unsigned short Version;
	unsigned short CS;
	unsigned int Offset;
	unsigned short CS16Bit;	//This is the 16-bit protected mode code segment
	unsigned short DS;
	unsigned short Flags;
	unsigned short CSLength;
	unsigned short CS16BitLength;
	unsigned short DSLength;
};

struct VbeInfoBlock
{
	char Signature[4];
	unsigned short Version;
	short OEMString[2];
	unsigned char Capabilities[4];
	short VideoModes[2];
	short TotalMemory;
};

/*struct VbeModeInfo
{
	unsigned short Attributes;
	unsigned char WinA;
	unsigned char WinB;
	unsigned short Granularity;
	unsigned short WinSize;
	unsigned short SegmentA;
	unsigned short SegmentB;
	unsigned short WindowFunctionPointer[2];
	unsigned short Pitch;
	unsigned short XResolution;
	unsigned short YResolution;
	unsigned char CharacterWidth;
	unsigned char CharacterHeight;
	unsigned char Planes;
	unsigned char BitsPerPixel;
	unsigned char Banks;
	unsigned char MemoryModel;
	unsigned char BankSize;
	unsigned char ImagePages;
	unsigned char Reserved;

	unsigned char RedMask;
	unsigned char RedPosition;

	unsigned char GreenMask;
	unsigned char GreenPosition;

	unsigned char BlueMask;
	unsigned char BluePosition;

	unsigned char ReservedMask;
	unsigned char ReservedPosition;

	unsigned char DirectColorAttributes;

	unsigned int FrameBuffer;
};*/

struct VbeModeInfo
{
	UINT16 ModeAttributes;
	char WinAAttributes;
	char WinBAttributes;
	UINT16 WinGranularity;
	UINT16 WinSize;
	UINT16 WinASegment;
	UINT16 WinBSegment;
	UINT32 WinFuncPtr;
	short BytesPerScanLine;
	short XRes;
	short YRes;
	char XCharSize;
	char YCharSize;
	char NumberOfPlanes;
	char BitsPerPixel;
	char NumberOfBanks;
	char MemoryModel;
	char BankSize;
	char NumberOfImagePages;
	char res1;
	char RedMaskSize;
	char RedFieldPosition;
	char GreenMaskSize;
	char GreenFieldPosition;
	char BlueMaskSize;
	char BlueFieldPosition;
	char RsvedMaskSize;
	char RsvedFieldPosition;
	char DirectColorModeInfo; //MISSED IN THIS TUTORIAL!! SEE ABOVE
							  //VBE 2.0
	UINT32 PhysBasePtr;
	UINT32 OffScreenMemOffset;
	short OffScreenMemSize;
	//VBE 2.1
	short LinbytesPerScanLine;
	char BankNumberOfImagePages;
	char LinNumberOfImagePages;
	char LinRedMaskSize;
	char LinRedFieldPosition;
	char LingreenMaskSize;
	char LinGreenFieldPosition;
	char LinBlueMaskSize;
	char LinBlueFieldPosition;
	char LinRsvdMaskSize;
	char LinRsvdFieldPosition;
	char res2[194];
};


/*
GRUB�� ��ȣ ���� ��ȯ�� �� Ŀ���� ȣ�� �ϴµ�,
Ŀ�� ���忡���� �ϵ���� ���� ������ ��Ⱑ �����Ƿ�
GRUB�� Ŀ�ο� �Ѱ��ִ� multiboot_info ����ü�� Ȱ���ؼ�
�޸� ���������� � ����̽����� ������ �Ǿ������� ���� ������ �˾Ƴ� �� �ִ�.
OS�� GRUB�� �Ѱ��� �� ����ü�� ���� �ý��� ȯ���� �ʱ�ȭ �Ѵ�.
*/
struct multiboot_info
{
	uint32_t flags;
	// �÷��� : �÷��� ���� Ȯ���ؼ� VESA ��尡 ���������� ���θ� �ľ��� �� �ִ�.
	// ���� ���ڰ��� ǥ������ȸ(Video Electronics Standards Association, VESA)
	// ���̿����κ��� ���� �̿� ������ �޸� ���� ����
	uint32_t mem_lower;
	uint32_t mem_upper;

	uint32_t boot_device; // ���� ����̽��� ��ȣ
	char* cmdline; // Ŀ�ο� �ѱ�� command line

	//���� ��� ����Ʈ
	uint32_t mods_count;
	Module* Modules;
	// ������ ���ϰ� ����� ����
	union
	{
		AOUTSymbolTable AOUTTable;
		ELFHeaderTable ELFTable;
	} SymbolTables;

	// �޸� ���� ������ �˷��ش�.
	// �� ������ ���� �޸� Ư�� ����� ����� �� �ִ��� �ľ� �����ϴ�.
	uint32_t mmap_length;
	uint32_t mmap_addr;

	//�ش� PC�� �����ϴ� ����̺꿡 ���� ����
	uint32_t drives_length;
	drive_info* drives_addr;

	// ROM configuration table
	ROMConfigurationTable* ConfigTable;

	// ��Ʈ�δ� �̸�
	char* boot_loader_name;

	// APM table
	APMTable* APMTable;

	// ����
	VbeInfoBlock* vbe_control_info;
	VbeModeInfo* vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
};
typedef struct multiboot_info multiboot_info_t;



#pragma pack(pop) // ������ ���� ���·� �ǵ���
