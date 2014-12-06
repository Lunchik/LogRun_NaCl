 /**
  * @file veholder.h
  * @author garipova.nailya@tgtoil.com
  * @author Copyright(C) 2012 TGT Oil & Gas Services. All right reserved.
  */

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
//#include <cairo/cairo.h> //I'll use AGG
//#include <cairoint.h>
#include "Las.h"

#include "agg-2.5/agg_basics.h"
#include "agg-2.5/agg_rendering_buffer.h"
#include "agg-2.5/agg_pixfmt_rgb.h"
#include "agg-2.5/agg_glyph_raster_bin.h"
#include "agg-2.5/agg_renderer_raster_text.h"
#include "agg-2.5/agg_embedded_raster_fonts.h"
#include "agg-2.5/agg_renderer_primitives.h"
#include "agg-2.5/agg_renderer_markers.h"
#include "agg-2.5/agg_rasterizer_scanline_aa.h"
#include "agg-2.5/agg_scanline_p.h"
#include "agg-2.5/agg_renderer_scanline.h"
#include "agg-2.5/agg_pixfmt_gray.h"
#include "agg-2.5/agg_pixfmt_rgba.h"
#include "agg-2.5/agg_platform_support.h"
#include "agg-2.5/agg_span_allocator.h"
#include "agg-2.5/agg_span_gouraud_rgba.h"
#include "agg-2.5/agg_path_storage.h"
#include "agg-2.5/agg_conv_stroke.h"

#include "lr_scale_div_c.h"


namespace {

const int32_t cBorderWidth = 0;

const uint32_t kOpaqueColorMask = 0xffff00ff;  // Opaque pixels.
const uint32_t kRedMask = 0xffff0000;
const uint32_t kWhiteMask = 0x00ffffff;
const uint32_t kBlackMask = 0xff000000;
const uint32_t kGreyMask = 0xff808080;

const int32_t scroll_control_size_ = 20;

}

enum flip_y_e { flip_y = true };

enum positioning { HORISONTAL, VERTICAL };

namespace LogRun {
	
	class TickDivider;

	//std::vector<coord> coords_;
	typedef struct coord {
		double x;
		double y;
	}COORD;

	struct Header { //h-resizable
		
		void setSize(int32_t w, int32_t h) {
			width_ = w;
			height_ = h;
		}

		void setCoords(int32_t x, int32_t y) {
			cox_ = x;
			coy_ = y;
		}

