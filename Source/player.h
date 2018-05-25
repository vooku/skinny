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
#include "demuxer.h"
#include "format_converter.h"
#include "queue.h"
#include "timer.h"
#include "video_decoder.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
extern "C" {
	#include <libavcodec/avcodec.h>
}

class Player {
public:
	Player(const std::string &file_name);
	void operator()();
private:
	void demultiplex();
	void decode_video();
	void video();
private:
	std::unique_ptr<Demuxer> demuxer_;
	std::unique_ptr<VideoDecoder> video_decoder_;
	std::unique_ptr<FormatConverter> format_converter_;
	//std::unique_ptr<Display> display_;
	std::unique_ptr<Timer> timer_;
	std::unique_ptr<PacketQueue> packet_queue_;
	std::unique_ptr<FrameQueue> frame_queue_;
	std::vector<std::thread> stages_;
	static const size_t queue_size_;
	std::exception_ptr exception_{};
};
