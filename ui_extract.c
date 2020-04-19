#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#define UI_TABLE_START 0x4ef4 
#define STRING_POINTER_TABLE_START 0x1fad0
#define STRING_LOCALIZED_TABLE_START 0x4260

#define STRING_POINTER_TABLE_MAXLEN 5000

#define MAX_STRING_LENGTH 100
#define NUM_STRINGS 2000

#define STRUCT_SIZE 28
#define NUM_VALS 20

char* string_data = {0};
unsigned int pointer_table[STRING_POINTER_TABLE_MAXLEN];
unsigned int pointer_null_offset = 0;
unsigned int pointer_table_length = 0;
unsigned int pointer_table_max_value = 0;

typedef enum language_t {
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
	if (!fread(pointer_table, sizeof(unsigned int), sizeof(pointer_table), f)) return 1;
	for (unsigned int i = 0; i < STRING_POINTER_TABLE_MAXLEN; ++i)
	{
		unsigned int *pi = (unsigned int*)(pointer_table + i);
		if (*pi == 0xffffffff)
		{
			printf("string pointer table end: 0x%04x, %d items (0x%04x bytes)\n", STRING_POINTER_TABLE_START + i * 4, i, i*4);
			pointer_table_length = i;
			break;
		}
		if (i == 0)
		{
			pointer_null_offset = *pi;
			printf("string data null offset: 0x%04x\n", pointer_null_offset);
		}
		*pi -= pointer_null_offset;
		if (*pi > pointer_table_max_value) pointer_table_max_value = *pi;
	}
	if (pointer_table_length == 0) return 1;
	unsigned int num_bytes = (pointer_table_max_value + 200)*sizeof(unsigned char);
	unsigned int start = pointer_null_offset - 0x8000000;
	fseek(f, start, 0);
	printf("reading %d bytes from 0x%04x...", num_bytes, start);
	string_data = (char*)malloc(num_bytes);
	if(!fread(string_data, num_bytes, 1, f)) return 1;
	printf("done.\n");
	return 0;
}

typedef struct string_len_table_t
{
	unsigned short len;
	unsigned int localized_string_id;
} string_len_table;

// local_string local_string_table[10];
// int read_localized_string_table(FILE *f)
// {
// 	fseek(f, STRING_LOCALIZED_TABLE_START, 0);
// 	if (!fread(local_string_table, sizeof(local_string), 10, f)) return 1;
// 	for (int i = 0; i < 10; ++i)
// 	{
// 		printf("%x\n", local_string_table[i].len);
// 	}
// 	return 1;


// 	return 0;
// }

char* string_lookup(short *string_id)
{
	char ***string_raw_table;
	string_len_table **string_localized_table;
	unsigned int locale;
	string_raw_table = (char***)string_raw_table_get();
	string_localized_table = (string_len_table**)(global_variables());
	locale = string_get_locale();
	return (*string_raw_table)[string_localized_table[2][*string_id].localized_stringg_id[locale]];
}

char* getstring(int i)
{
	assert(i < pointer_table_length);
	return &string_data[pointer_table[i]];
}

void tobits(char val) {
  for (int i = 7; 0 <= i; i--) {
    printf("%c", (val & (1 << i)) ? '1' : '0');
  }
}

// 28 bytes
typedef struct __attribute__((__packed__)) therapy_ui_value_t {
    unsigned short flags;
    unsigned int idk;
    unsigned short str_ind;
    unsigned short default_val;
    unsigned short idk4;
    unsigned short hilimit;
    unsigned short idk6;
    unsigned short lolimit;
    unsigned short idk7;
    unsigned short asfdsdfsg;
} therapy_ui_value;

int pprint_therapy_value (therapy_ui_value *t) {
if (t->str_ind != 0xde)	printf("%s\n",getstring(t->str_ind));
	unsigned char *tp = (unsigned char*)t;
	printf("flags: ");
	tobits(t->flags);
	printf(" idk1: %08x ", t->idk);
	printf("str_ind: %02x ", t->str_ind);
	printf("val: %.2f ", (float)t->default_val / 50);
	printf("(%.2f-%.2f) ", (float)t->lolimit / 50, (float)t->hilimit / 50);
//printf("High Limit: %f ", (float)*(uint32_t*)(tp+12) / 50);
//printf("Low Limit: %f", (float)*(uint32_t*)(tp+16) / 50);
return 0;
}

int main(int argc, char ** argv)
{
	FILE *f = fopen(argv[1], "rb");	
	int ret = 0;
	if (!read_string_table(f) && !read_localized_string_table(f))
	{
		fseek(f, UI_TABLE_START, 0);
		therapy_ui_value tv = {0};
		int i = 0;
		int num_vals = NUM_VALS;
		while (feof(f)==0 && i < num_vals) {
			fread(&tv, sizeof(tv), 1, f);
			fseek(f, STRUCT_SIZE - sizeof(tv), SEEK_CUR);
			printf("%03d ", i);
			if (!pprint_therapy_value(&tv)) i++;
			else continue;
			printf("\n");
		}
	} else ret = 1;
	fclose(f);
	return ret;
}
/*
int read_string_table(FILE *f)
{
	read_string_pointer_table(f);
	return 1;
	fseek(f, STRING_TABLE_START, 0);
	unsigned int stind = 0;
	while (stind < NUM_STRINGS && !feof(f))
	{
		int c = fgetc(f);
		if (c == '\0') continue;
		int i = 0;
		while (c != '\0')
		{
			string_table[stind][i++] = c;
			assert (i < MAX_STRING_LENGTH);
			c = fgetc(f);
		}
		printf("%02d %04x %s\n", stind, ftell(f), string_table[stind]);
		++stind;
		if (strstr(string_table[stind],"Crc16")){
			break;
		}
		while (fgetc(f) != '\0
		fscanf (f, "%s", string_table[stind]);
		printf ("%s \n", string_table[stind]);
	}
	return 0;
}
*/
