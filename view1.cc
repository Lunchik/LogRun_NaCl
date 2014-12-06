 /**
  * @file view1.cc
  * @author garipova.nailya@tgtoil.com
  * @author Copyright(C) 2012 TGT Oil & Gas Services. All right reserved.
  */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include <algorithm>

/*#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/image_data.h"
//#include "ppapi/cpp/input_event.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/point.h"
#include "ppapi/cpp/var.h"*/

#include "view1.h"
//#include "veholder.h"
//#include "../las_file/Las.h"

namespace {

//const uint32_t kOpaqueColorMask = 0xffff00ff;  // Opaque pixels.
const uint32_t okRedMask = 0xffff0000;
const uint32_t okWhiteMask = 0x00ffffff;
const uint32_t okBlackMask = 0xff000000;
const uint32_t okGreyMask = 0xffa0a0a0;
const uint32_t kLightGrey = 0xffe0e0e0;


// This is called by the browser when the 2D context has been flushed to the
// browser window.
void FlushCallback(void* data, int32_t result) {
  //IOExample* fileIO = static_cast<IOExample*>(data);
  //fileIO->PostMessage(pp::Var("setflushpending"));
	static_cast<LogRun::View*>(data)->set_flush_pending(false);
}

}  // namespace


namespace LogRun {
View::View(pp::Instance* instance)
    : instance_(instance), 
	flush_pending_(false),
    graphics_2d_context_(NULL), 
	pixel_buffer_(NULL)//,
	//scale_width_(100),
	//columns_width_(120),
	//header_height_(80) 
	{}

View::~View() {
  DestroyContext();
  delete pixel_buffer_;
}

pp::Size View::GetSize() const {
  pp::Size size;
  if (graphics_2d_context_) {
    size.SetSize(graphics_2d_context_->size().width(),
                  graphics_2d_context_->size().height());
  }
  return size;
}

/*int View::DrawFlush() {
  uint32_t* pixels = static_cast<uint32_t*>(pixel_buffer_->data());
  if (NULL == pixels)
    return 0;
  // Clear the buffer
  const int32_t height = pixel_buffer_->size().height();
  //return pixel_buffer_->size().height(); //0 somehow
  const int32_t width = pixel_buffer_->size().width();
  //const float radius2 = (ball_rect_.width() / 2) * (ball_rect_.width() / 2);
  for (int32_t py = 0; py < height; ++py) {
    for (int32_t px = 0; px < width; ++px) {
      const int32_t pos = px + py * width;
      uint32_t color = kRedMask;
      // Draw the paddles
      //if (left_paddle_rect_.Contains(px, py) ||
          //right_paddle_rect_.Contains(px, py)) {
        //color |= kWhiteMask;
      //} else {
        //pp::Point center_point = ball_rect_.CenterPoint();
        //float distance_x = px - center_point.x();
        //float distance_y = py - center_point.y();
        //float distance2 = distance_x * distance_x + distance_y * distance_y;
        // Draw the ball
        //if (distance2 <= radius2)
          //color |= kWhiteMask;
      //}
      pixels[pos] = color;
    }
  }
  return 0;
  FlushPixelBuffer();
}

int View::Draw(const pp::Rect& position) {
  
  //IOExample* fileIO = static_cast<IOExample*>(data);
  //fileIO->PostMessage(pp::Var("blablabla"));
  // Create a new device context with the new size.
  DestroyContext();
  return position.size().height();
  CreateContext(position.size(), instance_);
  
  // Delete the old pixel buffer and create a new one.
  delete pixel_buffer_;
  pixel_buffer_ = NULL;
  if (graphics_2d_context_ != NULL) {
    pixel_buffer_ = new pp::ImageData(instance_, PP_IMAGEDATAFORMAT_BGRA_PREMUL,
                                      graphics_2d_context_->size(),
                                      false);
	return DrawFlush();
	//return true;
  }
  else
	return 0;

} */

void View::Draw(/*data::las::logs_section_t* logs*/) {
	//using namespace std;
  //Do nothing yet
  /*uint32_t* pixels = static_cast<uint32_t*>(pixel_buffer_->data());
  if (NULL == pixels)
    return;
  // Clear the buffer
  const int32_t height = pixel_buffer_->size().height();
  const int32_t width = pixel_buffer_->size().width();
  hborder_coords_.push_back(height-1);
  vborder_coords_.push_back(width-1);
  //
  //data::las::depth_log_t& depth_log_ = logs.depth_log();
  //data::las::logs_section_t::logs_list_t& logs_list_ = logs.logs_list();
  //Number of columns
  //const int col_num = logs_list_->size() + 1;

  //const float radius2 = (ball_rect_.width() / 2) * (ball_rect_.width() / 2);
  for (int32_t py = 0; py < height; ++py) {
    for (int32_t px = 0; px < width; ++px) {
      const int32_t pos = px + py * width;
	  uint32_t color;
	  //std::vector<int32_t>::iterator find_result_v = std::find(vborder_coords_.begin(), vborder_coords_.end(), px);
	  //std::vector<int32_t>::iterator find_result_h = std::find(hborder_coords_.begin(), hborder_coords_.end(), py);
	  //if (find_result_v!=vborder_coords_.end() || find_result_h!=hborder_coords_.end())
		  //color = okBlackMask;
	  //else
	  color = okWhiteMask;
	  
      pixels[pos] = color;
    }
  }
  //In case next time function will be called with other parameters of height or width
  hborder_coords_.pop_back();
  vborder_coords_.pop_back();
  FlushPixelBuffer();*/
}

int View::DrawLas(VEHolder* veholder) {
	//using namespace std;
  uint32_t* pixels = static_cast<uint32_t*>(pixel_buffer_->data());
  if (NULL == pixels)
    return 0;
  // Clear the buffer
  const int32_t height = pixel_buffer_->size().height();
  const int32_t width = pixel_buffer_->size().width();

  //return (int)(sizeof((pixels))/sizeof((pixels[0]))); // - ok here

  for (int32_t py = 0; py < height; ++py) {
    //for (int32_t px = 0; px < width; ++px) {
    int32_t px = 0;
	if ((py < veholder->header_height_+cBorderWidth) && (py >= cBorderWidth)) { //1st case - header
		for (int icol=0; icol < veholder->column_num_; ++icol) {
			int32_t pos;
			int32_t col_width_ = veholder->column_list_[icol]->header_.width_;
			//draw borders
			if (icol == 0) {
				for (px = 0; px < cBorderWidth; ++px) {
					pos = px + py*width;
					pixels[pos] = okGreyMask;
					pos = px + cBorderWidth + col_width_ + py*width;
					pixels[pos] = okGreyMask;
				}
			}
			else {
				for (px = veholder->column_list_[icol]->header_.cox_ + col_width_; px < veholder->column_list_[icol]->header_.cox_  + col_width_ + cBorderWidth; ++px) {
					pos = px + py*width;
					pixels[pos] = okGreyMask;
				}
			}
			px = veholder->column_list_[icol]->header_.cox_;
			for (int32_t px_loc=0; px_loc < col_width_; ++px_loc) {
				pos = px + px_loc + py*width;
				/*if (find(veholder->coord_vl.begin(), veholder->coord_vl.end(), px + px_loc - veholder->border_width_) 
					!= veholder->coord_vl.end())
				{
					pixels[pos] = okBlackMask;
					continue;
				};*/
				pixels[pos] = veholder->column_list_[icol]->header_.pixel_buffer_[px_loc+(py - cBorderWidth)*col_width_];
			}
		}		 //for icol < column_num_

		int32_t edge = veholder->column_list_[veholder->column_num_-1]->header_.cox_ + 
					   veholder->column_list_[veholder->column_num_-1]->header_.width_;
		for (int32_t px_loc = edge; px_loc < width; px_loc++) {
			int32_t pos = px_loc + py*width;
			pixels[pos] = okGreyMask;
		}
		
		//now draw the scroll bar
		/*int32_t px = 0;
		for (int32_t px_loc=veholder->vControl_->cox_; px_loc<veholder->vControl_->cox_+veholder->vControl_->width_; px_loc++) {
			//
			int32_t pos = px_loc + py*width;
			pixels[pos] = veholder->vControl_->pixel_buffer_[px+(py-cBorderWidth-veholder->vControl_->coy_)*veholder->vControl_->width_];
			px++;
		}*/
	} //if 1st case (header)
	//2nd case - columns
	else if (py >= veholder->header_height_ + cBorderWidth*2 && py < veholder->overall_height_ - veholder->hControl_->height_ - cBorderWidth) {
		for (int icol=0; icol < veholder->column_num_; ++icol) {
			int32_t pos;
			//if (icol == 2) return px;
			int32_t col_width_ = veholder->column_list_[icol]->column_row_.width_;
			//return height;
			if (icol == 0) {
				for (px = 0; px < cBorderWidth; ++px) {
					pos = px + py*width;
					pixels[pos] = okGreyMask;
					pos = px + cBorderWidth + col_width_ + py*width;
					pixels[pos] = okGreyMask;
				}
			}
			else {
				for (px = veholder->column_list_[icol]->header_.cox_ + col_width_; px < veholder->column_list_[icol]->header_.cox_  + col_width_ + cBorderWidth; ++px) {
					pos = px + py*width;
					pixels[pos] = okGreyMask;
				}
			}
			px = veholder->column_list_[icol]->column_row_.cox_;
			for (int32_t px_loc=0; px_loc < col_width_; ++px_loc) {
				pos = px + px_loc + py*width;
				pixels[pos] = veholder->column_list_[icol]->column_row_.pixel_buffer_[px_loc+(py - veholder->header_height_ - cBorderWidth*2)*col_width_];
			}
		} // for icol

		int32_t edge= veholder->column_list_[veholder->column_num_-1]->header_.cox_ + 
					  veholder->column_list_[veholder->column_num_-1]->header_.width_;
		for (int32_t px_loc = edge; px_loc < width; px_loc++) {
			int32_t pos = px_loc + py*width;
			pixels[pos] = okWhiteMask;
		}

		//now draw the scroll bar
		int32_t px = 0;
		for (int32_t px_loc=veholder->vControl_->cox_; px_loc<veholder->vControl_->cox_+veholder->vControl_->width_; px_loc++) {
			//
			int32_t pos = px_loc + py*width;
			pixels[pos] = veholder->vControl_->pixel_buffer_[px+(py-cBorderWidth-veholder->vControl_->coy_)*veholder->vControl_->width_];
			px++;
		}
	} //else if 2nd case
	//3rd case - border
	else if (py < veholder->header_height_ + cBorderWidth*2 && py >= veholder->header_height_ + cBorderWidth) {
		for (int32_t px = 0; px < width - veholder->vControl_->width_; ++px) {
			const int32_t pos = px + py * width;
			pixels[pos] = okGreyMask;
		}
		//now draw the scroll bar
		int32_t px = 0;
		for (int32_t px_loc=veholder->vControl_->cox_; px_loc<veholder->vControl_->cox_+veholder->vControl_->width_; px_loc++) {
			//
			int32_t pos = px_loc + py*width;
			pixels[pos] = veholder->vControl_->pixel_buffer_[px+(py-cBorderWidth-veholder->vControl_->coy_)*veholder->vControl_->width_];
			px++;
		}
	} //else if
	//4th case - horisontal scroll bar
	else if (py >= veholder->overall_height_ - veholder->hControl_->height_) {
		//here I draw a horisontal scroll bar and the area where bars cross
		int32_t px = 0;
		for (int32_t px_loc=veholder->hControl_->cox_; px_loc < veholder->hControl_->cox_+veholder->hControl_->width_; ++px_loc) {
			int32_t pos = px_loc + py*width;
			pixels[pos] = veholder->hControl_->pixel_buffer_[px+(py-cBorderWidth-veholder->hControl_->coy_)*veholder->hControl_->width_];
			px++;
		}
		for (int32_t px_loc= veholder->hControl_->cox_+veholder->hControl_->width_; px_loc < width; px_loc++) {
			//just light grey
			int32_t pos = px_loc + py*width;
			pixels[pos] = kLightGrey;
		}
	}
	/*else if (py < cBorderWidth) {
		//
		for (int32_t px = 0; px < width; ++px) {
			const int32_t pos = px + py * width;
			pixels[pos] = okGreyMask;
		}
	}*/ // else if
	/*else if (py >= veholder->overall_height_ - cBorderWidth) {
		for (int32_t px = 0; px < width; ++px) {
			const int32_t pos = px + py * width;
			pixels[pos] = okGreyMask;
		}
	}*/
  }
  FlushPixelBuffer();
  return 111;
}

void View::UpdateView(const pp::Rect& position,
                      const pp::Rect& clip,
                      pp::Instance* instance) {
  const int32_t width =
      pixel_buffer_ ? pixel_buffer_->size().width() : 0;
  const int32_t height =
      pixel_buffer_ ? pixel_buffer_->size().height() : 0;

  if (position.size().width() == width &&
      position.size().height() == height)
    return;  // Size didn't change, no need to update anything.

  // Create a new device context with the new size.
  DestroyContext();
  CreateContext(position.size(), instance);
  // Delete the old pixel buffer and create a new one.
  delete pixel_buffer_;
  pixel_buffer_ = NULL;
  if (graphics_2d_context_ != NULL) {
    pixel_buffer_ = new pp::ImageData(instance, PP_IMAGEDATAFORMAT_RGBA_PREMUL,
                                      graphics_2d_context_->size(),
                                      false);
  }
}

void View::UpdateSmallView(const pp::Rect& position,
                      pp::Instance* instance) {
  const int32_t width =
      pixel_buffer_ ? pixel_buffer_->size().width() : 0;
  const int32_t height =
      pixel_buffer_ ? pixel_buffer_->size().height() : 0;

  if (position.size().width() == width &&
      position.size().height() == height)
    return;  // Size didn't change, no need to update anything.

  // Create a new device context with the new size.
  DestroyContext();
  CreateContext(position.size(), instance);
  // Delete the old pixel buffer and create a new one.
  delete pixel_buffer_;
  pixel_buffer_ = NULL;
  if (graphics_2d_context_ != NULL) {
    pixel_buffer_ = new pp::ImageData(instance, PP_IMAGEDATAFORMAT_RGBA_PREMUL,
                                      graphics_2d_context_->size(),
                                      false);
  }
}

void View::CreateContext(const pp::Size& size, pp::Instance* instance) {
  if (IsContextValid())
    return;
  graphics_2d_context_ = new pp::Graphics2D(instance, size,
                                            false);
  if (!instance->BindGraphics(*graphics_2d_context_)) {
    instance_->PostMessage(pp::Var("ERROR: Couldn't bind the device context"));
  }
}

void View::DestroyContext() {
  if (!IsContextValid())
    return;
  delete graphics_2d_context_;
  graphics_2d_context_ = NULL;
}

void View::FlushPixelBuffer() {
  if (!IsContextValid())
    return;
  // Note that the pixel lock is held while the buffer is copied into the
  // device context and then flushed.
  graphics_2d_context_->PaintImageData(*pixel_buffer_, pp::Point());
  if (flush_pending_)
    return;
  flush_pending_ = true;
  graphics_2d_context_->Flush(pp::CompletionCallback(&FlushCallback, this));
}

bool View::IsContextValid() const {
    return graphics_2d_context_ != NULL;
}

}  // namespace LogRun