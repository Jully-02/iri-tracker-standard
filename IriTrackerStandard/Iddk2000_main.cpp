#include "Iddk2000_main.h"

static void get_module_dir(const char *pModulePath, char *pDirectory)
{
	unsigned int dwSize = 0;
	char szModuleFile[256] = "";
	char* pTemp = NULL;
	strcpy(pDirectory, "");
	strcpy(szModuleFile, pModulePath);

	if(strlen(szModuleFile) == 0)
	{		
		return;
	}
	pTemp = strrchr(szModuleFile, ('\\'));
	if(!pTemp)
	{
		pTemp = strrchr(szModuleFile, ('/'));
	}
	if(!pTemp)
	{		
		strcpy(pDirectory, "");
	}
	else
	{	
		*pTemp = 0;
		strcpy(pDirectory, szModuleFile);
	}
}

#if defined(WINCE)
int _tmain(int argc, TCHAR *argv[])
{
	char pArgvBuffer[256] = {0};
	if(argv != NULL)
	{
		wcstombs(pArgvBuffer, argv[0], 255);
	}

	/* obtain the module directory */
	get_module_dir(pArgvBuffer, g_binDir);
	print_menu();
	return SUCCESS;
}
#else
//int main(int argc, char** argv)
//{
//	/* obtain the module directory */
//	get_module_dir(argv[0], g_binDir);
//	print_menu();
//	return SUCCESS;
//}
#endif