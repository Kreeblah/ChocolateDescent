/*
The code contained in this file is not the property of Parallax Software,
and is not under the terms of the Parallax Software Source license.
Instead, it is released under the terms of the MIT License.
*/

#include "platform/platform_filesys.h"
#include "platform/posixstub.h"
#include "misc/error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define CHOCOLATE_DESCENT_HOG_15_BROKEN_HOG_SIZE 6856183
#define CHOCOLATE_DESCENT_HOG_15_FIXED_HOG_SIZE 6856701
#define CHOCOLATE_DESCENT_2_HOG_12_HOG_SIZE 7595079

extern int sha1digest(uint8_t *digest, char *hexdigest, const uint8_t *data, size_t databytes);
static char local_file_path_prefix[CHOCOLATE_MAX_FILE_PATH_SIZE] = {0};

int fix_broken_descent_1_15_patch();
int patch_descent_1_14_to_15();
int patch_descent_2_11_to_12();

void get_missing_file_locations(char* missing_file_string, const char* missing_file_list);

// This is not completely ready for primetime
// for Windows.  Drive letters aren't really
// accounted for.
void mkdir_recursive(const char *dir)
{
	char tmp[CHOCOLATE_MAX_FILE_PATH_SIZE];
	char *p = NULL;
	size_t len;

	snprintf(tmp, sizeof(tmp), "%s", dir);
	len = strlen(tmp);

	if(tmp[len - 1] == PLATFORM_PATH_SEPARATOR)
	{
		tmp[len - 1] = 0;
	}

	for(p = tmp + 1; *p; p++)
	{
		if(*p == PLATFORM_PATH_SEPARATOR)
		{
			*p = 0;
			mkdir(tmp, S_IRWXU);
			*p = PLATFORM_PATH_SEPARATOR;
		}
	}

	mkdir(tmp, S_IRWXU);
}

void init_all_platform_localized_paths()
{
	char temp_buf[CHOCOLATE_MAX_FILE_PATH_SIZE];

	mkdir_recursive(get_platform_localized_file_path_prefix());

	if(strlen(CHOCOLATE_CONFIG_DIR) > 0)
	{
		get_platform_localized_path(temp_buf, CHOCOLATE_CONFIG_DIR);
		mkdir_recursive(temp_buf);
	}
	if(strlen(CHOCOLATE_SYSTEM_FILE_DIR) > 0)
	{
		get_platform_localized_path(temp_buf, CHOCOLATE_SYSTEM_FILE_DIR);
		mkdir_recursive(temp_buf);
	}
	if(strlen(CHOCOLATE_PILOT_DIR) > 0)
	{
		get_platform_localized_path(temp_buf, CHOCOLATE_PILOT_DIR);
		mkdir_recursive(temp_buf);
	}
	if(strlen(CHOCOLATE_SAVE_DIR) > 0)
	{
		get_platform_localized_path(temp_buf, CHOCOLATE_SAVE_DIR);
		mkdir_recursive(temp_buf);
	}
	if(strlen(CHOCOLATE_HISCORE_DIR) > 0)
	{
		get_platform_localized_path(temp_buf, CHOCOLATE_HISCORE_DIR);
		mkdir_recursive(temp_buf);
	}
	if(strlen(CHOCOLATE_MISSIONS_DIR) > 0)
	{
		get_platform_localized_path(temp_buf, CHOCOLATE_MISSIONS_DIR);
		mkdir_recursive(temp_buf);
	}
	if(strlen(CHOCOLATE_DEMOS_DIR) > 0)
	{
		get_platform_localized_path(temp_buf, CHOCOLATE_DEMOS_DIR);
		mkdir_recursive(temp_buf);
	}
	if(strlen(CHOCOLATE_SOUNDFONTS_DIR) > 0)
	{
		get_platform_localized_path(temp_buf, CHOCOLATE_SOUNDFONTS_DIR);
		mkdir_recursive(temp_buf);
	}
}

