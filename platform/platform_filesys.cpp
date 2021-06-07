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

// Sizes of supported files for which that's relevant
#define CHOCOLATE_DESCENT_HOG_15_FIXED_HOG_SIZE 6856701

// Sizes of files which can be patched to supported files
#define CHOCOLATE_DESCENT_HOG_14_HOG_SIZE 6856701
#define CHOCOLATE_DESCENT_HOG_15_BROKEN_HOG_SIZE 6856183
#define CHOCOLATE_DESCENT_2_HOG_11_HOG_SIZE 7595079
#define CHOCOLATE_DESCENT_2_HOG_12_HOG_SIZE 7595079

// Hashes of supported files
#define CHOCOLATE_DESCENT_HOG_15_FIXED_SHA1 "4d6fb40e943f92574aba2c9fc1574330de89905b"
#define CHOCOLATE_DESCENT_PIG_15_SHA1 "53f6453d5a836cf660c6dcca0f636d46a28ee20c"
#define CHOCOLATE_ALIEN1_PIG_12_SHA1 "80e040199b347f8f17f31afb32d551113cf40956"
#define CHOCOLATE_ALIEN2_PIG_12_SHA1 "2bbb8d941f8b06b56d5ddc7f723a206907a95548"
#define CHOCOLATE_D2_2PLYR_HOG_12_SHA1 "15767e36f12ed8667249e19e948686c13dd5d90b"
#define CHOCOLATE_D2_2PLYR_MN2_12_SHA1 "db13b038e5b3b8ccbc6f2418bbe49dc0bac87235"
#define CHOCOLATE_D2CHAOS_HOG_12_SHA1 "156f66ae7ae48fff582116bdf0e91e0f3386b3af"
#define CHOCOLATE_D2CHAOS_MN2_12_SHA1 "bf266256dc726852bad4896861a1dfad4022f8b1"
#define CHOCOLATE_D2X_HOG_12_SHA1 "c6ad7c7f5b052b1be346edebdcd1673d5797755b"
#define CHOCOLATE_D2X_MN2_12_SHA1 "4951910d16e1765c647acfafb077c0d8063e4473"
#define CHOCOLATE_D2X_H_MVL_12_SHA1 "e04995cd04bd75024156b094968f84ab8dc23c7a"
#define CHOCOLATE_D2X_L_MVL_12_SHA1 "2d391af492f86f285ad0126a7acb4501921e5545"
#define CHOCOLATE_DESCENT2_HAM_12_SHA1 "9f621706cbd601bd6938a44f5fb7e5cc55d4b61f"
#define CHOCOLATE_DESCENT2_HOG_12_SHA1 "8e73beb4b64a7222fa2b5214282d5e671b313992"
#define CHOCOLATE_DESCENT2_S11_12_SHA1 "72372b2fc9f88553fae6e26aba34a7c433322051"
#define CHOCOLATE_DESCENT2_S22_12_SHA1 "f67d22b5aa2e06b286719e063b4e054169a765b1"
#define CHOCOLATE_FIRE_PIG_12_SHA1 "6365ed9256411e883189e7c2149671f9de24526b"
#define CHOCOLATE_GROUPA_PIG_12_SHA1 "fc3dd5059ff445986d05c143c54284bd7ec2ee68"
#define CHOCOLATE_HOARD_HAM_12_SHA1 "9c68072b154d7e135bd8beb0a544cd4da4cde603"
#define CHOCOLATE_ICE_PIG_12_SHA1 "f3df0545359e9f2a7f58a4dd07615bd87cce10bf"
#define CHOCOLATE_INTRO_H_MVL_12_SHA1 "9614aac967aa9f2efe10af553e506e27bb91e5ba"
#define CHOCOLATE_INTRO_L_MVL_12_SHA1 "fe8f973ec54ae527c8dd205dc4e0027fbe4a87df"
#define CHOCOLATE_OTHER_H_MVL_12_SHA1 "e8f1a2d643feb2731fc05974ab202c4cd3aea0fd"
#define CHOCOLATE_OTHER_L_MVL_12_SHA1 "234c5b7810b5a8a820baa755dc634f993c6a7af9"
#define CHOCOLATE_PANIC_HOG_12_SHA1 "90bbf31be2e4f220a4ac623c48b48a10eb89e448"
#define CHOCOLATE_PANIC_MN2_12_SHA1 "830ab8aed668b3485e17c08afa24fc55dd7cc56c"
#define CHOCOLATE_ROBOTS_H_MVL_12_SHA1 "c130ecda730b5ce111947308a33b7911759fb3a5"
#define CHOCOLATE_ROBOTS_L_MVL_12_SHA1 "42be364b135bb2c3497ca39839533bde49c08644"
#define CHOCOLATE_WATER_PIG_12_SHA1 "1874f70353680a803811a5ff2cce949e3f5d5076"

