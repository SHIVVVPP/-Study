#pragma once
#include "windef.h"
#include "stdint.h"
#include "HAL.h"
#include "PageDirectoryEntry.h"
#include "PageTableEntry.h"

#define USER_VIRTUAL_STACK_ADDRESS		0x00F00000
#define KERNEL_VIRTUAL_HEAP_ADDRESS		0x10000000

using namespace PageTableEntry;
using namespace PageDirectoryEntry;

// I86 ��Ű��ó������ ������ ���̺��̳� ������ ���丮�� ���� 1024���� ��Ʈ���� ������.
// 32��Ʈ���� ��Ʈ���� ũ��� 4����Ʈ�̴�.
// ũ��( 4 * 1024 = 4K)
// ���� �� ���μ����� �����ּ� 4�Ⱑ�� ���� ǥ���ϱ� ���� �� 4MB��
// �޸� ������ �ʿ�� �Ѵ�. (4K(PageDirectoryEntry) + 1024 * 4K(PageTableEntry))
// �ϳ��� ������ ���̺��� 4MB�� ǥ���� �� �ְ� ������ ���丮�� 1024���� ������ ���̺���
// ǥ���� �� �����Ƿ� 4MB * 1024 = 4GB, �� 4GB ��ü�� ǥ���� �� �ִ�.

#define PAGES_PER_TABLE		1024
#define PAGES_PER_DIRECTORY	1024
#define PAGE_TABLE_SIZE		4096 // 1024(directory) * 1024(table) * 4MB

// ������ ���̺� �ϳ��� �ּ� ���� : 4MB
#define PTABLE_ADDR_SPACE_SIZE 0x400000 // 4* 2^20(MB)
// ������ ���丮 �ϳ��� ǥ���� �� �ִ� �ּҰ��� 4GB
#define DTABLE_ADDR_SPACE_SIZE 0x100000000 // 1024 * (4*2^20)

// �����ּҴ� PDE(10bit) PTE(10bit) Offset(12bit) �� �����Ǿ� ������,
// 0x3ff = 0011 1111 1111 (10�ڸ�)
#define PAGE_DIRECTORY_INDEX(x)			(((x >> 22) & 0x3ff))
#define PAGE_TABLE_INDEX(x)				((x >> 12) & 0x3ff)

#define MAX_PAGE_DIRECTORY_COUNT 40

typedef struct tag_PageTable
{
	PTE m_entries[PAGES_PER_TABLE];
}PageTable;

typedef struct tag_PageDirectory
{
	PDE m_entries[PAGES_PER_DIRECTORY];
}PageDirectory;

namespace VirtualMemoryManager
{
	// ����޸� �Ŵ����� �ʱ�ȭ �Ѵ�.
	bool Initialize();


// Page Directory
	PageDirectory* CreateCommonPageDirectory();
	//������ ���丮�� PDTR(PageDirectoryTable Register)�� ��Ʈ�Ѵ�.
	void SetPageDirectory(PageDirectory* dir);

	bool SetCurPageDirectory(PageDirectory* dir);
	PageDirectory* GetCurPageDirectory();

	bool SetKernelPageDirectory(PageDirectory* dir);
	PageDirectory* GetKernelPageDirectory();
	 
	//�ּҷκ��� ������ ���丮 ��Ʈ���� ���´�.
	PDE* GetPDE(PageDirectory* dir, uint32_t addr);

	// ������ ���丮�� �����Ѵ�
	void FreePageDirectory(PageDirectory* dir);
	//������ ���丮�� �ʱ�ȭ�Ѵ�.
	void ClearPageDirectory(PageDirectory* dir);

	//������ ���丮�� �����Ѵ�. �� ���� �ּҰ����� �����Ѵٴ� �ǹ��̴�.
	PageDirectory* CreatePageDirectory();

// Page Table

	//������ ���̺��� �����Ѵ�. ������ ���̺��� ũ��� 4K�̴�.
	bool CreatePageTable(PageDirectory* dir, uint32_t virt, uint32_t flags);

	// �������� �Ҵ��Ѵ�.
	bool AllocPage(PTE* e);
	// �������� ȸ���Ѵ�.
	void FreePage(PTE* e);

	//������ ���̺��� �ʱ�ȭ �Ѵ�.
	void ClearPageTable(PageTable* p);
	 
	// ������ ���丮�� ���ε� ������ ���̺��� �����Ѵ�.
	void UnmapPageTable(PageDirectory* dir, uint32_t virt);
	void UnmapPhysicalAddress(PageDirectory* dir, uint32_t virt);

	//������ ���̺� ��Ʈ��(PTE)�� �����´�.
	PTE* GetPTE(PageTable* p, uint32_t addr);

	//�ּҷκ��� PTE�� ���´�.
	uint32_t GetPageTableEntryIndex(uint32_t addr);
	//�ּҷκ��� ������ ���̺��� ���´�.
	uint32_t GetPageTableIndex(uint32_t addr);


//Mapping

	// �����ּҸ� �����ּҿ� �����Ѵ�. �� �������� ������ ���̺� ��Ʈ���� ������ ��ϵȴ�.
	void MapPhysicalAddressToVirtualAddress(PageDirectory* dir, uint32_t virt, uint32_t phys, uint32_t flags);

	// �����ּҷκ��� ���� �����ּҸ� ����.
	void* GetPhysicalAddressFromVirtualAddress(PageDirectory* directory, uint32_t virtualAddress);

}
