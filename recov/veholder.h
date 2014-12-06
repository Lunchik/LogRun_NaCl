/**/

#include <stdio.h>
#include <string>
#include <vector>
#include <cairo/cairo.h>
//#include <cairoint.h>
#include "Las.h"

/*#include "agg-2.5/agg_basics.h"
#include "agg-2.5/agg_rendering_buffer.h"
#include "agg-2.5/agg_rasterizer_scanline_aa.h"
#include "agg-2.5/agg_scanline_u.h"
#include "agg-2.5/agg_renderer_scanline.h"
#include "agg-2.5/agg_pixfmt_rgba.h"
#include "agg-2.5/agg_platform_support.h"
#include "agg-2.5/agg_slider_ctrl.h"
#include "agg-2.5/agg_cbox_ctrl.h"*/


namespace {

const int32_t cBorderWidth = 0;

const uint32_t kOpaqueColorMask = 0xffff00ff;  // Opaque pixels.
const uint32_t kRedMask = 0xffff0000;
const uint32_t kWhiteMask = 0x00ffffff;
const uint32_t kBlackMask = 0xff000000;
const uint32_t kGreyMask = 0xff808080;

}

/*namespace data { namespace las {
	
	struct data_log : public data_log_t
	{
	public:
		//Constructor - deep copy
	private:
		double minVal;
		double maxVal;
	}
	class logs_section : public logs_section_t 
	{
	public:
		logs_section(){};
		~logs_section();

		

	private:
		depth_log_t depth_;
		logs_list_t logs_list_;
	}
} //las
} //data*/