		void aggDraw(agg::rendering_buffer &renbuf)
		{
			typedef agg::pixfmt_rgba32 pixfmt_type;
			typedef agg::renderer_base<pixfmt_type> ren_type;
			typedef agg::renderer_scanline_aa_solid<ren_type> renderer_solid;
			typedef agg::glyph_raster_bin<agg::rgba8> glyph_gen;

			unsigned iWidth = renbuf.width();
			unsigned iHeight = renbuf.height();
			//unsigned iStride = renbuf.stride();

			pixfmt_type pixf(renbuf);
			ren_type rbase(pixf);
			renderer_solid rsolid(rbase);
			//rbase.clip_box(2, 2, iWidth-2, iHeight-2);
			//For now I'll just fill the space with grey, but I want to use gradient - If I have time
			ren_type::color_type lightgrey(0xC0, 0xC0, 0xC0, 0xFF);
			ren_type::color_type grey(0xA0, 0xA0, 0xA0, 0xFF);
			ren_type::color_type black(0, 0, 0, 0xFF);

			/*typedef agg::span_interpolator_linear<> interpolator_type;
			typedef agg::span_allocator<agg::rgba8> span_alloc_type;
			typedef agg::span_gradient<agg::rgba8, 
                                   interpolator_type, 
								   agg::gradient_repeat_adaptor<agg::gradient_circle>, 
                                   agg::gradient_linear_color<agg::rgba8> > span_gen_type;
			typedef agg::renderer_scanline_aa<rbase,
									span_alloc_type,
									span_gen_type> ren_sca;*/

			rbase.clear(grey);

			agg::rasterizer_scanline_aa<> ras;
			agg::scanline_p8 sl;

			agg::path_storage pstor;
			agg::conv_stroke<agg::path_storage> pl(pstor);

			pl.width(1.0);

			//grey border
			pstor.remove_all();
			pstor.move_to(0,0);
			pstor.line_to(iWidth-1,0);
			pstor.line_to(iWidth-1,iHeight-1);
			pstor.line_to(0,iHeight-1);
			pstor.line_to(0,0);
			ras.add_path(pl);
			rsolid.color(lightgrey);
			agg::render_scanlines(ras, sl, rsolid);

			//black border
			pstor.remove_all();
			pstor.move_to(1,1);
			pstor.line_to(iWidth-2,1);
			pstor.line_to(iWidth-2,iHeight-2);
			pstor.line_to(1,iHeight-2);
			pstor.line_to(1,1);
			ras.add_path(pl);
			rsolid.color(black);
			agg::render_scanlines(ras, sl, rsolid);
			
			//agg::glyph_rendering gren = agg::glyph_ren_native_mono;
			//if(m_feng.create_font("Times New Roman", agg::glyph_ren_outline))

			glyph_gen glyph(0);

			agg::renderer_raster_htext_solid<ren_type, glyph_gen> rt(rbase, glyph);
			glyph.font(agg::verdana18_bold);
			rt.color(black);
			rt.render_text(iWidth/2 - legend_.length()*6,20, &legend_[0], flip_y);
			if (!units_.empty())
			{
				glyph.font(agg::verdana18);
				rt.render_text(iWidth/2 - units_.length()*5,40, &units_[0], flip_y);
			}

			/*typedef agg::span_allocator<agg::rgba8> span_alloc_type;
			typedef agg::span_gouraud_rgba<agg::rgba8> span_gen_type;
			typedef agg::renderer_scanline_aa<ren_base, span_alloc_type, span_gen_type> ren_type;

			span_alloc_type sa;
			
			span_gen_type sg;
			ren_type ren_sca(rbase, sa, sg);

			agg::renderer_raster_htext<ren_type, glyph_gen> rt_sca(ren_sca, glyph);
			glyph.font(agg::verdana18_bold);
			rt_sca.render_text(iWidth/2 - legend_.length()*7,15, &legend_[0], flip_y); */
		}

		std::string setPixels(std::string legend, std::string units) {

			std::string gotit;
			//pixel_buffer_.assign((width_)*(height_), kGreyMask);

			legend_ = legend;
			units_ = units;
			
			//create rendering buffer
			unsigned char* buffer = new unsigned char[width_ * height_ * 4];
			memset(buffer, 255, width_ * height_ * 4);

			agg::rendering_buffer rbuf(buffer,
									   width_,
									   height_,
									   width_ * 4);

			aggDraw(rbuf);

			pixel_buffer_ = (uint32_t*)(rbuf.buf());
			
			gotit = "end";
			return gotit;
			
		}

		//coords_t coords_;
		int32_t cox_;
		int32_t coy_;
		//int32_t height_; - has to inherit from headers
		//int32_t height_;
		//int32_t width_;
		//size_type size_;
		int32_t width_;
		int32_t height_;
		//std::vector<std::string> legend_;
		uint32_t* pixel_buffer_;
		std::string legend_;
		std::string units_;
	};

	struct Column_row { //h-resizable

		void setSize(int32_t w, int32_t h) {
			width_ = w;
			height_ = h;
		}
		
		void setCoords(int32_t x, int32_t y) {
			cox_ = x;
			coy_ = y;
		}

		void setDepth(double up, double low) {
			upper_ = up;
			lower_ = low;
		}

