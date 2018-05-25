/**
 * This file is part of pockethook/player https://github.com/pockethook/player.
 * Copyright (C) 2013-1017  Cheehan Weereratne
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#pragma once
#include <array>
#include <stdexcept>
extern "C" {
	#include "libavutil/avutil.h"
}

namespace ffmpeg {
class Error : std::runtime_error {
public:
	Error(const std::string &message);
	Error(int status);
};

std::string error_string(const int error_code);

inline int check(const int status) {
	if (status < 0) {
		throw ffmpeg::Error{status};
	}
	return status;
}
}
