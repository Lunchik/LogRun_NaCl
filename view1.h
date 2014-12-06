 /**
  * @file view1.h
  * @author garipova.nailya@tgtoil.com
  * @author Copyright(C) 2012 TGT Oil & Gas Services. All right reserved.
  */

#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/image_data.h"
//#include "ppapi/cpp/input_event.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/point.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/mouse_cursor.h"

#include "ppapi/cpp/rect.h"
#include "veholder.h"

namespace pp {
class Graphics2D;
class ImageData;
class Instance;
//class KeyboardInputEvent;
class Rect;
class Size;
}  // namespace pp

namespace LogRun {

class VEHolder;

class View {
 public:
  explicit View(pp::Instance* instance);
  ~View();

  //int DrawFlush();

  void set_hborder_coords_(std::vector<int32_t> hborder_coords){
	  hborder_coords_ = hborder_coords;
  }

  void set_vborder_coords_(std::vector<int32_t> vborder_coords){
	  vborder_coords_ = vborder_coords;
  }

  void set_flush_pending(bool flush_pending) {
      flush_pending_ = flush_pending;
    }

  pp::Size GetSize() const;

  void Draw();
  int DrawLas(VEHolder* veholder);
  void UpdateView(const pp::Rect& position,
                    const pp::Rect& clip,
                    pp::Instance* instance);
  void UpdateSmallView(const pp::Rect& position,
                      pp::Instance* instance);

  //data::las::las_t las_;

 private:
  pp::Instance* const instance_;  // weak
  // Create and initialize the 2D context used for drawing.
  void CreateContext(const pp::Size& size, pp::Instance* instance);
  // Destroy the 2D drawing context.
  void DestroyContext();
  // Push the pixels to the browser, then attempt to flush the 2D context.  If
  // there is a pending flush on the 2D context, then update the pixels only
  // and do not flush.
  void FlushPixelBuffer();
  bool IsContextValid() const;
  //Geometry for drawing
  std::vector<int32_t> hborder_coords_;
  std::vector<int32_t> vborder_coords_;
  // Drawing stuff
  bool flush_pending_;
  pp::Graphics2D* graphics_2d_context_;
  pp::ImageData* pixel_buffer_;
  //int32_t scale_width_;
  //int32_t columns_width_;
  //int32_t header_height_;
};

}  // namespace fileIO