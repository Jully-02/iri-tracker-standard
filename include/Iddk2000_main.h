#include <stdio.h>
#include <stdlib.h>

#include "Iddk2000_features.h"

_menu_list main_menu ={
	"Login",
	"Logout",
	"Device Management",
	"Device & SDK Information",
	"Capturing Process",
	"Iris Recognition",
	"Power Management",
	"Recovery (IriShield USB only)",
	"Capturing Process Custom",
	"Exit"
};

const char* introduction = "\tThis demonstration shows how to use the IDDK 2000 library in a basic \n\tmanner. This demonstration only works with the IriShield devices.\n";

static void iritech_guide(const char* message)
{
	printf("*************************************************************************\n");
	printf("Usage:\n");
	printf(introduction);
	printf("\n\tCopyright (C) 2012 by IriTech, Inc. All rights reserved.");
	printf("\n*************************************************************************\n");
	printf("Please press ENTER to continue ...\n");
	getchar();
}

static void print_menu()
{
	int option = -1;

	iritech_guide(introduction);

	get_device_handle_custom();

	if(g_hDevice == NULL) 
	{
		printf("Press any key to exit !\n");
		getchar();
		return;
	}

	while(true)
	{
		printf("\nMAIN MENU: Select one of the features below\n");
		option = display_menu(main_menu, sizeof(main_menu)/sizeof(main_menu[0]), -1);
	
		if(option != 9 && option != -1)
		{
			printf("\n\n*************** %s ***************\n\n", main_menu[option - 1]);
		}

		switch(option)
		{
		case -1:
			/* User presses enter */
			break;
		case 1:
			login();
			break;
		case 2:
			logout();
			break;
		case 3:
			device_configuration();
			break;
		case 4:
			get_information();
			break;
		case 5:			
			capturing_process(false, true, true);
			break;
		case 6:
			iris_recognition();
			break;
		case 7: 
			power_management();
			break;
		case 8:
			recovery();
			break;
		case 9:
			capturing_process_custom(false, true, true);
			break;
		case 10:
			goto RETSEC;
		}

		if(option != 9 && option != -1)
		{
			printf("\n\n***********************************************\n\n");
		}
	}

RETSEC:
	/* Last try to close device before exiting */
	finalize_device();
}
