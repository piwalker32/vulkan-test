#pragma once

#include "device.h"
uint32_t findMemoryType(Device* device, uint32_t typeFilter, VkMemoryPropertyFlags properties);