/*enum { flip_y = true };

namespace agg
{

    class square
    {
    public:
        square(double size) : m_size(size) {}

        template<class Rasterizer, class Scanline, class Renderer> 
        void draw(Rasterizer& ras, Scanline& sl, Renderer& ren, double x, double y)
        {
            ras.reset();
            ras.move_to_d(x*m_size,        y*m_size);
            ras.line_to_d(x*m_size+m_size, y*m_size);
            ras.line_to_d(x*m_size+m_size, y*m_size+m_size);
            ras.line_to_d(x*m_size,        y*m_size+m_size);
            ras.render(sl, ren);
        }

    private:
        double m_size;
    };



    template<class Renderer> class renderer_enlarged
    {
    public:
        renderer_enlarged(Renderer& ren, double size) : 
            m_ren(ren),
            m_square(size), 
            m_size(size) {}

        //--------------------------------------------------------------------
        void color(rgba8 c) { m_color = c; }

        //--------------------------------------------------------------------
        void prepare() {}

        //--------------------------------------------------------------------
        template<class Scanline> void render(const Scanline& sl)
        {
            int y = sl.y();

            unsigned num_spans = sl.num_spans();
            typename Scanline::const_iterator span = sl.begin();

            do
            {
                int x = span->x;
                const typename Scanline::cover_type* covers = span->covers;
                int num_pix = span->len;
                
                do 
                {
                    int a = (*covers++ * m_color.a) >> 8;
                    m_ren.color(rgba8(m_color.r, m_color.g, m_color.b, a));
                    m_square.draw(m_ras, m_sl, m_ren, x, y);
                    ++x;
                }
                while(--num_pix);
            }
            while(--num_spans);
        }

    private:
        rasterizer_scanline_aa<> m_ras;
        scanline_u8 m_sl;
        Renderer&   m_ren;
        square      m_square;
        rgba8       m_color;
        double      m_size;
    };



}

class the_application : public agg::platform_support
{
    double m_x[3];
    double m_y[3];
    double m_dx;
    double m_dy;
    int    m_idx;

    agg::slider_ctrl<agg::rgba8> m_slider1;
    agg::slider_ctrl<agg::rgba8> m_slider2;

public:
    the_application(agg::pix_format_e format, bool flip_y) :
        agg::platform_support(format, flip_y),
        m_slider1(80, 10,    600-10, 19,    !flip_y),
        m_slider2(80, 10+20, 600-10, 19+20, !flip_y)
    {
        m_idx = -1;
        m_x[0] = 57;    m_y[0] = 100;
        m_x[1] = 369;   m_y[1] = 170;
        m_x[2] = 143;   m_y[2] = 310;

        add_ctrl(m_slider1);
        add_ctrl(m_slider2);

        m_slider1.range(8.0, 100.0);
        m_slider1.num_steps(23);
        m_slider1.value(32.0);

        m_slider2.range(0.1, 3.0);
        m_slider2.value(1.0);

        m_slider1.label("Pixel size=%1.0f");
        m_slider2.label("Gamma=%4.3f");

        m_slider1.no_transform();
        m_slider2.no_transform();
    }


    virtual ~the_application()
    {
    }


    virtual void on_init()
    {
    }


    virtual void on_draw()
    {
        typedef agg::renderer_base<agg::pixfmt_rgba32> ren_base;

        agg::pixfmt_rgba32 pixf(rbuf_window());
        ren_base ren(pixf);
        agg::scanline_u8 sl;

        ren.clear(agg::rgba(1,1,1));

        agg::rasterizer_scanline_aa<> ras;

        int size_mul = int(m_slider1.value());

        ras.gamma(agg::gamma_power(m_slider2.value()));


        agg::renderer_enlarged<ren_base> ren_en(ren, size_mul);

        ras.reset();
        ras.move_to_d(m_x[0]/size_mul, m_y[0]/size_mul);
        ras.line_to_d(m_x[1]/size_mul, m_y[1]/size_mul);
        ras.line_to_d(m_x[2]/size_mul, m_y[2]/size_mul);
        ren_en.color(agg::rgba8(0,0,0, 255));
        agg::render_scanlines(ras, sl, ren_en);


        agg::render_scanlines_aa_solid(ras, sl, ren, agg::rgba8(0,0,0));

        ras.gamma(agg::gamma_none());

        agg::path_storage ps;
        agg::conv_stroke<agg::path_storage> pg(ps);
        pg.width(2.0);

        ps.remove_all();
        ps.move_to(m_x[0], m_y[0]);
        ps.line_to(m_x[1], m_y[1]);
        ras.add_path(pg);
        agg::render_scanlines_aa_solid(ras, sl, ren, agg::rgba8(0,150,160, 200));

        ps.remove_all();
        ps.move_to(m_x[1], m_y[1]);
        ps.line_to(m_x[2], m_y[2]);
        ras.add_path(pg);
        agg::render_scanlines_aa_solid(ras, sl, ren, agg::rgba8(0,150,160, 200));

        ps.remove_all();
        ps.move_to(m_x[2], m_y[2]);
        ps.line_to(m_x[0], m_y[0]);
        ras.add_path(pg);
        agg::render_scanlines_aa_solid(ras, sl, ren, agg::rgba8(0,150,160, 200));

        // Render the controls
        //agg::render_ctrl(ras, sl, ren, m_slider1);
        //agg::render_ctrl(ras, sl, ren, m_slider2);
    }




    virtual void on_mouse_button_down(int x, int y, unsigned flags)
    {
        if(flags & agg::mouse_left)
        {
            unsigned i;
            for (i = 0; i < 3; i++)
            {
                if(sqrt( (x-m_x[i]) * (x-m_x[i]) + (y-m_y[i]) * (y-m_y[i]) ) < 10.0)
                {
                    m_dx = x - m_x[i];
                    m_dy = y - m_y[i];
                    m_idx = i;
                    break;
                }
            }
            if(i == 3)
            {
                if(agg::point_in_triangle(m_x[0], m_y[0], 
                                          m_x[1], m_y[1],
                                          m_x[2], m_y[2],
                                          x, y))
                {
                    m_dx = x - m_x[0];
                    m_dy = y - m_y[0];
                    m_idx = 3;
                }
            }
        }
    }


    virtual void on_mouse_move(int x, int y, unsigned flags)
    {
        if(flags & agg::mouse_left)
        {
            if(m_idx == 3)
            {
                double dx = x - m_dx;
                double dy = y - m_dy;
                m_x[1] -= m_x[0] - dx;
                m_y[1] -= m_y[0] - dy;
                m_x[2] -= m_x[0] - dx;
                m_y[2] -= m_y[0] - dy;
                m_x[0] = dx;
                m_y[0] = dy;
                force_redraw();
                return;
            }

            if(m_idx >= 0)
            {
                m_x[m_idx] = x - m_dx;
                m_y[m_idx] = y - m_dy;
                force_redraw();
            }
        }
        else
        {
            on_mouse_button_up(x, y, flags);
        }
    }

    virtual void on_mouse_button_up(int x, int y, unsigned flags)
    {
        m_idx = -1;
    }
}; //
*/