		void aggDrawScale(agg::rendering_buffer &renbuf)
		{
			typedef agg::pixfmt_rgba32 pixfmt_type;
			typedef agg::renderer_base<pixfmt_type> ren_type;
			typedef agg::renderer_scanline_aa_solid<ren_type> renderer_solid;
			//typedef agg::renderer_scanline_aa_solid<ren_type> renderer_solid;
			typedef agg::glyph_raster_bin<agg::rgba8> glyph_gen;

			unsigned iWidth = renbuf.width();
			unsigned iHeight = renbuf.height();
			//unsigned iStride = renbuf.stride();

			pixfmt_type pixf(renbuf);
			ren_type rbase(pixf);
			renderer_solid rsolid(rbase);
			//rbase.clip_box(2, 2, iWidth-2, iHeight-2);
			//For now I'll just fill the space with grey, but I want to use gradient - If I have time
			ren_type::color_type lightgrey(0xC0, 0xC0, 0xC0, 0xFF);
			ren_type::color_type black(0, 0, 0, 0xFF);

			rbase.clear(lightgrey);

			agg::rasterizer_scanline_aa<> ras;
			agg::scanline_p8 sl;

			agg::path_storage pstor;
			agg::conv_stroke<agg::path_storage> pl(pstor);

			pl.width(1.0);

			/*//just checking

			std::string gotit;
			std::ostringstream strs;
			strs << firstTick_;
			gotit = strs.str();*/
			
			glyph_gen glyph(0);

			agg::renderer_raster_htext_solid<ren_type, glyph_gen> rt(rbase, glyph);
			glyph.font(agg::verdana14_bold);
			rt.color(black);
			/*rt.render_text(2,20, &gotit[0], flip_y);*/

			/*strs << firstTickPx_;
			gotit = strs.str();
			rt.render_text(2,20, &gotit[0], flip_y);

			strs << step_;
			gotit = strs.str();
			rt.render_text(2,20, &gotit[0], flip_y);

			strs << stepPx_;
			gotit = strs.str();
			rt.render_text(2,20, &gotit[0], flip_y);*/
			
			//tiil here


			pstor.remove_all();

			int minLen = iWidth / 8;
			int medLen = iWidth / 4;
			double smallStepPx = stepPx_ / 10;
			//nextStep is in meters
			double nextStep = fabs(firstTick_);
			int i=0;
			//while (nextStep < fabs(lower_)) {
			while (firstTickPx_ < iHeight) {
				pstor.move_to(2,firstTickPx_);
				pstor.line_to(iWidth-2, firstTickPx_);
				ras.add_path(pl);
				for (int ii =1; ii<10; ++ii) {
					if ( ii ==5) {
						//draw med tick
						pstor.move_to(2, firstTickPx_ + smallStepPx*ii);
						pstor.line_to(2 + medLen, firstTickPx_ + smallStepPx*ii);
						pstor.move_to(iWidth-2-medLen, firstTickPx_ + smallStepPx*ii);
						pstor.line_to(iWidth-2, firstTickPx_ + smallStepPx*ii);
					}
					else {
						//draw minor tick
						pstor.move_to(2, firstTickPx_ + smallStepPx*ii);
						pstor.line_to(2 + minLen, firstTickPx_ + smallStepPx*ii);
						pstor.move_to(iWidth-2-minLen, firstTickPx_ + smallStepPx*ii);
						pstor.line_to(iWidth-2, firstTickPx_ + smallStepPx*ii);
					}
					ras.add_path(pl);
				}
				
				std::string gotit;
				std::ostringstream strs;
				strs << nextStep;
				gotit = strs.str();

				rt.render_text(iWidth/2 - gotit.length()*4,firstTickPx_ - 2, &gotit[0], flip_y);

				firstTickPx_ += stepPx_;
				nextStep += step_;
				++i;
				if (i>1000) break;
			}
			rsolid.color(black);
			agg::render_scanlines(ras, sl, rsolid);

			//grey border
			pstor.remove_all();
			pstor.move_to(0,0);
			pstor.line_to(iWidth-1,0);
			pstor.line_to(iWidth-1,iHeight-1);
			pstor.line_to(0,iHeight-1);
			pstor.line_to(0,0);
			ras.add_path(pl);
			rsolid.color(lightgrey);
			agg::render_scanlines(ras, sl, rsolid);

			//black border
			pstor.remove_all();
			pstor.move_to(1,1);
			pstor.line_to(iWidth-2,1);
			pstor.line_to(iWidth-2,iHeight-2);
			pstor.line_to(1,iHeight-2);
			pstor.line_to(1,1);
			ras.add_path(pl);
			rsolid.color(black);
			agg::render_scanlines(ras, sl, rsolid);

			/*agg::renderer_primitives<ren_type> sRenPrim(rbase);
			//sRenPrim.fill_color(lightgrey);
			sRenPrim.line_color(black);
			sRenPrim.rectangle(0, 0, iWidth-1, iHeight-1);*/

		}

