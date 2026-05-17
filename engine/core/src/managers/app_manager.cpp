#include "../../include/managers/app_manager.h"

#include "../../include/app/app.h"

#include "../../include/managers/log_manager.h"


engine::AppManager::~AppManager()
{
	logger.trace("AppManager destructor called");
}