namespace LogRun {

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

		std::string setPixels(std::string legend) {

			std::string gotit;
			//pixel_buffer_.assign((width_)*(height_), kGreyMask);
			cairo_surf_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width_, height_);
			cairo_cont_ = cairo_create(cairo_surf_);

			//cairo_text_extents_t extents;
			//double x,y;
			//const char* utf8 = "he";

			//Grey Border
			cairo_set_source_rgba (cairo_cont_, 0.8, 0.8, 0.8, 1);
			cairo_set_line_width (cairo_cont_, 1.0);
			cairo_move_to (cairo_cont_, 0, 0);
			cairo_line_to (cairo_cont_, width_, 0);
			cairo_line_to (cairo_cont_, width_, height_);
			cairo_line_to (cairo_cont_, 0, height_);
			cairo_line_to (cairo_cont_, 0, 0);
			cairo_stroke (cairo_cont_);

			//Black Border
			cairo_set_source_rgba (cairo_cont_, 0, 0, 0, 1);
			cairo_set_line_width (cairo_cont_, 1.0);
			cairo_move_to (cairo_cont_, 1, 1);
			cairo_line_to (cairo_cont_, width_-1, 1);
			cairo_line_to (cairo_cont_, width_-1, height_-1);
			cairo_line_to (cairo_cont_, 1, height_-1);
			cairo_line_to (cairo_cont_, 1, 1);
			cairo_stroke (cairo_cont_);

			cairo_pattern_t *pat;

			pat = cairo_pattern_create_linear ((double)width_-4, (double)height_-4,  0.0, 0.0);
			cairo_pattern_add_color_stop_rgba (pat, 1, 0.5, 0.5, 0.5, 1);
			cairo_pattern_add_color_stop_rgba (pat, 0.2, 0.8, 0.8, 0.8, 1);
			cairo_pattern_add_color_stop_rgba (pat, 0, 0.7, 0.7, 0.7, 1);
			cairo_rectangle (cairo_cont_, 2, 2, (double)width_-2, (double)height_-2);
			cairo_set_source (cairo_cont_, pat);
			cairo_fill (cairo_cont_);
			cairo_pattern_destroy (pat);

			gotit = "57";
			//return gotit; //returns

			gotit = "64";

			cairo_surface_flush(cairo_surf_);

			unsigned char * pix_buf_=cairo_image_surface_get_data(cairo_surf_);
			pixel_buffer_ = (uint32_t*)pix_buf_;//static_cast<uint32_t*>(pix_buf_);
			gotit = "end";
			return gotit;
			/*agg::rasterizer_scanline_aa<> ras;
			agg::scanline_u8 sl;
			ras.reset();
			agg::renderer_enlarged<renderer> ren_en(ren, size_mul);
			ren_en.color(agg::rgba8(0,0,0, 255));
			ras.render(sl, ren_en);
			the_application app(agg::pix_format_rgba32, flip_y);
			if(app.init(600, 400, agg::window_resize)){
				if (app.run() == -1)
				{
					gotit = "app.init";
					return gotit;
				}
				agg::rendering_buffer rend_b_ = app.rbuf_window();
				pixel_buffer_ = (uint32_t*)rend_b_.buf();
			};
			gotit = "381";
			return gotit;*/
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
		std::vector<std::string> legend_;
		uint32_t* pixel_buffer_;
		cairo_surface_t* cairo_surf_;
		cairo_t* cairo_cont_;
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

		void setPixelsScale() {
			//pixel_buffer_.assign((width_)*(height_), kRedMask);
			cairo_surf_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width_, height_);
			cairo_cont_ = cairo_create(cairo_surf_);

