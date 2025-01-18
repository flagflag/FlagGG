#pragma once

#include "Core/CryAssert.h"
#include "Log.h"

#define VULKAN_CHECK(call, fail) \
	{ \
		VkResult result = call; \
		if (result != VK_SUCCESS) \
		{ \
			FLAGGG_LOG_STD_ERROR(#call "=> Vulkan error: %d", result); \
			ASSERT_MESSAGE(false, #call); \
			return fail; \
		} \
	}

