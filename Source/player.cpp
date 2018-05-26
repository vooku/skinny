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

#include "player.h"
#include <algorithm>
#include <chrono>
#include <iostream>
extern "C" {
	#include <libavutil/time.h>
	#include <libavutil/imgutils.h>
}

const size_t Player::queue_size_{5};

Player::Player(const std::string &file_name) :
	demuxer_{ std::make_unique<Demuxer>(file_name) },
	video_decoder_{ std::make_unique<VideoDecoder>(demuxer_->video_codec_parameters()) },
	format_converter_{ std::make_unique<FormatConverter>(
		video_decoder_->width(), video_decoder_->height(),
		video_decoder_->pixel_format(), AV_PIX_FMT_RGB24) },
	timer_{ std::make_unique<player::Timer>() },
	packet_queue_{ std::make_unique<PacketQueue>(queue_size_) },
	frame_queue_{ std::make_unique<FrameQueue>(queue_size_) }
{
}

void Player::play() {
	stages_.emplace_back(&Player::demultiplex, this);
	stages_.emplace_back(&Player::decode_video, this);
	video();

	for (auto &stage : stages_) {
		stage.join();
	}

	if (exception_) {
		std::rethrow_exception(exception_);
	}
}

void Player::demultiplex() {
	try {
		for (;;) {
			// Create AVPacket
			std::unique_ptr<AVPacket, std::function<void(AVPacket*)>> packet{
				new AVPacket,
				[](AVPacket* p){ av_packet_unref(p); delete p; }};
			av_init_packet(packet.get());
			packet->data = nullptr;

			// Read frame into AVPacket
			if (!(*demuxer_)(*packet)) {
				packet_queue_->finished();
				break;
			}

			// Move into queue if first video stream
			if (packet->stream_index == demuxer_->video_stream_index()) {
				if (!packet_queue_->push(move(packet))) {
					break;
				}
			}
		}
	} catch (...) {
		exception_ = std::current_exception();
		frame_queue_->quit();
		packet_queue_->quit();
	}
}

void Player::decode_video() {
	try {
		const AVRational microseconds = {1, 1000000};

		for (;;) {
			// Create AVFrame and AVQueue
			std::unique_ptr<AVFrame, std::function<void(AVFrame*)>>
				frame_decoded{
					av_frame_alloc(), [](AVFrame* f){ av_frame_free(&f); }};
			std::unique_ptr<AVPacket, std::function<void(AVPacket*)>> packet{
				nullptr, [](AVPacket* p){ av_packet_unref(p); delete p; }};

			// Read packet from queue
			if (!packet_queue_->pop(packet)) {
				frame_queue_->finished();
				break;
			}

			// If the packet didn't send, receive more frames and try again
			bool sent = false;
			while (!sent) {
				sent = video_decoder_->send(packet.get());

				// If a whole frame has been decoded,
				// adjust time stamps and add to queue
				while (video_decoder_->receive(frame_decoded.get())) {
					frame_decoded->pts = av_rescale_q(
						frame_decoded->pkt_dts,
						demuxer_->time_base(),
						microseconds
                    );

					std::unique_ptr<AVFrame, std::function<void(AVFrame*)>> frame_converted {
							av_frame_alloc(),
							[](AVFrame* f){ av_free(f->data[0]); }
                    };
					if (av_frame_copy_props(frame_converted.get(), frame_decoded.get()) < 0) {
						throw std::runtime_error("Copying frame properties");
					}
                    frame_converted->width = video_decoder_->width();
                    frame_converted->height = video_decoder_->height();
					if (av_image_alloc(
						    frame_converted->data, frame_converted->linesize,
                            frame_converted->width, frame_converted->height,
                            AV_PIX_FMT_RGB24, 1
                        ) < 0) {
						throw std::runtime_error("Allocating picture");
					}

					(*format_converter_)(frame_decoded.get(), frame_converted.get());

					if (!frame_queue_->push(move(frame_converted))) {
						break;
					}
				}
			}
		}
	} catch (...) {
		exception_ = std::current_exception();
		frame_queue_->quit();
		packet_queue_->quit();
	}
}

void Player::video() {
	try {
		int64_t last_pts = 0;
        []() {};
		for (uint64_t frame_number = 0;; ++frame_number) {

			//display_->input();

			if (/*display_->get_quit()*/ false) {
				break;

			} else if (/*display_->get_play()*/ true) {
				std::unique_ptr<AVFrame, std::function<void(AVFrame*)>> frame {
					nullptr,
                    [](AVFrame* f){ av_frame_free(&f); }
                };
				if (!frame_queue_->pop(frame)) {
					break;
				}

				if (frame_number) {
					const int64_t frame_delay = frame->pts - last_pts;
					last_pts = frame->pts;
					timer_->wait(frame_delay);

				} else {
					last_pts = frame->pts;
					timer_->update();
				}
                
                w = frame->width;
                h = frame->height;
                pixels.resize(w * h);
                for (int y = 0; y < h; y++) {
                    auto srcln = y * frame->linesize[0];
                    auto dstln = y * w;
                    for (int x = 0; x < w; x++) {
                        pixels[dstln + x] = {
                            255,
                            frame->data[0][srcln + 3 * x + 0], 
                            frame->data[0][srcln + 3 * x + 1],
                            frame->data[0][srcln + 3 * x + 2]
                        };
                    }
                }
                newTex = true;

				//display_->refresh(
				//	{ frame->data[0], frame->data[1], frame->data[2] },
				//	{ static_cast<size_t>(frame->linesize[0]),
				//	  static_cast<size_t>(frame->linesize[1]),
				//	  static_cast<size_t>(frame->linesize[2]) });

			} else {
				std::chrono::milliseconds sleep(10);
				std::this_thread::sleep_for(sleep);
				timer_->update();
			}
		}

	} catch (...) {
		exception_ = std::current_exception();
	}

	frame_queue_->quit();
	packet_queue_->quit();
}
