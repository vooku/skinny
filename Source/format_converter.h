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
extern "C" {
	#include "libavformat/avformat.h"
	#include "libswscale/swscale.h"
}

class FormatConverter {
public:
	FormatConverter(
		size_t width, size_t height,
		AVPixelFormat input_pixel_format, AVPixelFormat output_pixel_format);
	void operator()(AVFrame* src, AVFrame* dst);
private:
	size_t width_;
	size_t height_;
	SwsContext* conversion_context_{};
};