void validate_required_files()
{
	char missing_file_list[2048];
	char missing_file_location_list[(20 * CHOCOLATE_MAX_FILE_PATH_SIZE) + 20];
	char missing_file_string[65536]; //There's no way anything should get this huge anywhere
	char temp_buf[CHOCOLATE_MAX_FILE_PATH_SIZE], temp_buf2[CHOCOLATE_MAX_FILE_PATH_SIZE];
	FILE *fp;

	memset(missing_file_list, 0, 2048);
	memset(missing_file_location_list, 0, (20 * CHOCOLATE_MAX_FILE_PATH_SIZE) + 20);

#if defined(BUILD_DESCENT1)
	get_full_file_path(temp_buf, "descent.hog", CHOCOLATE_SYSTEM_FILE_DIR);

	fp = fopen(temp_buf, "rb");
	if (!fp)
	{
		strncat(missing_file_list, "descent.hog\n", 12);
		strncat(missing_file_location_list, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE);
		strncat(missing_file_location_list, "\n", 1);
	}
	else
	{
		fclose(fp);
	}

	get_full_file_path(temp_buf, "descent.pig", CHOCOLATE_SYSTEM_FILE_DIR);

	fp = fopen(temp_buf, "rb");
	if (!fp)
	{
		strncat(missing_file_list, "descent.pig\n", 12);
		strncat(missing_file_location_list, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE);
		strncat(missing_file_location_list, "\n", 1);
	}
	else
	{
		fclose(fp);
	}
#elif defined(BUILD_DESCENT2)
	get_full_file_path(temp_buf, "alien1.pig", CHOCOLATE_SYSTEM_FILE_DIR);

	fp = fopen(temp_buf, "rb");
	if (!fp)
	{
		strncat(missing_file_list, "alien1.pig\n", 11);
		strncat(missing_file_location_list, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE);
		strncat(missing_file_location_list, "\n", 1);
	}
	else
	{
		fclose(fp);
	}

	get_full_file_path(temp_buf, "alien2.pig", CHOCOLATE_SYSTEM_FILE_DIR);

	fp = fopen(temp_buf, "rb");
	if (!fp)
	{
		strncat(missing_file_list, "alien2.pig\n", 11);
		strncat(missing_file_location_list, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE);
		strncat(missing_file_location_list, "\n", 1);
	}
	else
	{
		fclose(fp);
	}

	get_full_file_path(temp_buf, "d2x-h.mvl", CHOCOLATE_SYSTEM_FILE_DIR);

	fp = fopen(temp_buf, "rb");
	if (!fp)
	{
		get_full_file_path(temp_buf2, "d2x-l.mvl", CHOCOLATE_SYSTEM_FILE_DIR);

		fp = fopen(temp_buf2, "rb");

		if (!fp)
		{
			strncat(missing_file_list, "d2x-h.mvl and/or d2x-l.mvl\n", 27);
			strncat(missing_file_location_list, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE);
			strncat(missing_file_location_list, "\n", 1);
			strncat(missing_file_location_list, "and/or\n", 7);
			strncat(missing_file_location_list, temp_buf2, CHOCOLATE_MAX_FILE_PATH_SIZE);
			strncat(missing_file_location_list, "\n", 1);
		}
		else
		{
			fclose(fp);
		}
	}
	else
	{
		fclose(fp);
	}

	get_full_file_path(temp_buf, "descent2.ham", CHOCOLATE_SYSTEM_FILE_DIR);

	fp = fopen(temp_buf, "rb");
	if (!fp)
	{
		strncat(missing_file_list, "descent2.ham\n", 13);
		strncat(missing_file_location_list, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE);
		strncat(missing_file_location_list, "\n", 1);
	}
	else
	{
		fclose(fp);
	}

	get_full_file_path(temp_buf, "descent2.hog", CHOCOLATE_SYSTEM_FILE_DIR);

	fp = fopen(temp_buf, "rb");
	if (!fp)
	{
		strncat(missing_file_list, "descent2.hog\n", 13);
		strncat(missing_file_location_list, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE);
		strncat(missing_file_location_list, "\n", 1);
	}
	else
	{
		fclose(fp);
	}

	get_full_file_path(temp_buf, "descent2.s11", CHOCOLATE_SYSTEM_FILE_DIR);

	fp = fopen(temp_buf, "rb");
	if (!fp)
	{
		strncat(missing_file_list, "descent2.s11\n", 13);
		strncat(missing_file_location_list, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE);
		strncat(missing_file_location_list, "\n", 1);
	}
	else
	{
		fclose(fp);
	}

	get_full_file_path(temp_buf, "descent2.s22", CHOCOLATE_SYSTEM_FILE_DIR);

	fp = fopen(temp_buf, "rb");
	if (!fp)
	{
		strncat(missing_file_list, "descent2.s22\n", 13);
		strncat(missing_file_location_list, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE);
		strncat(missing_file_location_list, "\n", 1);
	}
	else
	{
		fclose(fp);
	}

	get_full_file_path(temp_buf, "fire.pig", CHOCOLATE_SYSTEM_FILE_DIR);

	fp = fopen(temp_buf, "rb");
	if (!fp)
	{
		strncat(missing_file_list, "fire.pig\n", 9);
		strncat(missing_file_location_list, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE);
		strncat(missing_file_location_list, "\n", 1);
	}
	else
	{
		fclose(fp);
	}

	get_full_file_path(temp_buf, "groupa.pig", CHOCOLATE_SYSTEM_FILE_DIR);

	fp = fopen(temp_buf, "rb");
	if (!fp)
	{
		strncat(missing_file_list, "groupa.pig\n", 11);
		strncat(missing_file_location_list, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE);
		strncat(missing_file_location_list, "\n", 1);
	}
	else
	{
		fclose(fp);
	}

	get_full_file_path(temp_buf, "ice.pig", CHOCOLATE_SYSTEM_FILE_DIR);

	fp = fopen(temp_buf, "rb");
	if (!fp)
	{
		strncat(missing_file_list, "ice.pig\n", 8);
		strncat(missing_file_location_list, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE);
		strncat(missing_file_location_list, "\n", 1);
	}
	else
	{
		fclose(fp);
	}

	get_full_file_path(temp_buf, "intro-h.mvl", CHOCOLATE_SYSTEM_FILE_DIR);

	fp = fopen(temp_buf, "rb");
	if (!fp)
	{
		get_full_file_path(temp_buf2, "intro-l.mvl", CHOCOLATE_SYSTEM_FILE_DIR);

		fp = fopen(temp_buf2, "rb");

		if (!fp)
		{
			strncat(missing_file_list, "intro-h.mvl and/or intro-l.mvl\n", 31);
			strncat(missing_file_location_list, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE);
			strncat(missing_file_location_list, "\n", 1);
			strncat(missing_file_location_list, "and/or\n", 7);
			strncat(missing_file_location_list, temp_buf2, CHOCOLATE_MAX_FILE_PATH_SIZE);
			strncat(missing_file_location_list, "\n", 1);
		}
		else
		{
			fclose(fp);
		}
	}
	else
	{
		fclose(fp);
	}

	get_full_file_path(temp_buf, "other-h.mvl", CHOCOLATE_SYSTEM_FILE_DIR);

	fp = fopen(temp_buf, "rb");
	if (!fp)
	{
		get_full_file_path(temp_buf2, "other-l.mvl", CHOCOLATE_SYSTEM_FILE_DIR);

		fp = fopen(temp_buf2, "rb");

		if (!fp)
		{
			strncat(missing_file_list, "other-h.mvl and/or other-l.mvl\n", 31);
			strncat(missing_file_location_list, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE);
			strncat(missing_file_location_list, "\n", 1);
			strncat(missing_file_location_list, "and/or\n", 7);
			strncat(missing_file_location_list, temp_buf2, CHOCOLATE_MAX_FILE_PATH_SIZE);
			strncat(missing_file_location_list, "\n", 1);
		}
		else
		{
			fclose(fp);
		}
	}
	else
	{
		fclose(fp);
	}

	get_full_file_path(temp_buf, "robots-h.mvl", CHOCOLATE_SYSTEM_FILE_DIR);

	fp = fopen(temp_buf, "rb");
	if (!fp)
	{
		get_full_file_path(temp_buf2, "robots-l.mvl", CHOCOLATE_SYSTEM_FILE_DIR);

		fp = fopen(temp_buf2, "rb");

		if (!fp)
		{
			strncat(missing_file_list, "robots-h.mvl and/or robots-l.mvl\n", 33);
			strncat(missing_file_location_list, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE);
			strncat(missing_file_location_list, "\n", 1);
			strncat(missing_file_location_list, "and/or\n", 7);
			strncat(missing_file_location_list, temp_buf2, CHOCOLATE_MAX_FILE_PATH_SIZE);
			strncat(missing_file_location_list, "\n", 1);
		}
		else
		{
			fclose(fp);
		}
	}
	else
	{
		fclose(fp);
	}

	get_full_file_path(temp_buf, "water.pig", CHOCOLATE_SYSTEM_FILE_DIR);

	fp = fopen(temp_buf, "rb");
	if (!fp)
	{
		strncat(missing_file_list, "water.pig\n", 10);
		strncat(missing_file_location_list, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE);
		strncat(missing_file_location_list, "\n", 1);
	}
	else
	{
		fclose(fp);
	}
#endif

	if(strlen(missing_file_list) > 0)
	{
		memset(missing_file_string, 0, 65536);
		snprintf(missing_file_string, 65536, "You are missing the following required files:\n%s\nPlease place the missing files at the following locations:\n%s", missing_file_list, missing_file_location_list);
		Error(missing_file_string);
	}
}

