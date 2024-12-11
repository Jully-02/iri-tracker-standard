#include "Iddk2000_utils.h"
#include <QString>

#define ON		true		/* Device is on */
#define OFF		false		/* Device is off */

extern HIRICAMM g_hDevice;	/* Device handle */
extern char g_binDir[256];  /* assembly directory */

#define RIGHT_EYE_IDX 0
#define LEFT_EYE_IDX  1

////////////////////////////////////////////////

void login();

void logout();

void device_configuration();

void get_information();

void get_device_handle();

void get_device_handle_custom();

void capturing_process(bool bDefaultParams, bool bMultiple, bool bProcessResult);

void capturing_process_custom(bool bDefaultParams, bool bMultiple, bool bProcessResult);

void iris_recognition();

void power_management();

void recovery();

void finalize_device();

void handle_error(IddkResult error);

void reset_error_level(IddkResult result);
void get_result_image(int times);
void get_result_template(int times);

IddkImage* get_result_image_custom(int times);

IddkDataBuffer get_result_template_custom(int times);

bool compare_templates_custom(int dataSize, unsigned char* data);

////////////////////////////////////////////////