		void aggDrawColumn(agg::rendering_buffer &renbuf)
		{
			typedef agg::pixfmt_rgba32 pixfmt_type;
			typedef agg::renderer_base<pixfmt_type> ren_type;
			typedef agg::renderer_scanline_aa_solid<ren_type> renderer_solid;

			unsigned iWidth = renbuf.width();
			unsigned iHeight = renbuf.height();
			//unsigned iStride = renbuf.stride();

			pixfmt_type pixf(renbuf);
			ren_type rbase(pixf);
			renderer_solid rsolid(rbase);
			//rbase.clip_box(2, 2, iWidth-2, iHeight-2);
			//For now I'll just fill the space with grey, but I want to use gradient - If I have time
			ren_type::color_type white(0xFF, 0xFF, 0xFF, 0xFF);
			ren_type::color_type lightgrey(0xC0, 0xC0, 0xC0, 0xFF);
			ren_type::color_type black(0, 0, 0, 0xFF);
			ren_type::color_type red(0xFF, 0, 0, 0xFF);

			rbase.clear(white);

			agg::rasterizer_scanline_aa<> ras;
			agg::scanline_p8 sl;

			agg::path_storage pstor;
			agg::conv_stroke<agg::path_storage> pl(pstor);

			pl.width(1.0);

			pstor.remove_all();
			pstor.move_to(coords_[0].x, coords_[0].y);
			for (unsigned int ii = 1; ii < coords_.size()-1; ++ii)
			{
				pstor.line_to(coords_[ii].x, coords_[ii].y);
			}
			pstor.line_to(coords_[coords_.size()-1].x, coords_[coords_.size()-1].y);
			ras.add_path(pl);
			rsolid.color(red);
			agg::render_scanlines(ras, sl, rsolid);

			//grey border
			pstor.remove_all();
			pstor.move_to(0,0);
			pstor.line_to(iWidth-1,0);
			pstor.line_to(iWidth-1,iHeight-1);
			pstor.line_to(0,iHeight-1);
			pstor.line_to(0,0);
			ras.add_path(pl);
			rsolid.color(lightgrey);
			agg::render_scanlines(ras, sl, rsolid);

			//black border
			pstor.remove_all();
			pstor.move_to(1,1);
			pstor.line_to(iWidth-2,1);
			pstor.line_to(iWidth-2,iHeight-2);
			pstor.line_to(1,iHeight-2);
			pstor.line_to(1,1);
			ras.add_path(pl);
			rsolid.color(black);
			agg::render_scanlines(ras, sl, rsolid);

			/*agg::renderer_primitives<ren_type> sRenPrim(rbase);
			//sRenPrim.fill_color(white);
			sRenPrim.line_color(black);
			sRenPrim.rectangle(0, 0, iWidth-1, iHeight-1);
			
			//draw curves
			sRenPrim.line_color(red);
			sRenPrim.move_to(sRenPrim.coord(coords_[0].x), sRenPrim.coord(coords_[0].y));
			for (unsigned int ii = 1; ii < coords_.size()-1; ++ii)
			{
				sRenPrim.line_to(sRenPrim.coord(coords_[ii].x), sRenPrim.coord(coords_[ii].y), false);
			}
			sRenPrim.line_to(sRenPrim.coord(coords_[coords_.size()-1].x), sRenPrim.coord(coords_[coords_.size()-1].y), true);
			*/
		}

		void setPixelsScale() {
			
			unsigned char* buffer = new unsigned char[width_ * height_ * 4];
			memset(buffer, 255, width_ * height_ * 4);

			//figure out the best way to divide a scale
			//double firstTick, step;

			TickDivider *tDiv = new TickDivider(upper_, lower_,
												0, height_);

			tDiv->findEvenDivision(15, firstTick_, step_);

			//I need to keep firtsTick_ and step_ values in meters to form labels
			//I also need these values in pixels in order to save 1 operation while drawing
			firstTickPx_ = (firstTick_ - upper_) * tDiv->mInPx_;
			stepPx_ = step_ / tDiv->mInPx_;

			//bring the firstTick_ value to our depth value
			//firstTick_ += upper_;

			agg::rendering_buffer rbuf(buffer,
									   width_,
									   height_,
									   width_ * 4);

			aggDrawScale(rbuf);

			pixel_buffer_ = (uint32_t*)(rbuf.buf());

		}

