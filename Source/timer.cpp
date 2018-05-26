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

#include "timer.h"
#include <algorithm>
#include <thread>

namespace player {

Timer::Timer() :
	target_time_{std::chrono::high_resolution_clock::now()} {
}

void Timer::wait(const int64_t period) {
	target_time_ += std::chrono::microseconds{period};

	const auto lag =
		std::chrono::duration_cast<std::chrono::microseconds>(
			target_time_ - std::chrono::high_resolution_clock::now()) +
			std::chrono::microseconds{adjust()};

	std::this_thread::sleep_for(lag);

	const int64_t error =
		std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now() - target_time_).count();
	derivative_ = error - proportional_;
	integral_ += error;
	proportional_ = error;

}

void Timer::update() {
	target_time_ = std::chrono::high_resolution_clock::now();
}

int64_t Timer::adjust() const {
	return P_ * proportional_ + I_ * integral_ + D_ * derivative_;
}

} // namespace player