#include <stdio.h>
#include <string.h>

#define M_SIZE 10000
#define VAR 100

typedef struct Variable
{
	char name[50];
	void *ptr;
} Variable;

typedef struct MetaData
{
	size_t size;
	int free;
	struct MetaData *next;
} MetaData;

MetaData *lptr = NULL;
Variable var[VAR];
int var_count = 0;
char memory[M_SIZE];

void init()
{
	lptr = (MetaData *)memory;
	lptr->size = M_SIZE - sizeof(MetaData);
	lptr->free = 1;
	lptr->next = NULL;
}

void *Allocation(size_t size)
{
	if (size <= 0 || size > M_SIZE - sizeof(MetaData))
	{
		printf("Invalid size for allocation\n");
		return NULL;
	}

	MetaData *curr = lptr;
	MetaData *prev = NULL;
	MetaData *bestFit = NULL;

	while (curr != NULL)
	{
		if (curr->free && curr->size >= size && (!bestFit || curr->size < bestFit->size))
		{
			bestFit = curr;
			if (bestFit->size == size)
				break;
		}
		prev = curr;
		curr = curr->next;
	}

	if (!bestFit)
	{
		printf(" Memory is insufficient !!!\n");
		return NULL;
	}

	if (bestFit->size >= size + sizeof(MetaData))
	{
		MetaData *new = (MetaData *)((char *)bestFit + sizeof(MetaData) + size);
		new->size = bestFit->size - size - sizeof(MetaData);
		new->free = 1;
		new->next = bestFit->next;
		bestFit->next = new;
		bestFit->size = size;
	}

	bestFit->free = 0;
	return (void *)(bestFit + 1);
}

void merge()
{
	MetaData *curr = lptr;
	while (curr && curr->next)
	{
		if (curr->free && curr->next->free)
		{
			curr->size += sizeof(MetaData) + curr->next->size;
			curr->next = curr->next->next;
		}
		else
		{
			curr = curr->next;
		}
	}
}

void Free_(const char *nameptr)
{
	for (int i = 0; i < var_count; ++i)
	{
		if (strcmp(var[i].name, nameptr) == 0)
		{
			MetaData *block_to_free = (MetaData *)var[i].ptr - 1;
			block_to_free->free = 1;
			merge();

			strcpy(var[i].name, "");
			var[i].ptr = NULL;
			return;
		}
	}
	printf("Variable '%s' not found\n", nameptr);
}

void displayList()
{
	printf("Memory Block List:\n");
	printf("==================\n");
	MetaData *curr = lptr;
	int blockNumber = 1;
	while (curr)
	{
		printf("Block %d: Address: %p, Total Size: %zu, Free: %s\n",
		       blockNumber, curr, curr->size + sizeof(MetaData), curr->free ? "Yes" : "No");
		curr = curr->next;
		blockNumber++;
	}
	printf("==================\n");
}

int main()
{
	init();

	int choice;
	size_t size;
	char variableName[50];

	do
	{
		printf("\n1. Allocate memory\n");
		printf("2. Free memory\n");
		printf("3. Display memory block list\n");
		printf("0. Exit\n");
		printf("Enter your choice: ");
		scanf("%d", &choice);

		switch (choice)
		{
		case 1:
			printf("Enter variable name: ");
			scanf("%s", variableName);
			printf("Enter size to allocate: ");
			scanf("%zu", &size);
			void *ptr = Allocation(size);
			if (ptr)
			{

				strcpy(var[var_count].name, variableName);
				var[var_count].ptr = ptr;
				var_count++;
			}
			break;
		case 2:
			printf("Enter variable name to free: ");
			scanf("%s", variableName);
			Free_(variableName);
			break;
		case 3:
			displayList();
			break;
		case 0:
			displayList();
			printf("Exiting...\n");
			break;
		default:
			printf("Invalid choice\n");
		}
	} while (choice != 0);

	return 0;
}
