#pragma once

#include "windef.h"
#include "ordered_array.h"

#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MAGIC		0x123890AB
#define HEAP_MIN_SIZE	0x70000

/*
	hole / block �� ������ ����
*/
// header
typedef struct
{
	u32int magic;	// Magic Number, �� �ջ��� �߻��ߴ����� Ȯ���� �� �ִ� üũ�� ��
	u8int is_hole;	// 1�̸� hole, 0�̸� block
	u32int size;	// header�� footer�� ������ ����� �� ũ��
} header_t;

// footer
typedef struct
{
	u32int magic;		// 4����Ʈ �����ѹ�
	header_t* header;	// ��� ����� ����Ű�� ������
} footer_t;

typedef struct
{
	ordered_array_t index;
	u32int start_address;	// �Ҵ�� ������ �����ּ�
	u32int end_address;		// �Ҵ�� ������ ���ּ�, max_address���� Ȯ��� �� �ִ�.
	u32int max_address;		// ���� Ȯ��� �� �ִ� �Ѱ� �ּ�
	u8int supervisor;		// Should extra pages requested by us be mapped as supervisor-only?
	u8int readonly;			// Should extra pages requested by us be mapped as read-only?
} heap_t;

#ifdef __cplusplus
extern "C" {
#endif

// ���ο� �� ����
heap_t* create_kernel_heap(u32int start, u32int end_addr, u32int max, u8int supervisor, u8int readonly);
heap_t* create_heap(u32int start, u32int end_addr, u32int max, u8int supervisor, u8int readonly);

// size ��ŭ �������� �޸� ������ �Ҵ��Ѵ�.
// ���� page_align == 1 �̶��, ������ ��迡�� ���� ����� �����Ѵ�.
/*
�ý��� �޸𸮴� "������"��� ���� ������ ����������.
���� ��Ű��ó�� ��� 4096Byte(4K)�� �����ȴ�.
�޸� �ּҰ� ������ �����ּҶ�� "������ ���ĵǾ���.(page-aligned)"�� �Ѵ�.
�׷��Ƿ� 4K ������ ũ���� ��Ű��ó���� 
4096, 12,288(3*4096), 413,696(101*4096)�� ������ ���ĵ� �޸� �ּ��� �ν��Ͻ����̴�.

�� �޸� �����ڿ� ���� ����� ���μ������� ������ ������ �䱸�ȴ�.
*/
void* memory_alloc(u32int size, u8int page_align, heap_t* heap);

// 'alloc'�� �����Ҵ�� block�� �����Ѵ�.
void free(void* p, heap_t* heap);

/*
	sz ũ�⸸ŭ �޸𸮸� �Ҵ��Ѵ�.
	���� align == 1�̸�, �޸𸮴� page-align�Ǿ�� �ϰ�,
	���� phys != 0 �̸�, �Ҵ�� �޸��� ��ġ�� phys�� ����ȴ�.

	�̰��� kmalloc�� ���� �����̴�. ����� ģȭ��
	�Ű����� ǥ���� kmalloc, kmalloc_a�� ����� �� �ִ�.
	kmalloc_ap, kmalloc_p.
*/
u32int kmalloc_int(u32int sz, int align, u32int* phys);

u32int kmalloc_a(u32int sz);

// sz ��ŭ �޸𸮸� �Ҵ��� phys�� �����Ѵ�.
u32int kmalloc_p(u32int sz, u32int* phys);

// sz ��ŭ �޸𸮸� �Ҵ��ϰ� �����ּҴ� phys�� ���ϵȴ�.
u32int kmalloc_ap(u32int sz, u32int* phys);

// �Ϲ����� �Ҵ��Լ�
u32int kmalloc(u32int sz);
u32int malloc(u32int sz);
u32int calloc(u32int count, u32int size);

// �Ϲ����� ���� �Լ�
void kfree(void* p);

#ifdef __cplusplus
}
#endif