		void setPixelsColumn(std::vector<COORD> coords) {
			//coords_ = coords;
			coords_.resize(coords.size());
			coords_.assign(coords.begin(), coords.end());
			unsigned char* buffer = new unsigned char[width_ * height_ * 4];
			memset(buffer, 255, width_ * height_ * 4);

			agg::rendering_buffer rbuf(buffer,
									   width_,
									   height_,
									   width_ * 4);

			aggDrawColumn(rbuf);

			pixel_buffer_ = (uint32_t*)(rbuf.buf());
		}

		//coords_t coords_;
		int32_t cox_;
		int32_t coy_;
		//size_type size_;
		int32_t width_;
		int32_t height_;
		double upper_;
		double lower_;
		double firstTick_;
		double step_;
		double firstTickPx_;
		double stepPx_;
		//std::vector<uint32_t> pixel_buffer_;
		uint32_t* pixel_buffer_;
		std::vector<COORD> coords_;
	};

	//class RestOfSpace : public Column {}
	
	//class Scale : public Column {}

	struct Column {
	public:
		Column() : cox_(0),
				   coy_(0),
				   width_(0),
				   height_(0) {};
		//~Column();

		/*struct coord {
			double x;
			double y;
		}COORD;*/
		//std::vector<coord> coords_;

		void setxBorders(double minVal, double maxVal) {
			//
			minVal_ = minVal;
			maxVal_ = maxVal;
		}

		void setyBorders(double depthMin, double depthMax) {
			depthMin_ = depthMin;
			depthMax_ = depthMax;
		}

		void setSize(int32_t w, int32_t h) {
			width_ = w;
			height_ = h;
		}

		void setCoords(int32_t x, int32_t y) {
			cox_ = x;
			coy_ = y;
		}

		double setInitCurveCoords(std::vector<COORD> &coords) {
			initCoords_.resize(coords.size());
			initCoords_.assign(coords.begin(), coords.end());
			//initCoords_ = coords;
			return initCoords_[4].y;
		}
		
		void setLegend(std::string legend, std::string units) {
			legend_.resize(legend.size());
			//legend_.assign(legend);
			units_.resize(units.size());
			//units_.assign(units);
			legend_ = legend;
			units_ = units;
		}

		std::string setHeader(int32_t x, int32_t y,
							int32_t w, int32_t h) {
					   //, std::string legend, std::string units) {
			header_.setSize(w, h);
			header_.setCoords(x, y);
			//header_.legend_ = legend;
			//return legend_;
			return header_.setPixels(legend_, units_);
		}

		void setRowScale(int32_t x, int32_t y,
					int32_t w, int32_t h) { //,
					//double up, double low) {
			column_row_.setSize(w, h);
			column_row_.setCoords(x, y);
			column_row_.setDepth(depthMin_, depthMax_);
			column_row_.setPixelsScale();
		}

		void setRowColumn(/*std::vector<COORD> coords_,*/ int32_t x, int32_t y,
					int32_t w, int32_t h) {
			column_row_.setSize(w, h);
			column_row_.setCoords(x, y);

			//Calculate the pixel coordinates from the real coords (value, depth)
			double lDepth = fabs(depthMax_ - depthMin_);
			double yScale = (height_ - 10)/lDepth;
			
			std::vector<COORD> coords_;
			double lWidth = maxVal_ - minVal_;
			double xScale = (width_-10)/lWidth;
			//here i form the coordinates vector in coords_ struct, then pass the pointer on
			for (unsigned int ii=0; ii<initCoords_.size(); ++ii) 
			{
				//
				COORD coord_;
				coord_.y = (initCoords_[ii].y - depthMin_)*yScale+5;
				coord_.x = (initCoords_[ii].x - minVal_)*xScale+5;
				coords_.push_back(coord_);
			}

			column_row_.setPixelsColumn(coords_);
		}


	//private:
		//coords_t coords_;
		int32_t cox_;
		int32_t coy_;
		Header header_;
		//uint32_t* pixel_buffer_;
		Column_row column_row_;
		//size_type size_;
		int32_t width_;
		int32_t height_;
		double minVal_;
		double maxVal_;
		double depthMin_;
		double depthMax_;
		//std::vector<coord> coords_;
		std::vector<COORD> initCoords_;
		std::string legend_;
		std::string units_;
	};

	struct Control {
	public:
		Control() 
			: cox_(0),
			  coy_(0),
			  width_(0),
			  height_(0),
			  scene_size_(0),
			  position_(0)
			  {};