const char* get_platform_localized_file_path_prefix()
{
#if defined(__APPLE__) && defined(__MACH__)
	if(local_file_path_prefix[0] == 0)
	{
		char chocolate_descent_directory[CHOCOLATE_MAX_FILE_PATH_SIZE];
		memset(chocolate_descent_directory, 0, CHOCOLATE_MAX_FILE_PATH_SIZE);
		snprintf(chocolate_descent_directory, CHOCOLATE_MAX_FILE_PATH_SIZE, "%s/Library/Application Support/Chocolate Descent/%s", getenv("HOME"), CHOCOLATE_DESCENT_VERSION_STRING);
		strcpy(local_file_path_prefix, chocolate_descent_directory);
	}

	return local_file_path_prefix;
#else
	return ".";
#endif
}

void get_platform_localized_interior_path(char* platform_localized_interior_path, const char* interior_path)
{
	memset(platform_localized_interior_path, 0, CHOCOLATE_MAX_FILE_PATH_SIZE);
	if (strlen(interior_path) == 0)
	{
		return;
	}

	if (PLATFORM_PATH_SEPARATOR == '/')
	{
		strncpy(platform_localized_interior_path, interior_path, CHOCOLATE_MAX_FILE_PATH_SIZE - 1);
		return;
	}

	char temp_buf[CHOCOLATE_MAX_FILE_PATH_SIZE];
	strncpy(temp_buf, interior_path, CHOCOLATE_MAX_FILE_PATH_SIZE - 1);

	char* separator_pos = strchr(temp_buf, '/');
	while (separator_pos)
	{
		*separator_pos = PLATFORM_PATH_SEPARATOR;
		separator_pos = strchr(separator_pos, '/');
	}

	strncpy(platform_localized_interior_path, temp_buf, CHOCOLATE_MAX_FILE_PATH_SIZE - 1);
}

void get_platform_localized_path(char* platform_localized_path, const char* subpath)
{
	memset(platform_localized_path, 0, CHOCOLATE_MAX_FILE_PATH_SIZE);
	get_full_file_path(platform_localized_path, "", subpath);
}

void get_platform_localized_query_string(char* platform_localized_query_string, const char* subpath, const char* query)
{
	char temp_buf[CHOCOLATE_MAX_FILE_PATH_SIZE];
	memset(platform_localized_query_string, 0, CHOCOLATE_MAX_FILE_PATH_SIZE);
	memset(temp_buf, 0, CHOCOLATE_MAX_FILE_PATH_SIZE);
	const char* separator_pos;

	separator_pos = strrchr(query, PLATFORM_PATH_SEPARATOR);
	if (separator_pos != NULL)
	{
		separator_pos++;
	}
	else
	{
		separator_pos = &query[0];
	}

	if (strlen(subpath) == 0)
	{
		strncpy(platform_localized_query_string, separator_pos, CHOCOLATE_MAX_FILE_PATH_SIZE - 1);
		return;
	}

	get_platform_localized_interior_path(temp_buf, subpath);
	snprintf(platform_localized_query_string, CHOCOLATE_MAX_FILE_PATH_SIZE, "%s%c%s", temp_buf, PLATFORM_PATH_SEPARATOR, separator_pos);
}