// Hashes of files which can be patched to supported files
#define CHOCOLATE_DESCENT_HOG_14_SHA1 "c4a003c11a62db61465abe0e20065f5cf97697c4"
#define CHOCOLATE_DESCENT_HOG_15_BROKEN_SHA1 "19659f7dc0a464b2778d1274cd136a7a4f02c0ca"
#define CHOCOLATE_DESCENT2_HOG_11_SHA1 "7a17a70ad6a99bb78fb2383a9eca40d06e3e9b45"

// Hashes of unsupported files
#define CHOCOLATE_DESCENT_HOG_10_SHA1 "d00b712206e84f6cbd874e271aa4d7ee7c2750e2"
#define CHOCOLATE_DESCENT_PIG_10_SHA1 "24d24e69f15d054774f40d0882c0d4a439fea26c"
#define CHOCOLATE_DESCENT_HOG_MACPLAY_SHA1 "c276203ebcca1939b8172246fbf5cd75fcbc14d3"
#define CHOCOLATE_DESCENT_PIG_MACPLAY_SHA1 "eb483fcad4905a7dfb0fd26477bd7215452c9890"
#define CHOCOLATE_D2_2PLYR_MN2_MACPLAY_12_SHA1 "72d53b6830657e6bbbbae2e510cab327673efd2f"
#define CHOCOLATE_D2CHAOS_MN2_MACPLAY_12_SHA1 "395074cf458b8fce9941168910f10e5cfee3c9b8"
#define CHOCOLATE_DESCENT2_HAM_10_SHA1 "0ff00307a3a319a7b876625b6270d84c95b32148"
#define CHOCOLATE_DESCENT2_HOG_10_SHA1 "4e818336d48c44e02899ec536a601b3fc6903c9a"
#define CHOCOLATE_ALIEN1_PIG_MACPLAY_12_SHA1 "bf0c852c836f89d5c3cc8c0508346c3ea07d35b1"
#define CHOCOLATE_ALIEN2_PIG_MACPLAY_12_SHA1 "51acda0ff9449304c9381352ed0f4ed8a3d4849a"
#define CHOCOLATE_DESCENT2_HOG_MACPLAY_12_SHA1 "3aa96efd12578740381f0398ed1d964b1d607400"
#define CHOCOLATE_FIRE_PIG_MACPLAY_12_SHA1 "75546a51e5f889047fc6909ffe7c8d20ca20db44"
#define CHOCOLATE_GROUPA_PIG_MACPLAY_12_SHA1 "5a2d7e67587af79311ad3fbef4d47a15e01980d6"
#define CHOCOLATE_ICE_PIG_MACPLAY_12_SHA1 "911e31273b15ff93589b7da0c32e51d1d7130654"
#define CHOCOLATE_INTRO_MVL_MACPLAY_12_SHA1 "8dc96ab3daaaf40aeb5971dafc6c63b60e198e32"
#define CHOCOLATE_OTHER_MVL_MACPLAY_12_SHA1 "d000f21afb3a89af8b35bc7747956237c26140fa"
#define CHOCOLATE_ROBOTS_MVL_MACPLAY_12_SHA1 "8e4daa890fb92d1b695a13064748eef87e50a16b"
#define CHOCOLATE_WATER_PIG_MACPLAY_12_SHA1 "294322fd1c56244e8eba0950aff74014f6c55f7f"

extern int sha1digest(uint8_t *digest, char *hexdigest, const uint8_t *data, size_t databytes);
static char local_file_path_prefix[CHOCOLATE_MAX_FILE_PATH_SIZE] = {0};

void get_missing_file_locations(char* missing_file_string, const char* missing_file_list);

int fix_broken_descent_1_15_patch();
int patch_descent_1_14_to_15();
int patch_descent_2_11_to_12();

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

	if (strncmp(hexdigest, CHOCOLATE_DESCENT_HOG_15_BROKEN_SHA1, 40) != 0)
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

	if (strncmp(hexdigest, CHOCOLATE_DESCENT_HOG_15_FIXED_SHA1, 40) != 0)
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

	printf("Successuflly patched %s to a fixed version of 1.5\nYour original descent.hog file can be found at %s\n", hogfile_full_path, hogfile_backup_full_path);

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

	in_fp = fopen(hogfile_full_path, "rb");
	if (in_fp == NULL)
	{
		printf("Unable to read %s for patching\n", hogfile_full_path);
		return 0;
	}

	read_write_result = fread(output_buffer, 1, CHOCOLATE_DESCENT_HOG_14_HOG_SIZE, in_fp);

	if (read_write_result != CHOCOLATE_DESCENT_HOG_14_HOG_SIZE)
	{
		printf("Didn't read expected amount of data from %s\n", hogfile_full_path);
		fclose(in_fp);
		return 0;
	}

	fclose(in_fp);

	if (sha1digest(digest, hexdigest, output_buffer, CHOCOLATE_DESCENT_HOG_14_HOG_SIZE))
	{
		printf("Unable to calculate SHA1 digest for %s\n", hogfile_full_path);
		return 0;
	}

	if (strncmp(hexdigest, CHOCOLATE_DESCENT_HOG_14_SHA1, 40) != 0)
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

	if (strncmp(hexdigest, CHOCOLATE_DESCENT_HOG_15_FIXED_SHA1, 40) != 0)
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

	printf("Successfully patched %s to 1.5\nYour original descent.hog file can be found at %s\n", hogfile_full_path, hogfile_backup_full_path);

	return 1;
}