		~Control();
		//I need to keep here the pixel_buffer_

		void aggDrawControl(agg::rendering_buffer &renbuf)
		{
			typedef agg::pixfmt_rgba32 pixfmt_type;
			typedef agg::renderer_base<pixfmt_type> ren_type;
			typedef agg::renderer_scanline_aa_solid<ren_type> renderer_solid;
			//typedef agg::renderer_scanline_aa_solid<ren_type> renderer_solid;

			unsigned iWidth = renbuf.width();
			unsigned iHeight = renbuf.height();
			//unsigned iStride = renbuf.stride();

			pixfmt_type pixf(renbuf);
			ren_type rbase(pixf);
			renderer_solid rsolid(rbase);
			//rbase.clip_box(2, 2, iWidth-2, iHeight-2);
			//For now I'll just fill the space with grey, but I want to use gradient - If I have time
			ren_type::color_type lightgrey(0xE0, 0xE0, 0xE0, 0xFF);
			ren_type::color_type blackgrey(0x20, 0x20, 0x20, 0xFF);

			rbase.clear(lightgrey);

			agg::rasterizer_scanline_aa<> ras;
			agg::scanline_p8 sl;

			agg::path_storage pstor;
			agg::conv_stroke<agg::path_storage> pl(pstor);

			pl.width(1.0);

			pstor.remove_all();

			if (hvpos_ == HORISONTAL) {
				//here I draw arrows - left arrow
				pstor.move_to(5, iHeight/2);
				pstor.line_to(5+4, iHeight/2 + 4);
				pstor.line_to(5+4, iHeight/2 - 4);
				pstor.close_polygon();
				ras.add_path(pl);
				rsolid.color(blackgrey);
				agg::render_scanlines(ras, sl, rsolid);

				//right arrow
				pstor.move_to(iWidth-5, iHeight/2);
				pstor.line_to(iWidth - 5 - 4, iHeight/2 + 4);
				pstor.line_to(iWidth - 5 - 4, iHeight/2 - 4);
				pstor.close_polygon();
				ras.add_path(pl);
				rsolid.color(blackgrey);
				agg::render_scanlines(ras, sl, rsolid);
			}
			else if (hvpos_ == VERTICAL) {
				//here I draw arrows - upper arrow
				pstor.move_to(iWidth/2, 5);
				pstor.line_to(iWidth/2 - 4, 5+4);
				pstor.line_to(iWidth/2 + 4, 5+4);
				pstor.close_polygon();
				ras.add_path(pl);
				rsolid.color(blackgrey);
				agg::render_scanlines(ras, sl, rsolid);

				//lower arrow
				pstor.move_to(iWidth/2, iHeight - 5);
				pstor.line_to(iWidth/2 - 4, iHeight - 5 - 4);
				pstor.line_to(iWidth/2 + 4, iHeight - 5 - 4);
				pstor.close_polygon();
				ras.add_path(pl);
				rsolid.color(blackgrey);
				agg::render_scanlines(ras, sl, rsolid);
			}
			else {} //smth's wrong - define the error later

			

			//grey border
			pstor.remove_all();
			pstor.move_to(0,0);
			pstor.line_to(iWidth-1,0);
			pstor.line_to(iWidth-1,iHeight-1);
			pstor.line_to(0,iHeight-1);
			pstor.line_to(0,0);
			ras.add_path(pl);
			rsolid.color(blackgrey);
			agg::render_scanlines(ras, sl, rsolid);


		}

		void setPixelsControl() {
			
			unsigned char* buffer = new unsigned char[width_ * height_ * 4];
			memset(buffer, 255, width_ * height_ * 4);

			agg::rendering_buffer rbuf(buffer,
									   width_,
									   height_,
									   width_ * 4);

			aggDrawControl(rbuf);

			pixel_buffer_ = (uint32_t*)(rbuf.buf());

		}

		void setControl(positioning hvpos, int32_t w, int32_t h, int32_t x, int32_t y, 
						int32_t position, int32_t scene_size) {
			hvpos_ = hvpos;
			width_ = w;
			height_ = h;
			cox_ = x;
			coy_ = y;
			position_ = position;
			scene_size_ = scene_size;

			setPixelsControl();
		}