void get_full_file_path(char* filename_full_path, const char* filename, const char* additional_path)
{
	char temp_buf[CHOCOLATE_MAX_FILE_PATH_SIZE], platform_localized_interior_path[CHOCOLATE_MAX_FILE_PATH_SIZE];
	const char* separator_pos;
	memset(filename_full_path, 0, CHOCOLATE_MAX_FILE_PATH_SIZE);
	memset(temp_buf, 0, CHOCOLATE_MAX_FILE_PATH_SIZE);
	separator_pos = strrchr(filename, PLATFORM_PATH_SEPARATOR);
	if (separator_pos == NULL)
	{
		if (additional_path == NULL || strlen(additional_path) == 0)
		{
			snprintf(filename_full_path, CHOCOLATE_MAX_FILE_PATH_SIZE, "%s%c%s", get_platform_localized_file_path_prefix(), PLATFORM_PATH_SEPARATOR, filename);
			return;
		}
		else
		{
			get_platform_localized_interior_path(platform_localized_interior_path, additional_path);
			snprintf(filename_full_path, CHOCOLATE_MAX_FILE_PATH_SIZE, "%s%c%s%c%s", get_platform_localized_file_path_prefix(), PLATFORM_PATH_SEPARATOR, platform_localized_interior_path, PLATFORM_PATH_SEPARATOR, filename);
			return;
		}
	}
	else
	{
		strncpy(temp_buf, separator_pos + 1, CHOCOLATE_MAX_FILE_PATH_SIZE - 1);

		if (additional_path == NULL || strlen(additional_path) == 0)
		{
			snprintf(filename_full_path, CHOCOLATE_MAX_FILE_PATH_SIZE, "%s%c%s", get_platform_localized_file_path_prefix(), PLATFORM_PATH_SEPARATOR, temp_buf);
			return;
		}
		else
		{
			get_platform_localized_interior_path(platform_localized_interior_path, additional_path);
			snprintf(filename_full_path, CHOCOLATE_MAX_FILE_PATH_SIZE, "%s%c%s%c%s", get_platform_localized_file_path_prefix(), PLATFORM_PATH_SEPARATOR, platform_localized_interior_path, PLATFORM_PATH_SEPARATOR, temp_buf);
			return;
		}
	}

	snprintf(filename_full_path, CHOCOLATE_MAX_FILE_PATH_SIZE, ".%c%s", PLATFORM_PATH_SEPARATOR, filename);
	return;
}

void get_temp_file_full_path(char* filename_full_path, const char* filename)
{
	memset(filename_full_path, 0, CHOCOLATE_MAX_FILE_PATH_SIZE);
#if defined(__APPLE__) && defined(__MACH__)
	char temp_buf[CHOCOLATE_MAX_FILE_PATH_SIZE];
	const char* separator_pos;
	memset(temp_buf, 0, CHOCOLATE_MAX_FILE_PATH_SIZE);
	separator_pos = strrchr(filename, PLATFORM_PATH_SEPARATOR);
	if (separator_pos == NULL)
	{
		snprintf(filename_full_path, CHOCOLATE_MAX_FILE_PATH_SIZE, "%s%s", getenv("TMPDIR"), filename);
		return;
	}
	else
	{
		strncpy(temp_buf, separator_pos + 1, CHOCOLATE_MAX_FILE_PATH_SIZE - 1);
		snprintf(filename_full_path, CHOCOLATE_MAX_FILE_PATH_SIZE, "%s%s", getenv("TMPDIR"), temp_buf);
		return;
	}
#endif

	snprintf(filename_full_path, CHOCOLATE_MAX_FILE_PATH_SIZE, ".%c%s", PLATFORM_PATH_SEPARATOR, filename);
	return;
}