			//Grey Border
			cairo_set_source_rgba (cairo_cont_, 0.8, 0.8, 0.8, 1);
			cairo_set_line_width (cairo_cont_, 1.0);
			cairo_move_to (cairo_cont_, 0, 0);
			cairo_line_to (cairo_cont_, width_, 0);
			cairo_line_to (cairo_cont_, width_, height_);
			cairo_line_to (cairo_cont_, 0, height_);
			cairo_line_to (cairo_cont_, 0, 0);
			cairo_stroke (cairo_cont_);

			//Black Border
			cairo_set_source_rgba (cairo_cont_, 0, 0, 0, 1);
			cairo_set_line_width (cairo_cont_, 1.0);
			cairo_move_to (cairo_cont_, 1, 1);
			cairo_line_to (cairo_cont_, width_-1, 1);
			cairo_line_to (cairo_cont_, width_-1, height_-1);
			cairo_line_to (cairo_cont_, 1, height_-1);
			cairo_line_to (cairo_cont_, 1, 1);
			cairo_stroke (cairo_cont_);

			cairo_pattern_t *pat;

			pat = cairo_pattern_create_linear ((double)width_-4, (double)height_-4,  0.0, 0.0);
			cairo_pattern_add_color_stop_rgba (pat, 1, 0.7, 0.7, 0.7, 1);
			cairo_pattern_add_color_stop_rgba (pat, 0.2, 0.8, 0.8, 0.8, 1);
			cairo_pattern_add_color_stop_rgba (pat, 0, 0.7, 0.7, 0.7, 1);
			cairo_rectangle (cairo_cont_, 2, 2, (double)width_-2, (double)height_-2);
			cairo_set_source (cairo_cont_, pat);
			cairo_fill (cairo_cont_);
			cairo_pattern_destroy (pat);

			cairo_surface_flush(cairo_surf_);