		bool enabled_; //if the scene is larger than the display dimension
		int32_t cox_;
		int32_t coy_;
		int32_t width_;
		int32_t height_;
		int32_t scene_size_;
		int32_t position_;
		uint32_t* pixel_buffer_;
		positioning hvpos_;
	};



	class VEHolder {
	public:
		//every element has its' own pixel_buffer_ and coordinates
		VEHolder()
				 {};
		VEHolder(/*widths*/int32_t s_w, int32_t c_w, int32_t o_w,
				 /*heights*/int32_t h_h, int32_t o_h)
				 : vControl_(NULL)
				 , hControl_(NULL)
				 , column_list_(NULL)
				 , scale_width_(s_w)
				 , column_width_(c_w)
				 , overall_width_(o_w)
				 , header_height_(h_h)
				 , overall_height_(o_h)
				 //, logs_(NULL)
				 //, depth(NULL)
				 , column_num_(0)
				 {
			//here I initiate h/w sizes of header and columns 
			//headers_ = new Headers();
			//columns_ = new Columns();

			//header_.coords_ = {0, 0};
			//header_.size_ = {overall_width, header_height};
			//columns_.coords_ = {0, header_height + 2};
			//columns_.size_ = {overall_width, overall_height - (header_height + 2)};
		}
		~VEHolder();

		void setColList(int col_num_)
		{
			//create a vector<Column> of needed size
			//return (int)column_list_.max_size();
			column_list_.reserve(col_num_);
			//return -1;
			for (int i=0; i<col_num_; ++i) {
				Column* col_ = new Column();
				//return i;
				column_list_.push_back(col_);
				//return i;
			};
			//return 111;
		};

		std::string getImageDataLas(data::las::las_t* las_) {

			std::ostringstream strs;


			//las_ = las;
			data::las::logs_section_t& logs_ = las_->logs_section();
			//logs_ = las_->logs_section();
			data::las::depth_log_t& depth = logs_.depth_log();
			data::las::logs_section_t::logs_list_t logs_list_ = logs_.logs_list();
			column_num_ = logs_list_.size() + 1;
			border_width_ = cBorderWidth;

			std::string gotit = "170";
			//return column_num_;

			hControl_ = new Control();

			hControl_->setControl(HORISONTAL, overall_width_ - scroll_control_size_, scroll_control_size_, 
								0, overall_height_ - scroll_control_size_, 
								0, 0); //they are zero for initialization

			vControl_ = new Control();

			vControl_->setControl(VERTICAL, scroll_control_size_, overall_height_ - header_height_ - scroll_control_size_, 
								overall_width_ - scroll_control_size_, header_height_, 
								0, 0); //they are zero for initialization
						
			//return gotit;

			setColList(column_num_);

			column_list_[0]->setxBorders(0, 0);
			column_list_[0]->setyBorders(depth.curve[0], depth.curve[depth.curve.size()-1]);
			column_list_[0]->setSize(scale_width_, overall_height_ - scroll_control_size_);
			column_list_[0]->setLegend(depth.name, depth.unit);
			column_list_[0]->setCoords(cBorderWidth, cBorderWidth);

			gotit = column_list_[0]->setHeader(cBorderWidth, cBorderWidth, 
											   scale_width_, header_height_); 
											  //logs_.depth_log().name, logs_.depth_log().unit);

			column_list_[0]->setRowScale(cBorderWidth, cBorderWidth*2+header_height_,
											scale_width_, overall_height_ - header_height_ - scroll_control_size_ - cBorderWidth*2);//,
											//depth.curve[0], depth.curve[depth.curve.size()-1]);

			for (int i = 1; i<column_num_; ++i)
			{

				//form the pixel coordinates from las data and depth logs.
				double minVal = logs_list_[i-1].curve[0].value;
				double maxVal = logs_list_[i-1].curve[0].value;
				std::vector<COORD> coords_;
				for (unsigned int ii=1; ii<logs_list_[i-1].curve.size(); ++ii)
				{
					//
					if (minVal > logs_list_[i-1].curve[ii].value)
						minVal = logs_list_[i-1].curve[ii].value;
					if (maxVal < logs_list_[i-1].curve[ii].value)
						maxVal = logs_list_[i-1].curve[ii].value;
				};

				//here i form the coordinates vector in coords_ struct, then pass the pointer on
				for (unsigned int ii=0; ii<logs_list_[i-1].curve.size(); ++ii) 
				{
					//
					COORD coord_;
					coord_.y = depth.curve[logs_list_[i-1].curve[ii].index];
					coord_.x = logs_list_[i-1].curve[ii].value;

					coords_.push_back(coord_);
				}

				column_list_[i]->setxBorders(minVal, maxVal);
				column_list_[i]->setyBorders(depth.curve[0], depth.curve[depth.curve.size()-1]);
				column_list_[i]->setLegend(logs_list_[i-1].name, logs_list_[i-1].unit);
				strs << column_list_[i]->setInitCurveCoords(coords_);
				
				strs << column_list_[i]->initCoords_[4].y;
				gotit = strs.str();
				//return gotit;
				column_list_[i]->setSize(column_width_, overall_height_);
				column_list_[i]->setCoords(scale_width_ + column_width_*(i-1) + cBorderWidth*(i+1), 
											cBorderWidth);
				column_list_[i]->setHeader(cBorderWidth*(i+1)+scale_width_ + column_width_*(i-1), cBorderWidth, 
												column_width_, header_height_); 

				column_list_[i]->setRowColumn(//coords_, 
												cBorderWidth*(i+1)+scale_width_ + column_width_*(i-1), 
												cBorderWidth*2+ header_height_,
												column_width_, overall_height_ - header_height_ - scroll_control_size_);

				strs << column_list_[i]->column_row_.coords_[6].y;
				gotit = strs.str();
				//return gotit;
			}

			//RestOfSpace - columns[size-1] - the rest of space if left any
			gotit = "end of fn";

			return gotit;

		} 