int fix_broken_descent_1_15_patch()
{
	FILE *in_fp, *out_fp, *temp_fp;
	size_t read_write_result;
	uint8_t digest[20], output_buffer[CHOCOLATE_DESCENT_HOG_15_FIXED_HOG_SIZE];
	char hexdigest[41];
	char hogfile_full_path[CHOCOLATE_MAX_FILE_PATH_SIZE];
	char hogfile_backup_full_path[CHOCOLATE_MAX_FILE_PATH_SIZE];
	char temp_file_full_path[CHOCOLATE_MAX_FILE_PATH_SIZE];
	const uint8_t chunk_1[0x02] = {static_cast<uint8_t>('\xac'), '\x03'};
	const uint8_t chunk_2[0x21] = {'\x2e', '\x68', '\x6d', '\x70', '\x09', '\x6d', '\x65', '\x6c', '\x6f', '\x64', '\x69', '\x63', '\x2e', '\x62', '\x6e', '\x6b', '\x09', '\x64', '\x72', '\x75', '\x6d', '\x2e', '\x62', '\x6e', '\x6b', '\x0d', '\x0a', '\x67', '\x61', '\x6d', '\x65', '\x30', '\x37'};
	const uint8_t chunk_3[0x0137] = {'\x38', '\x2e', '\x68', '\x6d', '\x70', '\x09', '\x72', '\x69', '\x63', '\x6b', '\x6d', '\x65', '\x6c', '\x6f', '\x2e', '\x62', '\x6e', '\x6b', '\x09', '\x72', '\x69', '\x63', '\x6b', '\x64', '\x72', '\x75', '\x6d', '\x2e', '\x62', '\x6e', '\x6b', '\x0d', '\x0a', '\x67', '\x61', '\x6d', '\x65', '\x30', '\x39', '\x2e', '\x68', '\x6d', '\x70', '\x09', '\x6d', '\x65', '\x6c', '\x6f', '\x64', '\x69', '\x63', '\x2e', '\x62', '\x6e', '\x6b', '\x09', '\x64', '\x72', '\x75', '\x6d', '\x2e', '\x62', '\x6e', '\x6b', '\x0d', '\x0a', '\x67', '\x61', '\x6d', '\x65', '\x31', '\x30', '\x2e', '\x68', '\x6d', '\x70', '\x09', '\x6d', '\x65', '\x6c', '\x6f', '\x64', '\x69', '\x63', '\x2e', '\x62', '\x6e', '\x6b', '\x09', '\x64', '\x72', '\x75', '\x6d', '\x2e', '\x62', '\x6e', '\x6b', '\x0d', '\x0a', '\x67', '\x61', '\x6d', '\x65', '\x31', '\x31', '\x2e', '\x68', '\x6d', '\x70', '\x09', '\x69', '\x6e', '\x74', '\x6d', '\x65', '\x6c', '\x6f', '\x2e', '\x62', '\x6e', '\x6b', '\x09', '\x69', '\x6e', '\x74', '\x64', '\x72', '\x75', '\x6d', '\x2e', '\x62', '\x6e', '\x6b', '\x0d', '\x0a', '\x67', '\x61', '\x6d', '\x65', '\x31', '\x32', '\x2e', '\x68', '\x6d', '\x70', '\x09', '\x6d', '\x65', '\x6c', '\x6f', '\x64', '\x69', '\x63', '\x2e', '\x62', '\x6e', '\x6b', '\x09', '\x64', '\x72', '\x75', '\x6d', '\x2e', '\x62', '\x6e', '\x6b', '\x0d', '\x0a', '\x67', '\x61', '\x6d', '\x65', '\x31', '\x33', '\x2e', '\x68', '\x6d', '\x70', '\x09', '\x69', '\x6e', '\x74', '\x6d', '\x65', '\x6c', '\x6f', '\x2e', '\x62', '\x6e', '\x6b', '\x09', '\x69', '\x6e', '\x74', '\x64', '\x72', '\x75', '\x6d', '\x2e', '\x62', '\x6e', '\x6b', '\x0d', '\x0a', '\x67', '\x61', '\x6d', '\x65', '\x31', '\x34', '\x2e', '\x68', '\x6d', '\x70', '\x09', '\x69', '\x6e', '\x74', '\x6d', '\x65', '\x6c', '\x6f', '\x2e', '\x62', '\x6e', '\x6b', '\x09', '\x69', '\x6e', '\x74', '\x64', '\x72', '\x75', '\x6d', '\x2e', '\x62', '\x6e', '\x6b', '\x0d', '\x0a', '\x67', '\x61', '\x6d', '\x65', '\x31', '\x35', '\x2e', '\x68', '\x6d', '\x70', '\x09', '\x6d', '\x65', '\x6c', '\x6f', '\x64', '\x69', '\x63', '\x2e', '\x62', '\x6e', '\x6b', '\x09', '\x64', '\x72', '\x75', '\x6d', '\x2e', '\x62', '\x6e', '\x6b', '\x0d', '\x0a', '\x67', '\x61', '\x6d', '\x65', '\x31', '\x36', '\x2e', '\x68', '\x6d', '\x70', '\x09', '\x6d', '\x65', '\x6c', '\x6f', '\x64', '\x69', '\x63', '\x2e', '\x62', '\x6e', '\x6b', '\x09', '\x64', '\x72', '\x75', '\x6d', '\x2e', '\x62', '\x6e', '\x6b', '\x0d', '\x0a', '\x67', '\x61', '\x6d', '\x65', '\x31'};
	const uint8_t chunk_4[0xae] = {'\x64', '\x72', '\x75', '\x6d', '\x2e', '\x62', '\x6e', '\x6b', '\x0d', '\x0a', '\x67', '\x61', '\x6d', '\x65', '\x31', '\x38', '\x2e', '\x68', '\x6d', '\x70', '\x09', '\x69', '\x6e', '\x74', '\x6d', '\x65', '\x6c', '\x6f', '\x2e', '\x62', '\x6e', '\x6b', '\x09', '\x69', '\x6e', '\x74', '\x64', '\x72', '\x75', '\x6d', '\x2e', '\x62', '\x6e', '\x6b', '\x0d', '\x0a', '\x67', '\x61', '\x6d', '\x65', '\x31', '\x39', '\x2e', '\x68', '\x6d', '\x70', '\x09', '\x6d', '\x65', '\x6c', '\x6f', '\x64', '\x69', '\x63', '\x2e', '\x62', '\x6e', '\x6b', '\x09', '\x64', '\x72', '\x75', '\x6d', '\x2e', '\x62', '\x6e', '\x6b', '\x0d', '\x0a', '\x67', '\x61', '\x6d', '\x65', '\x32', '\x30', '\x2e', '\x68', '\x6d', '\x70', '\x09', '\x6d', '\x65', '\x6c', '\x6f', '\x64', '\x69', '\x63', '\x2e', '\x62', '\x6e', '\x6b', '\x09', '\x64', '\x72', '\x75', '\x6d', '\x2e', '\x62', '\x6e', '\x6b', '\x0d', '\x0a', '\x67', '\x61', '\x6d', '\x65', '\x32', '\x31', '\x2e', '\x68', '\x6d', '\x70', '\x09', '\x69', '\x6e', '\x74', '\x6d', '\x65', '\x6c', '\x6f', '\x2e', '\x62', '\x6e', '\x6b', '\x09', '\x69', '\x6e', '\x74', '\x64', '\x72', '\x75', '\x6d', '\x2e', '\x62', '\x6e', '\x6b', '\x0d', '\x0a', '\x67', '\x61', '\x6d', '\x65', '\x32', '\x32', '\x2e', '\x68', '\x6d', '\x70', '\x09', '\x68', '\x61', '\x6d', '\x6d', '\x65', '\x6c', '\x6f', '\x2e', '\x62', '\x6e', '\x6b', '\x09', '\x68', '\x61', '\x6d'};

	get_full_file_path(hogfile_full_path, "descent.hog", CHOCOLATE_SYSTEM_FILE_DIR);
	get_full_file_path(hogfile_backup_full_path, "descent.hog.bak", CHOCOLATE_SYSTEM_FILE_DIR);

	printf("Attempting to patch %s to a fixed version of 1.5\n", hogfile_full_path);

	in_fp = fopen(hogfile_full_path, "rb");
	if (in_fp == NULL)
	{
		printf("Unable to read %s for patching\n", hogfile_full_path);
		return 0;
	}

	read_write_result = fread(output_buffer, 1, CHOCOLATE_DESCENT_HOG_15_BROKEN_HOG_SIZE, in_fp);

	if (read_write_result != CHOCOLATE_DESCENT_HOG_15_BROKEN_HOG_SIZE)
	{
		printf("Didn't read expected amount of data from %s\n", hogfile_full_path);
		fclose(in_fp);
		return 0;
	}

	fclose(in_fp);

	if (sha1digest(digest, hexdigest, output_buffer, CHOCOLATE_DESCENT_HOG_15_BROKEN_HOG_SIZE))
	{
		printf("Unable to calculate SHA1 digest for %s\n", hogfile_full_path);
		return 0;
	}

	if (strncmp(hexdigest, "19659f7dc0a464b2778d1274cd136a7a4f02c0ca", 40) != 0)
	{
		printf("Invalid digest for %s: %s\n", hogfile_full_path, hexdigest);
		return 0;
	}

	memset(output_buffer, 0, CHOCOLATE_DESCENT_HOG_15_FIXED_HOG_SIZE);

	in_fp = fopen(hogfile_full_path, "rb");
	if (in_fp == NULL)
	{
		printf("Unable to read %s for patching\n", hogfile_full_path);
		return 0;
	}

	read_write_result = fread(output_buffer, 1, 0x0f5e5a, in_fp);

	if (read_write_result != 0x0f5e5a)
	{
		printf("Didn't read expected amount of data from %s\n", hogfile_full_path);
		fclose(in_fp);
		return 0;
	}

	memcpy(&output_buffer[0x0f5e5a], chunk_1, 0x02);

	if (fseek(in_fp, 2, SEEK_CUR) != 0)
	{
		printf("Error seeking in %s\n", hogfile_full_path);
		return 0;
	}

	read_write_result = fread(&output_buffer[0x0f5e5c], 1, 0x016b, in_fp);

	if (read_write_result != 0x016b)
	{
		printf("Didn't read expected amount of data from %s\n", hogfile_full_path);
		fclose(in_fp);
		return 0;
	}

	memcpy(&output_buffer[0x0f5fc7], chunk_2, 0x21);

	read_write_result = fread(&output_buffer[0x0f5fe8], 1, 0x20, in_fp);

	if (read_write_result != 0x20)
	{
		printf("Didn't read expected amount of data from %s\n", hogfile_full_path);
		fclose(in_fp);
		return 0;
	}

	memcpy(&output_buffer[0x0f6008],chunk_3, 0x0137);

	read_write_result = fread(&output_buffer[0x0f613f], 1, 0x12, in_fp);

	if (read_write_result != 0x12)
	{
		printf("Didn't read expected amount of data from %s\n", hogfile_full_path);
		fclose(in_fp);
		return 0;
	}

	memcpy(&output_buffer[0x0f6151], chunk_4, 0xae);

	read_write_result = fread(&output_buffer[0x0f61ff], 1, 0x593dfe, in_fp);

	if (read_write_result != 0x593dfe)
	{
		printf("Didn't read expected amount of data from %s\n", hogfile_full_path);
		fclose(in_fp);
		return 0;
	}

	fclose(in_fp);

	if (sha1digest(digest, hexdigest, output_buffer, CHOCOLATE_DESCENT_HOG_15_FIXED_HOG_SIZE))
	{
		printf("Unable to calculate SHA1 sum for patched copy of %s\n", hogfile_full_path);
		return 0;
	}

	if (strncmp(hexdigest, "4d6fb40e943f92574aba2c9fc1574330de89905b", 40) != 0)
	{
		printf("Invalid digest for patched copy of %s: %s\n", hogfile_full_path, hexdigest);
		return 0;
	}

	get_temp_file_full_path(temp_file_full_path, "descent.hog");

	_unlink(temp_file_full_path);

	out_fp = fopen(temp_file_full_path, "wb");
	if (out_fp == NULL)
	{
		printf("Unable to open temp file for writing at %s\n", temp_file_full_path);
		return 0;
	}

	read_write_result = fwrite(output_buffer, 1, CHOCOLATE_DESCENT_HOG_15_FIXED_HOG_SIZE, out_fp);

	if (read_write_result != CHOCOLATE_DESCENT_HOG_15_FIXED_HOG_SIZE)
	{
		printf("Didn't write expected amount of data to temp file at %s\n", temp_file_full_path);
		fclose(out_fp);
		return 0;
	}

	fclose(out_fp);

	if (rename(hogfile_full_path, hogfile_backup_full_path) != 0)
	{
		printf("Unable to back up existing file from %s to %s\n", hogfile_full_path, hogfile_backup_full_path);
		return 0;
	}

	if (rename(temp_file_full_path, hogfile_full_path) != 0)
	{
		printf("Unable to write patched file to %s", hogfile_full_path);
		if(rename(hogfile_backup_full_path, hogfile_full_path) != 0)
		{
			printf("Unable to restore backup file from %s to %s\nPlease rename the file manually.", hogfile_backup_full_path, hogfile_full_path);
		}

		return 0;
	}

	printf("Successuflly patched %s to a fixed version of 1.5\n", hogfile_full_path);

	return 1;
}

