#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#define UI_TABLE_START 0x4ef4
#define STRING_POINTER_TABLE_START 0x1fad0
#define STRING_LOCALIZED_TABLE_START 0x4260

#define STRING_POINTER_TABLE_MAXLEN 5000

#define STRUCT_SIZE 28
#define NUM_VALS 479

char *string_data = {0};
unsigned int pointer_table[STRING_POINTER_TABLE_MAXLEN];
unsigned int pointer_null_offset = 0;
unsigned int pointer_table_length = 0;
unsigned int pointer_table_max_value = 0;

typedef enum language_t
{
	ENGLISH,
	FRENCH,
	GERMAN,
	ITALIAN,
	SPANISH,
	PORTUGUESE,
	DUTCH,
	SWEDISH,
	NORWEGIAN,
	FINNISH,
	JAPANESE,
	GREEK,
	TURKISH,
	CHINESE_TRADITIONAL,
	CHINESE_SIMPLIFIED,
	POLISH,
	JAPANESE_AGAIN,
	CZECH
} language;

int read_string_table(FILE *f)
{
	fseek(f, STRING_POINTER_TABLE_START, 0);
	if (!fread(pointer_table, sizeof(unsigned int), sizeof(pointer_table), f))
		return 1;
	for (unsigned int i = 0; i < STRING_POINTER_TABLE_MAXLEN; ++i)
	{
		unsigned int *pi = (unsigned int *)(pointer_table + i);
		if (*pi == 0xffffffff)
		{
			printf("string pointer table end: 0x%04x, %d items (0x%04x bytes)\n", STRING_POINTER_TABLE_START + i * 4, i, i * 4);
			pointer_table_length = i;
			break;
		}
		if (i == 0)
		{
			pointer_null_offset = *pi;
			printf("string data null offset: 0x%04x\n", pointer_null_offset);
		}
		*pi -= pointer_null_offset;
		if (*pi > pointer_table_max_value)
			pointer_table_max_value = *pi;
	}
	if (pointer_table_length == 0)
		return 1;
	unsigned int num_bytes = (pointer_table_max_value + 200) * sizeof(unsigned char);
	unsigned int start = pointer_null_offset - 0x8000000;
	fseek(f, start, 0);
	printf("reading %d bytes from 0x%04x...", num_bytes, start);
	string_data = (char *)malloc(num_bytes);
	if (!fread(string_data, num_bytes, 1, f))
		return 1;
	printf("done.\n");
	return 0;
}

// typedef struct local_string_t
// {
// 	unsigned short len;
// 	unsigned int localized_string_id;
// } local_string;

// local_string local_string_table[10];
// int read_localized_string_table(FILE *f)
// {
// 	fseek(f, STRING_LOCALIZED_TABLE_START, 0);
// 	if (!fread(local_string_table, sizeof(local_string), 10, f)) return 1;
// 	for (int i = 0; i < 10; ++i)
// 	{
// 		printf("%x\n", local_string_table[i].len);
// 	}
// 	return 0;
// }

char *getstring(int i)
{
	assert(i < pointer_table_length);
	return &string_data[pointer_table[i]];
}

void tobits(char val)
{
	for (int i = 7; 0 <= i; i--)
	{
		printf("%c", (val & (1 << i)) ? '1' : '0');
	}
}

// 28 bytes
typedef struct __attribute__((__packed__)) therapy_ui_value_t
{
	unsigned short flags;
	unsigned short idk;
	unsigned short idk2;
	unsigned short str_ind;
	unsigned short default_val;
	unsigned short idk3;
	unsigned short hilimit;
	unsigned short idk4;
	unsigned short lolimit;
	unsigned short idk5;
	unsigned short idk6;
	unsigned short scale;
	unsigned short increment;
	unsigned short unit;
} therapy_ui_value;

void print_unit(unsigned short val)
{
	switch (val)
	{
	case 0x149:
		printf("cmH2O");
		break;
	case 0x14d:
		printf("deg C");
		break;
	case 0x14e:
		printf("deg F");
		break;
	case 0x8c:
		printf("mL/kg");
		break;
	case 0x90:
		printf("mth");
		break;
	case 0x91:
		printf("hr");
		break;
	case 0x92:
		printf("min");
		break;
	case 0x93:
		printf("sec");
		break;
	case 0x95:
		printf("L");
		break;
	case 0x96:
		printf("L/min");
		break;
	case 0x98:
		printf("mL");
		break;
	case 0x9a:
		printf("dBm");
		break;
	case 0x8d:
		printf("BPM");
		break;
	case 0x8f:
		printf("%%");
		break;
	case 0xde:
		break;
	case 0xdf:
		printf("L");
		break;
	default:
		printf("(%04x)", val);
		break;
	}
}

int pprint_therapy_value(therapy_ui_value *t)
{
	unsigned char *tp = (unsigned char *)t;
	printf("flags: ");
	tobits(t->flags);
	printf(" str_ind: %04x ", t->str_ind);
	printf("val: %0.2f ", (float)t->default_val / (float)t->scale);
	printf("(%.2f-%.2f) ", (float)t->lolimit / (float)t->scale, (float)t->hilimit / (float)t->scale);
	printf("(+/- %.2f) ", (float)t->increment / (float)t->scale);
	print_unit(t->unit);
	// printf("%04x%04x%04x%04x%04x%04x", t->idk, t->idk2, t->idk3, t->idk4, t->idk5, t->idk6);
	return 0;
}

int main(int argc, char **argv)
{
	FILE *f = fopen(argv[1], "rb");
	int ret = 0;
	if (!read_string_table(f)) // && !read_localized_string_table(f))
	{
		fseek(f, UI_TABLE_START, 0);
		therapy_ui_value tv = {0};
		int i = 0;
		int num_vals = 479;
		while (feof(f) == 0 && i++ < num_vals)
		{
			fread(&tv, sizeof(tv), 1, f);
			fseek(f, STRUCT_SIZE - sizeof(tv), SEEK_CUR);
			// if (tv.unit != 0xde) continue;
			// if (tv.str_ind == 0xde) continue;
			printf("%03d ", i - 1);
			pprint_therapy_value(&tv);
			printf("\n");
		}
	}
	else
		ret = 1;
	fclose(f);
	return ret;
}