		//it has to return the number of column that corresponds to the xCoord
		int checkOutBorderCoords(int32_t xCoord) {
			//figure out if xCoord is inside of borders
			for (unsigned int i=1; i<column_list_.size(); ++i) {
				//
				if ((xCoord >= column_list_[i]->cox_ + column_list_[i]->width_ - 2) &&
					(xCoord <= column_list_[i]->cox_ + column_list_[i]->width_ + 2))
					return i;
			}
			return 0;
		}

		std::string prepToResize(int &col_no, int32_t shift) {
			
			std::string gotit;

			int32_t new_col_width = column_list_[col_no]->width_ + shift;

			std::ostringstream strs;

			//here set controls
			hControl_->set

			column_list_[col_no]->setSize(new_col_width, overall_height_);
			
			column_list_[col_no]->setHeader(column_list_[col_no]->cox_, cBorderWidth, 
											new_col_width, header_height_); 
											//logs_list_[col_no-1].name, logs_list_[col_no-1].unit);
			column_list_[col_no]->setRowColumn(//coords_, 
											column_list_[col_no]->cox_, 
											cBorderWidth*2+ header_height_,
											new_col_width, overall_height_ - header_height_ /*- 2*/);

			//now change coordinates for columns with index > than col_no, but not reassemble them

			for (int i = col_no+1; i < column_num_; ++i) {
				//
				column_list_[i]->setCoords(column_list_[i]->cox_ + shift, 
										column_list_[i]->coy_);
				column_list_[i]->setHeader(column_list_[i]->cox_ + shift, cBorderWidth, 
											column_list_[i]->header_.width_, column_list_[i]->header_.height_); 
											//logs_list_[col_no-1].name, logs_list_[col_no-1].unit);
				column_list_[i]->setRowColumn(//coords_, 
											column_list_[i]->cox_ + shift, 
											cBorderWidth*2+ header_height_,
											column_list_[i]->column_row_.width_, column_list_[i]->column_row_.height_ /*- 2*/);
			}
			strs << shift; 
			gotit = strs.str();
			return gotit;

		} //prepToResize

		Control *vControl_;
		Control *hControl_;

		std::vector< Column* > column_list_;
		//Headers headers_;
		int32_t scale_width_; // - const value
		int32_t column_width_; // - just the initial standard value
		int32_t overall_width_; // - const value
		int32_t header_height_; // - variable value, but it is alway same for all the columns
		int32_t overall_height_; // - const value

		//data::las::las_t* las_;
		//data::las::depth_log_t* depth;

		//std::vector<int32_t> coord_vl;
		//std::vector<int32_t> coord_hl;

		int column_num_;

		int32_t border_width_;

	};
}//namespace LogRun