int patch_descent_1_14_to_15()
{
	FILE *in_fp, *out_fp, *temp_fp;
	size_t read_write_result;
	uint8_t digest[20], output_buffer[CHOCOLATE_DESCENT_HOG_15_FIXED_HOG_SIZE];
	char hexdigest[41];
	char hogfile_full_path[CHOCOLATE_MAX_FILE_PATH_SIZE];
	char hogfile_backup_full_path[CHOCOLATE_MAX_FILE_PATH_SIZE];
	char temp_file_full_path[CHOCOLATE_MAX_FILE_PATH_SIZE];
	const uint8_t chunk_1 = '\x16';

	get_full_file_path(hogfile_full_path, "descent.hog", CHOCOLATE_SYSTEM_FILE_DIR);
	get_full_file_path(hogfile_backup_full_path, "descent.hog.bak", CHOCOLATE_SYSTEM_FILE_DIR);

	printf("Attempting to patch %s to 1.5\n", hogfile_full_path);

	// TODO: Validate SHA1 checksum to verify that the file to modify currently
	//       has a sum of c4a003c11a62db61465abe0e20065f5cf97697c4

	in_fp = fopen(hogfile_full_path, "rb");
	if (in_fp == NULL)
	{
		printf("Unable to read %s for patching\n", hogfile_full_path);
		return 0;
	}

	read_write_result = fread(output_buffer, 1, CHOCOLATE_DESCENT_HOG_15_FIXED_HOG_SIZE, in_fp);

	if (read_write_result != CHOCOLATE_DESCENT_HOG_15_FIXED_HOG_SIZE)
	{
		printf("Didn't read expected amount of data from %s\n", hogfile_full_path);
		fclose(in_fp);
		return 0;
	}

	fclose(in_fp);

	if (sha1digest(digest, hexdigest, output_buffer, CHOCOLATE_DESCENT_HOG_15_FIXED_HOG_SIZE))
	{
		printf("Unable to calculate SHA1 digest for %s\n", hogfile_full_path);
		return 0;
	}

	if (strncmp(hexdigest, "c4a003c11a62db61465abe0e20065f5cf97697c4", 40) != 0)
	{
		printf("Invalid digest for %s: %s\n", hogfile_full_path, hexdigest);
		return 0;
	}

	memset(output_buffer, 0, CHOCOLATE_DESCENT_HOG_15_FIXED_HOG_SIZE);

	in_fp = fopen(hogfile_full_path, "rb");
	if (in_fp == NULL)
	{
		printf("Unable to read %s for patching\n", hogfile_full_path);
		return 0;
	}

	read_write_result = fread(output_buffer, 1, 0x689ffd, in_fp);

	if (read_write_result != 0x689ffd)
	{
		printf("Didn't read expected amount of data from %s\n", hogfile_full_path);
		fclose(in_fp);
		return 0;
	}

	output_buffer[0x0f] = chunk_1;

	fclose(in_fp);

	if (sha1digest(digest, hexdigest, output_buffer, CHOCOLATE_DESCENT_HOG_15_FIXED_HOG_SIZE))
	{
		printf("Unable to calculate SHA1 sum for patched copy of %s\n", hogfile_full_path);
		return 0;
	}

	if (strncmp(hexdigest, "4d6fb40e943f92574aba2c9fc1574330de89905b", 40) != 0)
	{
		printf("Invalid digest for patched copy of %s: %s\n", hogfile_full_path, hexdigest);
		return 0;
	}

	get_temp_file_full_path(temp_file_full_path, "descent.hog");

	_unlink(temp_file_full_path);

	out_fp = fopen(temp_file_full_path, "wb");
	if (out_fp == NULL)
	{
		printf("Unable to open temp file for writing at %s\n", temp_file_full_path);
		return 0;
	}

	read_write_result = fwrite(output_buffer, 1, CHOCOLATE_DESCENT_HOG_15_FIXED_HOG_SIZE, out_fp);

	if (read_write_result != CHOCOLATE_DESCENT_HOG_15_FIXED_HOG_SIZE)
	{
		printf("Didn't write expected amount of data to temp file at %s\n", temp_file_full_path);
		fclose(out_fp);
		return 0;
	}

	fclose(out_fp);

	if (rename(hogfile_full_path, hogfile_backup_full_path) != 0)
	{
		printf("Unable to back up existing file from %s to %s\n", hogfile_full_path, hogfile_backup_full_path);
		return 0;
	}

	if (rename(temp_file_full_path, hogfile_full_path) != 0)
	{
		printf("Unable to write patched file to %s", hogfile_full_path);
		if(rename(hogfile_backup_full_path, hogfile_full_path) != 0)
		{
			printf("Unable to restore backup file from %s to %s\nPlease rename the file manually.", hogfile_backup_full_path, hogfile_full_path);
		}

		return 0;
	}

	printf("Successfully patched %s to 1.5\n", hogfile_full_path);

	return 1;
}