int patch_descent_2_11_to_12()
{
	FILE *in_fp, *out_fp, *temp_fp;
	size_t read_write_result;
	uint8_t digest[20], output_buffer[CHOCOLATE_DESCENT_2_HOG_12_HOG_SIZE];
	char hexdigest[41];
	char hogfile_full_path[CHOCOLATE_MAX_FILE_PATH_SIZE];
	char hogfile_backup_full_path[CHOCOLATE_MAX_FILE_PATH_SIZE];
	char temp_file_full_path[CHOCOLATE_MAX_FILE_PATH_SIZE];
	const uint8_t chunk_1 = static_cast<uint8_t>('\x8c');
	const uint8_t chunk_2[0x28] = {'\x0a', '\x0a', '\x63', '\x3c', static_cast<uint8_t>('\xb3'), '\x72', static_cast<uint8_t>('\xf0'), '\x32', '\x34', '\x35', static_cast<uint8_t>('\xe1'), static_cast<uint8_t>('\xa7'), static_cast<uint8_t>('\xa4'), static_cast<uint8_t>('\xe1'), static_cast<uint8_t>('\xbb'), static_cast<uint8_t>('\xb2'), static_cast<uint8_t>('\xf5'), static_cast<uint8_t>('\xf2'), static_cast<uint8_t>('\xb0'), static_cast<uint8_t>('\xb2'), static_cast<uint8_t>('\xb0'), static_cast<uint8_t>('\x72'), static_cast<uint8_t>('\xf4'), static_cast<uint8_t>('\xb1'), static_cast<uint8_t>('\xf4'), static_cast<uint8_t>('\xb3'), '\x32', '\x72', '\x0a', '\x3d', static_cast<uint8_t>('\xb1'), static_cast<uint8_t>('\xb2'), static_cast<uint8_t>('\xb3'), '\x75', static_cast<uint8_t>('\xe1'), '\x3d', static_cast<uint8_t>('\xb3'), '\x72', static_cast<uint8_t>('\xf3'), static_cast<uint8_t>('\xb1')};

	get_full_file_path(hogfile_full_path, "descent2.hog", CHOCOLATE_SYSTEM_FILE_DIR);
	get_full_file_path(hogfile_backup_full_path, "descent2.hog.bak", CHOCOLATE_SYSTEM_FILE_DIR);

	in_fp = fopen(hogfile_full_path, "rb");
	if (in_fp == NULL)
	{
		printf("Unable to read %s for patching\n", hogfile_full_path);
		return 0;
	}

	read_write_result = fread(output_buffer, 1, CHOCOLATE_DESCENT_2_HOG_11_HOG_SIZE, in_fp);

	if (read_write_result != CHOCOLATE_DESCENT_2_HOG_11_HOG_SIZE)
	{
		printf("Didn't read expected amount of data from %s\n", hogfile_full_path);
		fclose(in_fp);
		return 0;
	}

	fclose(in_fp);

	if (sha1digest(digest, hexdigest, output_buffer, CHOCOLATE_DESCENT_2_HOG_11_HOG_SIZE))
	{
		printf("Unable to calculate SHA1 digest for %s\n", hogfile_full_path);
		return 0;
	}

	if (strncmp(hexdigest, CHOCOLATE_DESCENT2_HOG_11_SHA1, 40) != 0)
	{
		printf("Invalid digest for %s: %s\n", hogfile_full_path, hexdigest);
		return 0;
	}

	memset(output_buffer, 0, CHOCOLATE_DESCENT_2_HOG_12_HOG_SIZE);

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

	if (sha1digest(digest, hexdigest, output_buffer, CHOCOLATE_DESCENT_2_HOG_12_HOG_SIZE))
	{
		printf("Unable to calculate SHA1 sum for patched copy of %s\n", hogfile_full_path);
		return 0;
	}

	if (strncmp(hexdigest, CHOCOLATE_DESCENT2_HOG_12_SHA1, 40) != 0)
	{
		printf("Invalid digest for patched copy of %s: %s\n", hogfile_full_path, hexdigest);
		return 0;
	}

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