			unsigned char * pix_buf_=cairo_image_surface_get_data(cairo_surf_);
			pixel_buffer_ = (uint32_t*)pix_buf_;//static_cast<uint32_t*>(pix_buf_);
		}

		void setPixelsColumn(std::vector<COORD> coords_) {
			//pixel_buffer_.assign((width_)*(height_), kRedMask);
			cairo_surf_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width_, height_);
			cairo_cont_ = cairo_create(cairo_surf_);

			//Grey Border
			cairo_set_source_rgba (cairo_cont_, 0.8, 0.8, 0.8, 1);
			cairo_set_line_width (cairo_cont_, 1.0);
			cairo_move_to (cairo_cont_, 0, 0);
			cairo_line_to (cairo_cont_, width_, 0);
			cairo_line_to (cairo_cont_, width_, height_);
			cairo_line_to (cairo_cont_, 0, height_);
			cairo_line_to (cairo_cont_, 0, 0);
			cairo_stroke (cairo_cont_);

			//Black Border
			cairo_set_source_rgba (cairo_cont_, 0, 0, 0, 1);
			cairo_set_line_width (cairo_cont_, 1.0);
			cairo_move_to (cairo_cont_, 1, 1);
			cairo_line_to (cairo_cont_, width_-1, 1);
			cairo_line_to (cairo_cont_, width_-1, height_-1);
			cairo_line_to (cairo_cont_, 1, height_-1);
			cairo_line_to (cairo_cont_, 1, 1);
			cairo_stroke (cairo_cont_);

			//Curves
			cairo_set_source_rgba (cairo_cont_, 0.8, 0.1, 0.1, 1);
			cairo_set_line_width (cairo_cont_, 1.0);
			cairo_move_to (cairo_cont_,coords_[0].x,coords_[0].y);
			for (unsigned int ii = 1; ii<coords_.size(); ++ii)
			{
				cairo_line_to (cairo_cont_,coords_[ii].x,coords_[ii].y);
			}
			
			cairo_stroke (cairo_cont_);

			cairo_surface_flush(cairo_surf_);

			unsigned char * pix_buf_=cairo_image_surface_get_data(cairo_surf_);
			pixel_buffer_ = (uint32_t*)pix_buf_;//static_cast<uint32_t*>(pix_buf_);
		}

		//coords_t coords_;
		int32_t cox_;
		int32_t coy_;
		//size_type size_;
		int32_t width_;
		int32_t height_;
		//std::vector<uint32_t> pixel_buffer_;
		cairo_surface_t* cairo_surf_;
		cairo_t* cairo_cont_;
		uint32_t* pixel_buffer_;
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

		struct coord {
			double x;
			double y;
		};
		//std::vector<coord> coords_;

		void setxBorders(double minVal, double maxVal) {
			//
			minVal_ = minVal;
			maxVal_ = maxVal;
		}

		void setSize(int32_t w, int32_t h) {
			width_ = w;
			height_ = h;
		}

		void setCoords(int32_t x, int32_t y) {
			cox_ = x;
			coy_ = y;
		}

		std::string setHeader(int32_t x, int32_t y,
					   int32_t w, int32_t h,/*
					   std::vector<*/std::string legend) {
			header_.setSize(w, h);
			header_.setCoords(x, y);
			//header_.legend_ = legend;
			return header_.setPixels(legend);
		}

		void setRowScale(int32_t x, int32_t y,
					int32_t w, int32_t h) {
			column_row_.setSize(w, h);
			column_row_.setCoords(x, y);
			column_row_.setPixelsScale();
		}

		void setRowColumn(std::vector<COORD> coords_, int32_t x, int32_t y,
					int32_t w, int32_t h) {
			column_row_.setSize(w, h);
			column_row_.setCoords(x, y);
			column_row_.setPixelsColumn(coords_);
		}

		/*void setPixels() {
			//set header pixels
			header_.setPixels(); //later I'll add name and unit
			//set row pixels
			column_row_.setPixels();
		}*/

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
		std::vector<coord> coords_;
	};

	struct Columns {
	public:
		Columns() : column_list_(0) {};
		/*Columns(int col_num_) {
			//create a vector<Column> of needed size
			column_list_.reserve(col_num_);
			for (int i=0; i<col_num_; ++i)
				column_list_[i] = new Column();
		};*/
		~Columns();

		int setColList(int col_num_)
		{
			//create a vector<Column> of needed size
			//return (int)column_list_.max_size();
			column_list_.reserve(col_num_);
			return -1;
			for (int i=0; i<col_num_; ++i) {
				Column* col_ = new Column();
				//return i;
				column_list_[i] = col_;
				return i;
			};
			return -1;
		};

		void setSize(int32_t w, int32_t h) {
			width_ = w;
			height_ = h;
		}

		//size_type size_;
		int32_t width_;
		int32_t height_;
		//Scale scale_;
		std::vector< Column* > column_list_;
		//RestOfSpace empty_space_;
	};

	class VEHolder {
	public:
		//every element has its' own pixel_buffer_ and coordinates
		VEHolder(){};
		VEHolder(/*widths*/int32_t s_w, int32_t c_w, int32_t o_w,
				 /*heights*/int32_t h_h, int32_t o_h)
				 : scale_width_(s_w)
				 , column_width_(c_w)
				 , overall_width_(o_w)
				 , header_height_(h_h)
				 , overall_height_(o_h)
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
			data::las::logs_section_t& logs = las_->logs_section();
			data::las::depth_log_t& depth = logs.depth_log();
			double lDepth = depth.curve[depth.curve.size()-1] -depth.curve[0];
			double yScale = (overall_height_ - header_height_ - 2)/lDepth;
			data::las::logs_section_t::logs_list_t logs_list_ = logs.logs_list();
			column_num_ = logs_list_.size() + 1;
			border_width_ = cBorderWidth;

			std::string gotit = "170";
			//return column_num_;
			
			//horisontal lines
			/*for (int iii=0; iii<cBorderWidth; ++iii) {
				coord_hl.push_back(iii);
			};
			for (int iii=0; iii<cBorderWidth; ++iii) {
				coord_hl.push_back(header_height_ + iii);
			};
			//vertical lines - the first one
			for (int iii=0; iii<cBorderWidth; ++iii) {
				coord_vl.push_back(iii);
			}*/
			setColList(column_num_);
			//return (int)column_list_.size(); it's 5 - ok
			//return (int)result;//columns_->column_list_.size();
			gotit = "174"; //there is return 
			//return gotit;
			//Scale column - column[0] - init, fill coordinates, size, labels
			gotit = "176"; //there is return
			//return gotit;
			
			//vertical line after the scale column
			/*for (int iii=0; iii<cBorderWidth; ++iii) {
				coord_vl.push_back(iii+scale_width_);
			}*/
			column_list_[0]->setxBorders(0, 0);
			column_list_[0]->setSize(scale_width_, overall_height_);
			gotit = "194"; //there is return
			//return gotit;
			column_list_[0]->setCoords(cBorderWidth, cBorderWidth);
			gotit = "197"; //there is return
			//return gotit;
			gotit = column_list_[0]->setHeader(cBorderWidth, cBorderWidth, 
											   scale_width_, header_height_, 
											  logs.depth_log().name/*{, logs.depth_log().unit}*/);
			gotit = "202"; //there is return
			//return gotit;
			column_list_[0]->setRowScale(cBorderWidth, cBorderWidth*2+header_height_,
											scale_width_, overall_height_ - header_height_ - cBorderWidth*2);
			//columns_.column_list_[0].setPixels();
			//Columns - real columns

			for (int i = 1; i<column_num_; ++i)
			{
				//vertical line after the scale column
				/*for (int iii=0; iii<cBorderWidth; ++iii) {
					coord_vl.push_back(iii+cBorderWidth*(i+1)+column_width_*i+scale_width_);
				}*/
				//form the pixel coordinates from las data and depth logs.
				double minVal = logs_list_[i-1].curve[0].value;
				double maxVal = logs_list_[i-1].curve[0].value;
				std::vector<coord> coords_;
				for (unsigned int ii=1; ii<logs_list_[i-1].curve.size(); ++ii)
				{
					//
					if (minVal > logs_list_[i-1].curve[ii].value)
						minVal = logs_list_[i-1].curve[ii].value;
					if (maxVal < logs_list_[i-1].curve[ii].value)
						maxVal = logs_list_[i-1].curve[ii].value;
				};
				double lWidth = maxVal - minVal;
				double xScale = (column_width_-10)/lWidth;
				//here i form the coordinates vector in coords_ struct, then pass the pointer on
				for (unsigned int ii=0; ii<logs_list_[i-1].curve.size(); ++ii) 
				{
					//
					COORD coord_;
					coord_.y = (depth.curve[logs_list_[i-1].curve[ii].index] - depth.curve[0])*yScale;
					coord_.x = (logs_list_[i-1].curve[ii].value - minVal)*xScale+5;

					coords_.push_back(coord_);
				}

				//column_list_[i]->setxBorders(minVal, maxVal);
				column_list_[i]->setSize(column_width_, overall_height_);
				column_list_[i]->setCoords(scale_width_ + column_width_*(i-1) + cBorderWidth*(i+1), 
											cBorderWidth);
				column_list_[i]->setHeader(cBorderWidth*(i+1)+scale_width_ + column_width_*(i-1), cBorderWidth, 
												column_width_, header_height_, 
												logs_list_[i-1].name/*{, logs_list_[i-1].unit}*/);
				column_list_[i]->setRowColumn(coords_, 
												cBorderWidth*(i+1)+scale_width_ + column_width_*(i-1), 
												cBorderWidth*2+ header_height_,
												column_width_, overall_height_ - header_height_ /*- 2*/);
			}

			//RestOfSpace - columns[size-1] - the rest of space if left any
			gotit = "end of fn";

			return gotit;

		} 



		std::vector< Column* > column_list_;
		//Headers headers_;
		int32_t scale_width_;
		int32_t column_width_;
		int32_t overall_width_;
		int32_t header_height_;
		int32_t overall_height_;

		//std::vector<int32_t> coord_vl;
		//std::vector<int32_t> coord_hl;

		int column_num_;

		int32_t border_width_;

	};
}//namespace LogRun