int patch_descent_2_11_to_12()
{
	FILE *in_fp, *out_fp, *temp_fp;
	size_t read_write_result;
	uint8_t output_buffer[CHOCOLATE_DESCENT_2_HOG_12_HOG_SIZE];
	char hogfile_full_path[CHOCOLATE_MAX_FILE_PATH_SIZE];
	char hogfile_backup_full_path[CHOCOLATE_MAX_FILE_PATH_SIZE];
	char temp_file_full_path[CHOCOLATE_MAX_FILE_PATH_SIZE];
	const uint8_t chunk_1 = static_cast<uint8_t>('\x8c');
	const uint8_t chunk_2[0x28] = {'\x0a', '\x0a', '\x63', '\x3c', static_cast<uint8_t>('\xb3'), '\x72', static_cast<uint8_t>('\xf0'), '\x32', '\x34', '\x35', static_cast<uint8_t>('\xe1'), static_cast<uint8_t>('\xa7'), static_cast<uint8_t>('\xa4'), static_cast<uint8_t>('\xe1'), static_cast<uint8_t>('\xbb'), static_cast<uint8_t>('\xb2'), static_cast<uint8_t>('\xf5'), static_cast<uint8_t>('\xf2'), static_cast<uint8_t>('\xb0'), static_cast<uint8_t>('\xb2'), static_cast<uint8_t>('\xb0'), static_cast<uint8_t>('\x72'), static_cast<uint8_t>('\xf4'), static_cast<uint8_t>('\xb1'), static_cast<uint8_t>('\xf4'), static_cast<uint8_t>('\xb3'), '\x32', '\x72', '\x0a', '\x3d', static_cast<uint8_t>('\xb1'), static_cast<uint8_t>('\xb2'), static_cast<uint8_t>('\xb3'), '\x75', static_cast<uint8_t>('\xe1'), '\x3d', static_cast<uint8_t>('\xb3'), '\x72', static_cast<uint8_t>('\xf3'), static_cast<uint8_t>('\xb1')};

	get_full_file_path(hogfile_full_path, "descent2.hog", CHOCOLATE_SYSTEM_FILE_DIR);
	get_full_file_path(hogfile_backup_full_path, "descent2.hog.bak", CHOCOLATE_SYSTEM_FILE_DIR);

	// TODO: Validate SHA1 checksum to verify that the file to modify currently
	//       has a sum of 7a17a70ad6a99bb78fb2383a9eca40d06e3e9b45

	in_fp = fopen(hogfile_full_path, "rb");
	if (in_fp == NULL)
	{
		printf("Unable to read %s for patching\n", hogfile_full_path);
		return 0;
	}

	read_write_result = fread(output_buffer, 1, 0x3a6a, in_fp);

	if (read_write_result != 0x3a6a)
	{
		printf("Didn't read expected amount of data from %s\n", hogfile_full_path);
		fclose(in_fp);
		return 0;
	}

	output_buffer[0x3991] = chunk_1;

	memcpy(&output_buffer[0x3a6b], chunk_2, 0x28);

	read_write_result = fread(&output_buffer[0x3a93], 1, 0x73a9b5, in_fp);

	if (read_write_result != 0x73a9b5)
	{
		printf("Didn't read expected amount of data from %s\n", hogfile_full_path);
		fclose(in_fp);
		return 0;
	}

	fclose(in_fp);

	get_temp_file_full_path(temp_file_full_path, "descent2.hog");

	_unlink(temp_file_full_path);

	out_fp = fopen(temp_file_full_path, "wb");
	if (out_fp == NULL)
	{
		printf("Unable to open temp file for writing at %s\n", temp_file_full_path);
		return 0;
	}

	read_write_result = fwrite(output_buffer, 1, CHOCOLATE_DESCENT_2_HOG_12_HOG_SIZE, out_fp);

	if (read_write_result != CHOCOLATE_DESCENT_2_HOG_12_HOG_SIZE)
	{
		printf("Didn't write expected amount of data to temp file at %s\n", temp_file_full_path);
		fclose(out_fp);
		return 0;
	}

	fclose(out_fp);

	// TODO: Validate SHA1 checksum to verify that the file written to the temp
	//       location has a sum of 8e73beb4b64a7222fa2b5214282d5e671b313992

	if (rename(hogfile_full_path, hogfile_backup_full_path) != 0)
	{
		printf("Unable to back up existing file from %s to %s\n", hogfile_full_path, hogfile_backup_full_path);
		return 0;
	}

	if (rename(temp_file_full_path, hogfile_full_path) != 0)
	{
		printf("Unable to write patched file to %s", hogfile_full_path);
		if(rename(hogfile_backup_full_path, hogfile_full_path) != 0)
		{
			printf("Unable to restore backup file from %s to %s\nPlease rename the file manually.", hogfile_backup_full_path, hogfile_full_path);
		}

		return 0;
	}

	return 1;
}