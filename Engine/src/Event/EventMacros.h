#pragma once

#define BIT(x) (1 << x)
#define BIND_EVENT_CALLBACK(x) std::bind(x, this, std::placeholders::_1)