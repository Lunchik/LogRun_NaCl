 /**
  * @file logrun.cc
  * @author garipova.nailya@tgtoil.com
  * @author Copyright(C) 2012 TGT Oil & Gas Services. All right reserved.
  */


//#include <cstdio>
//#include <iostream>
#include "ctype.h"
#include "time.h"
#include <iostream>
#include <sstream>
#include <iterator>

#include "ppapi/cpp/input_event.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/var_array_buffer.h"
#include "ppapi/c/ppb_var_array_buffer.h"
#include "ppapi/c/pp_file_info.h"
#include "ppapi/c/ppb_file_io.h"
#include "ppapi/cpp/file_io.h"
#include "ppapi/cpp/file_ref.h"
#include "ppapi/cpp/file_system.h"
#include "ppapi/cpp/completion_callback.h"
//#include "base64enc2.h"
#include "base64.h"
#include "uu.h"
#include "XUnzip.h"

#include <errno.h>

//#include "las_file/Las.h"

//#include "unzipper_cpp_sve/XZip.h"



namespace {

const char* const kDidChangeView = "DidChangeView";
const char* const kHandleInputEvent = "DidHandleInputEvent";
const char* const kDidChangeFocus = "DidChangeFocus";
const char* const kHaveFocus = "HaveFocus";
const char* const kDontHaveFocus = "DontHaveFocus";

const int32_t LEFT_LINE = 0;
const int32_t INIT_SCALE_WIDTH = 80;
const int32_t INIT_COL_WIDTH = 100;

const int32_t TOP_LINE = 0;
const int32_t TOOLBAR_LLINE = TOP_LINE + 30;
const int32_t HEADER_LLINE = TOOLBAR_LLINE + 50;

std::string ModifierToString(uint32_t modifier) {
  std::string s;
  if (modifier & PP_INPUTEVENT_MODIFIER_SHIFTKEY) {
    s += "shift ";
  }
  if (modifier & PP_INPUTEVENT_MODIFIER_CONTROLKEY) {
    s += "ctrl ";
  }
  if (modifier & PP_INPUTEVENT_MODIFIER_ALTKEY) {
    s += "alt ";
  }
  if (modifier & PP_INPUTEVENT_MODIFIER_METAKEY) {
    s += "meta ";
  }
  if (modifier & PP_INPUTEVENT_MODIFIER_ISKEYPAD) {
    s += "keypad ";
  }
  if (modifier & PP_INPUTEVENT_MODIFIER_ISAUTOREPEAT) {
    s += "autorepeat ";
  }
  if (modifier & PP_INPUTEVENT_MODIFIER_LEFTBUTTONDOWN) {
    s += "left-button-down ";
  }
  if (modifier & PP_INPUTEVENT_MODIFIER_MIDDLEBUTTONDOWN) {
    s += "middle-button-down ";
  }
  if (modifier & PP_INPUTEVENT_MODIFIER_RIGHTBUTTONDOWN) {
    s += "right-button-down ";
  }
  if (modifier & PP_INPUTEVENT_MODIFIER_CAPSLOCKKEY) {
    s += "caps-lock ";
  }
  if (modifier & PP_INPUTEVENT_MODIFIER_NUMLOCKKEY) {
    s += "num-lock ";
  }
  return s;
}

std::string MouseButtonToString(PP_InputEvent_MouseButton button) {
  switch (button) {
    case PP_INPUTEVENT_MOUSEBUTTON_NONE:
      return "None";
    case PP_INPUTEVENT_MOUSEBUTTON_LEFT:
      return "Left";
    case PP_INPUTEVENT_MOUSEBUTTON_MIDDLE:
      return "Middle";
    case PP_INPUTEVENT_MOUSEBUTTON_RIGHT:
      return "Right";
    default:
      std::ostringstream stream;
      stream << "Unrecognized ("
             << static_cast<int32_t>(button)
             << ")";
      return stream.str();
  }
}

} // namespace

	static const std::string base64_chars = 
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

//extern int errno;

namespace LogRun {

namespace AsyncCallbacks {

     // Callback that is called as a result of pp::FileSystem::Open
     /*void FileSystemOpenCallback(void* data, int32_t result) {
       if (result != PP_OK)
         return;
       LRInstance* instLR_ = static_cast<LRInstance*>(data);
	   //instLR_->PostMessage(pp::Var("We've opened the FS")); //we did
       instLR_->OpenFile();
     }*/

     // Callback that is called as a result of pp::FileIO::Read
     /*void ReadCallback(void* data, int32_t bytes_read) {
	   LRInstance* instLR_ = static_cast<LRInstance*>(data);
	   if (bytes_read < 0)
		   instLR_->PostMessage(pp::Var(bytes_read));
	   //instLR_->PostMessage(pp::Var(bytes_read));
	   //sleep(60);
	   //instLR_->PostMessage(pp::Var((int32_t)instLR_->bytes_buffer_.length()));
       

       //instLR_->PostMessage(pp::Var("We're at the ReadCallback"));
	   instLR_->bytes_to_read_ -= bytes_read;
       if (instLR_->bytes_to_read_ ==  0) {
         // File has been read to completion.  Parse the bytes to get the scores.
         //instLR_->SendFile();
		   //instLR_->FileProcessing();
	     switch(instLR_->data_format_) {
			 case 1: //lvz
				 instLR_->UnzipLVZFile();
				 break;
			 case 2: //las
				 instLR_->ParseLas();
				 break;
			 default:
				 instLR_->PostMessage(pp::Var("Error in data_format_"));
				 break;
		 };
       } else {
         instLR_->offset_ += bytes_read;
         instLR_->file_io_->Read(instLR_->offset_,
                              &instLR_->bytes_buffer_[instLR_->offset_],
                              instLR_->bytes_to_read_,
                              pp::CompletionCallback(ReadCallback, instLR_));
       }
     }*/

     // Callback that is called as a result of pp::FileIO::Query
     /*void QueryCallback(void* data, int32_t result) {
		 LRInstance* instLR_ = static_cast<LRInstance*>(data);
		 //instLR_->PostMessage(pp::Var(result));
		 if ((result != PP_OK)){
			 instLR_->PostMessage(pp::Var(result));
			 return;
		 }

	   instLR_->PostMessage(pp::Var((int32_t)instLR_->file_info_.size));
       instLR_->bytes_to_read_ = instLR_->file_info_.size;
       instLR_->offset_ = 0;
       instLR_->bytes_buffer_.resize(instLR_->bytes_to_read_);

       // Check if there is anything to read.
       if (instLR_->bytes_to_read_ == 0) {
         instLR_->file_io_->Read(instLR_->offset_,
                              &instLR_->bytes_buffer_[0],
                              instLR_->bytes_to_read_,
                              pp::CompletionCallback(ReadCallback, instLR_));
       }
     }*/

	 /*void QueryCallback(void* data, int32_t result)
	 { 
		LRInstance* instLR_ = static_cast<LRInstance*>(data);
		if (result != PP_OK)
			instLR_->PostMessage(pp::Var(result));
		instLR_->bytes_to_read_ = instLR_->message_contents_.length();
        instLR_->offset_ = 0;
		//instLR_->PostMessage(pp::Var((int32_t)instLR_->file_info_.size));
		instLR_->bytes_buffer_.resize(instLR_->bytes_to_read_);
		//instLR_->PostMessage(pp::Var((int32_t)instLR_->bytes_to_read_));
		instLR_->ReadFromFile();
	 }*/

	 /*void FlushCallback(void* data, int32_t) {
		 LRInstance* instLR_ = static_cast<LRInstance*>(data);
		 //instLR_->ReadFromFile();
		 //instLR_->PostMessage(pp::Var("we got here"));
		 //Don't go to Query, go to ParseLas if data_format_ == las or to UnzipLVZFile. 
		 instLR_->file_io_->Query(&instLR_->file_info_, pp::CompletionCallback(QueryCallback, instLR_));
		 //instLR_->UnzipMyFile();
		}*/
	
	 /*void WriteCallback(void* data, int32_t bytes_written) {
		if (bytes_written < 0)
			return;  // error
		LRInstance* instLR_ = static_cast<LRInstance*>(data);
		
		//instLR_->PostMessage(pp::Var((int32_t)instLR_->offset_)); //made it till here
		instLR_->offset_ += bytes_written;
		if (instLR_->offset_ == instLR_->message_contents_.length()) {
			//instLR_->PostMessage(pp::Var(instLR_->i_test)); //made it till here
			instLR_->file_io_->Flush(pp::CompletionCallback(FlushCallback, instLR_));
		} else {
			// Not all the bytes to be written have been written, so call
			// pp::FileIO::Write again.
			instLR_->i_test++;
			instLR_->file_io_->Write(instLR_->offset_, &instLR_->message_contents_[instLR_->offset_],
                          instLR_->message_contents_.length() - instLR_->offset_,
                          pp::CompletionCallback(WriteCallback, instLR_));
		}
	}*/

     // Callback that is called as a result of pp::FileIO::Open
     /*void FileOpenCallback(void*data, int32_t result) {
	   LRInstance* instLR_ = static_cast<LRInstance*>(data);
       if ((result != PP_OK)) {
		 //std::string mes1("File IO Open error = ");
		 //std::string result_str = result;
		 //mes1.append(result_str);
		 instLR_->PostMessage(pp::Var("did we open it?"));
         return;
       }

	   //instLR_->PostMessage(pp::Var("We've opened the file (callback)")); //we did
       // Query the file in order to get the file size.
       //instLR_->file_io_->Query(&instLR_->file_info_, pp::CompletionCallback(QueryCallback,
                                                                       //instLR_));
	   instLR_->SaveTempFile();
     }*/



}//namespace AsyncCallbacks


/// The Instance class.  One of these exists for each instance of your NaCl
/// module on the web page.  The browser will ask the Module object to create
/// a new Instance for each occurence of the <embed> tag that has these
/// attributes:
///     type="application/x-nacl"
///     src="hello_tutorial.nmf"
/// To communicate with the browser, you must override HandleMessage() for
/// receiving messages from the browser, and use PostMessage() to send messages
/// back to the browser.  Note that this interface is asynchronous.

LRInstance::LRInstance(PP_Instance instance)
        : pp::Instance(instance),
        //file_io_(NULL),
        //file_ref_(NULL),
        file_system_(NULL),
		//thread_started_(false),
		fd_(0),
		//main_thread_(0),
		written_(0),
        //file_name_(NULL),
		//data_format_(NULL),
		las_(),
		//veholder_(),
		view_(NULL),
		app_resizing_(false) {//,
		//directory_reader_(this){
        //nothing yet
			RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE | PP_INPUTEVENT_CLASS_WHEEL
								| PP_INPUTEVENT_CLASS_TOUCH);
			//RequestFilteringInputEvents();
    }
LRInstance::~LRInstance() {
        //destruction events
		//delete veholder_;
		/*if (main_thread_) {
			pthread_join(main_thread_, NULL);
		}*/
		delete view_;
        //file_io_->Close();
        //delete file_io_;
        //delete file_ref_;
        delete file_system_;
    }
  /// Handler for messages coming in from the browser via postMessage().  The
  /// @a var_message can contain anything: a JSON string; a string that encodes
  /// method names and arguments; etc.  For example, you could use
  /// JSON.stringify in the browser to create a message that contains a method
  /// name and some parameters, something like this:
  ///   var json_message = JSON.stringify({ "myMethod" : "3.14159" });
  ///   nacl_module.postMessage(json_message);
  /// On receipt of this message in @a var_message, you could parse the JSON to
  /// retrieve the method name, match it to a function call, and then call it
  /// with the parameter.
  /// @param[in] var_message The message posted by the browser.

bool LRInstance::Init(uint32_t argc, const char* argn[], const char* argv[]) {

	PostMessage(pp::Var((int32_t)errno));
	proxy_ = KernelProxy::KPInstance();
	PostMessage(pp::Var("created proxy_"));
	PostMessage(pp::Var("created MainThreadRunner"));
	//int32_t rv = file_system_->Open(
      //1024, pp::CompletionCallback(AsyncCallbacks::FileSystemOpenCallback, this));
	//if (rv != PP_OK_COMPLETIONPENDING) {
		//PostMessage(pp::Var("ERROR: Could not open local persistent file system."));
		//return true;
	//}
	//Init variable width fields
	scale_width_=100;
	columns_width_=120;
	header_height_ = 80;
	//columns_height_;
	//Init viewer class
    view_ = new View(this);
	//veholder_ = new VEHolder();
	//Init borders of interface coordinates
	//InitBorderCoordinates();
    file_name_ = "/container_temp";
    file_system_ = new pp::FileSystem(this, PP_FILESYSTEMTYPE_LOCALTEMPORARY);
	PostMessage(pp::Var("After creatin fs"));
    //file_ref_ = new pp::FileRef(*file_system_, file_name_.c_str());
	return true;
}

void LRInstance::DidChangeView(const pp::Rect& position,
                         const pp::Rect& clip) {
  PostMessage(pp::Var("DidChangeView"));
  pp::Size view_size = view_->GetSize();

  const bool view_was_empty = view_size.IsEmpty();
  //view_->UpdateView(position, clip, this);
  if (view_was_empty) {
	view_->UpdateView(position, clip, this);
	view_->Draw();
  }

}

  void LRInstance::HandleMessage(const pp::Var& var_message) {


	//if (!var_message.is_array_buffer())
      //return;
	//int i;
	//std::vector<unsigned char> out_cont_;
	  
	std::string message = var_message.AsString(); //here we have a binary string
	//std::stringstream mes_stream_(message_contents_);
	//std::stringstream out_stream_(out_cont_);
	//std::istream_iterator<std::string> begin(mes_stream_);
	//std::istream_iterator<std::string> end;
	//std::ostream_iterator<std::vector>(ofs)

	//out_cont_.reserve(1024*1024*1024);
	//view_ = new View(this);
	//view = new pp::View();
	//data::las::las_t las_;

	base64::decode(message.begin(), 
					message.end(), 
					std::back_inserter(message_contents_));

	/*After decoding we can have 2 types of contents:
		.las file - begins with "# " or "~"
		.lvz file - an archive, begins with "PK"
	*/	
	//PostMessage(pp::Var(message_contents_.substr(0,8)));
	mes_len_ = message_contents_.length();
	std::string beginning = message_contents_.substr(0, 2);
	if (beginning == "PK") {
		data_format_ = 1; //lvz
		//PostMessage(pp::Var("LVZ"));
	} else 
		if ((beginning == "# ") || (beginning == "~V")) {
			data_format_ = 2; //las
			//PostMessage(pp::Var("LAS"));
		} else PostMessage(pp::Var("File format identification error"));
	
	//PostMessage(pp::Var("before creating pepper_mount"));
	//opening fs

	/*if (0 == pthread_create(&main_thread_, NULL, thread_static, this)) {
        thread_started_ = true;
	}*/
	//PepperMount* pepper_mount = new PepperMount(runner_, file_system_, 5 * 1024 * 1024);
	//PostMessage(pp::Var("created pepper_mount"));
	//pepper_mount->SetDirectoryReader(&directory_reader_);
	//PostMessage(pp::Var("set directory_reader_"));
	//pepper_mount->SetPathPrefix("/LogRunFiles");
	//PostMessage(pp::Var("set path prefix"));

	//PostMessage(pp::Var((int32_t)errno)); //2
	
	//proxy_->mkdir("/LogRunFiles", 0777);
	//PostMessage(pp::Var("mkdir"));
	//int res = proxy_->mount("/LogRunFiles", pepper_mount);
	//PostMessage(pp::Var("mounted pepper_mount"));

	//PostMessage(pp::Var((int32_t)errno));

	//now opening file
	fd_ = proxy_->open(file_name_, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	//PostMessage(pp::Var("opened file"));

	//PostMessage(pp::Var((int32_t)errno));

	written_ = proxy_->write(fd_, &message_contents_[0], (size_t)mes_len_);
	if ( written_ <= 0) {
		PostMessage(pp::Var("something's wrong with writing operation"));
		//PostMessage(pp::Var(res));
	}

	//PostMessage(pp::Var((int32_t)written_));
	//PostMessage(pp::Var((int32_t)errno));

	off_t offset = lseek(fd_, 0, 0);

	PostMessage(pp::Var((int32_t)offset));
	//PostMessage(pp::Var("now read"));
	//PostMessage(pp::Var((int32_t)errno));

	bytes_buffer_.resize(mes_len_);

	//int read_ = read(fd_, &bytes_buffer_[0], mes_len_);
	//PostMessage(pp::Var((int32_t)errno));
	//if (read_ <=0)
		//PostMessage(pp::Var("smths wrong with reading op"));
	//PostMessage(pp::Var(test_message_));
	//PostMessage(pp::Var(mes_len_));
	//PostMessage(pp::Var(read_));
	//PostMessage(pp::Var(bytes_buffer_));
	//int32_t rv = file_system_->Open(
      //1024, pp::CompletionCallback(AsyncCallbacks::FileSystemOpenCallback, this));
	//if (rv != PP_OK_COMPLETIONPENDING) {
		//PostMessage(pp::Var("ERROR: Could not open local persistent file system."));
		//return true;
	//}
	//if we have las => go to parse las and read file there.
	//lvz => pass the fd_ to the unzip funclion along with the pointer to proxy_
	switch(data_format_) {
		case 1:
			UnzipLVZFile();
			break;
		case 2:
			ParseLas();
			break;
		default:
			PostMessage(pp::Var("Smth's wrong with file format"));
			break;
	}

  } //HandleMessage

  


  bool LRInstance::HandleInputEvent(const pp::InputEvent& IEvent) {
	pp::MouseInputEvent mEvent;
	if (!app_resizing_) col_no=0;
	//PostMessage(pp::Var(kHandleInputEvent));
    switch (IEvent.GetType()) {
      case PP_INPUTEVENT_TYPE_UNDEFINED:
        break;
      case PP_INPUTEVENT_TYPE_MOUSEDOWN:	//I'll need that
		//if mouse coordinates match with one of border lines
		//get position returns int
		mEvent = pp::MouseInputEvent(IEvent);
		//int col_no = 0;
		if ((!app_resizing_) && ((col_no = veholder_->checkOutBorderCoords(mEvent.GetPosition().x())) != 0)) {
			PostMessage(pp::Var((int32_t)col_no));
			//pp::ImageData temp_data = pp::ImageData();
			mouse_cur_->SetCursor(this, PP_MOUSECURSOR_TYPE_COLUMNRESIZE, pp::ImageData(), pp::Point(0,0));		 
			app_resizing_ = true;
			initial_xCoord_ = mEvent.GetPosition().x();
			last_xPosition_ = initial_xCoord_;
		}
		PostMessage(pp::Var((int32_t)col_no));
		//else - do nothing
        //GotMouseEvent(pp::MouseInputEvent(IEvent), "Down");
        break;
	  case PP_INPUTEVENT_TYPE_MOUSEUP:		//that
	    mEvent = pp::MouseInputEvent(IEvent);
		if (app_resizing_) {
		  app_resizing_ = false;
		  //render the scene
		}
        //GotMouseEvent(pp::MouseInputEvent(IEvent), "Up");
        break;
      case PP_INPUTEVENT_TYPE_MOUSEMOVE:	//that
		mEvent = pp::MouseInputEvent(IEvent);
		if (app_resizing_) {
		  //start rerendering the scene on every 3 px's difference or every 0.2 seconds?
			if (abs(mEvent.GetPosition().x() - last_xPosition_) >= 3) {
				//reassemble veholder model (change coords of affected columns, change the column that was resized
				int32_t shift = mEvent.GetPosition().x() - last_xPosition_;
				std::string gotit = veholder_->prepToResize(col_no, shift);
				PostMessage(pp::Var(shift));
				PostMessage(pp::Var(gotit));
				last_xPosition_ = mEvent.GetPosition().x();
				int result = view_->DrawLas(veholder_);
				PostMessage(pp::Var((int32_t)result));
			}
		}
		else {
		  //check if mouse coordinates match with one of border lines - change the cursors appearance
			//int col_no = 0;
			if ((col_no = veholder_->checkOutBorderCoords(mEvent.GetPosition().x())) != 0) {
				//change the cursor appearance
				mouse_cur_->SetCursor(this, PP_MOUSECURSOR_TYPE_COLUMNRESIZE, pp::ImageData(), pp::Point(0,0));
			}
			else {
				//male it pointer again
				mouse_cur_->SetCursor(this, PP_MOUSECURSOR_TYPE_POINTER, pp::ImageData(), pp::Point(0,0));
			}
		}
        //GotMouseEvent(pp::MouseInputEvent(IEvent), "Move");
        break;
      case PP_INPUTEVENT_TYPE_MOUSEENTER:	
        //GotMouseEvent(pp::MouseInputEvent(IEvent), "Enter");
        break;
      case PP_INPUTEVENT_TYPE_MOUSELEAVE:
        //GotMouseEvent(pp::MouseInputEvent(IEvent), "Leave");
        break;
      case PP_INPUTEVENT_TYPE_WHEEL:
        //GotWheelEvent(pp::WheelInputEvent(IEvent));
        break;
	  case PP_INPUTEVENT_TYPE_TOUCHSTART:
        //GotTouchEvent(pp::TouchInputEvent(IEvent), "Start");
        break;
      case PP_INPUTEVENT_TYPE_TOUCHMOVE:
        //GotTouchEvent(pp::TouchInputEvent(IEvent), "Move");
        break;
      case PP_INPUTEVENT_TYPE_TOUCHEND:
        //GotTouchEvent(pp::TouchInputEvent(IEvent), "End");
        break;
      case PP_INPUTEVENT_TYPE_TOUCHCANCEL:
        //GotTouchEvent(pp::TouchInputEvent(IEvent), "Cancel");
        break;
		case PP_INPUTEVENT_TYPE_RAWKEYDOWN:
        //PostMessage(pp::Var("PP_INPUTEVENT_TYPE_RAWKEYDOWN"));
        break;
      case PP_INPUTEVENT_TYPE_KEYDOWN:
        //PostMessage(pp::Var("PP_INPUTEVENT_TYPE_KEYDOWN"));
        break;
      case PP_INPUTEVENT_TYPE_KEYUP:
        //PostMessage(pp::Var("PP_INPUTEVENT_TYPE_KEYUP"));
        break;
      case PP_INPUTEVENT_TYPE_CHAR:
        //PostMessage(pp::Var("PP_INPUTEVENT_TYPE_CHAR"));
        break;
      case PP_INPUTEVENT_TYPE_CONTEXTMENU:
        //PostMessage(pp::Var("PP_INPUTEVENT_TYPE_CONTEXTMENU"));
        break;
      // Note that if we receive an IME event we just send a message back
      // to the browser to indicate we have received it.
      case PP_INPUTEVENT_TYPE_IME_COMPOSITION_START:
        //PostMessage(pp::Var("PP_INPUTEVENT_TYPE_IME_COMPOSITION_START"));
        break;
      case PP_INPUTEVENT_TYPE_IME_COMPOSITION_UPDATE:
        //PostMessage(pp::Var("PP_INPUTEVENT_TYPE_IME_COMPOSITION_UPDATE"));
        break;
      case PP_INPUTEVENT_TYPE_IME_COMPOSITION_END:
        //PostMessage(pp::Var("PP_INPUTEVENT_TYPE_IME_COMPOSITION_END"));
        break;
      case PP_INPUTEVENT_TYPE_IME_TEXT:
        //PostMessage(pp::Var("PP_INPUTEVENT_TYPE_IME_COMPOSITION_TEXT"));
        break;
	  default:
		  return false;
		  break;
	}
	return true;
  }


  void LRInstance::GotMouseEvent(const pp::MouseInputEvent& mouse_event,
                     const std::string& kind) {
    std::ostringstream stream;
    stream << 
            " Mouse event:" << kind
           << " modifier:" << ModifierToString(mouse_event.GetModifiers())
           << " button:" << MouseButtonToString(mouse_event.GetButton())
           << " x:" << mouse_event.GetPosition().x()
           << " y:" << mouse_event.GetPosition().y()
           << " click_count:" << mouse_event.GetClickCount()
           << " time:" << mouse_event.GetTimeStamp()
           << "\n";
    PostMessage(pp::Var(stream.str()));
  }

  void LRInstance::GotWheelEvent(const pp::WheelInputEvent& wheel_event) {
    std::ostringstream stream;
    stream << 
            " modifier:" << ModifierToString(wheel_event.GetModifiers())
           << " deltax:" << wheel_event.GetDelta().x()
           << " deltay:" << wheel_event.GetDelta().y()
           << " wheel_ticks_x:" << wheel_event.GetTicks().x()
           << " wheel_ticks_y:"<< wheel_event.GetTicks().y()
           << " scroll_by_page: "
           << (wheel_event.GetScrollByPage() ? "true" : "false")
           << "\n";
    PostMessage(pp::Var(stream.str()));
  }

  void LRInstance::GotTouchEvent(const pp::TouchInputEvent& touch_event,
                     const std::string& kind) {
    std::ostringstream stream;
    stream << 
            " Touch event:" << kind
           << " modifier:" << ModifierToString(touch_event.GetModifiers());
    uint32_t touch_count =
        touch_event.GetTouchCount(PP_TOUCHLIST_TYPE_CHANGEDTOUCHES);
    for (uint32_t i = 0; i < touch_count; ++i) {
      pp::TouchPoint point =
          touch_event.GetTouchByIndex(PP_TOUCHLIST_TYPE_CHANGEDTOUCHES, i);
      stream << " x[" << point.id() << "]:" << point.position().x();
      stream << " y[" << point.id() << "]:" << point.position().y();
      stream << " radius_x[" << point.id() << "]:" << point.radii().x();
      stream << " radius_y[" << point.id() << "]:" << point.radii().y();
      stream << " angle[" << point.id() << "]:" << point.rotation_angle();
      stream << " pressure[" << point.id() << "]:" << point.pressure();
    }
    PostMessage(pp::Var(stream.str()));
  }


  /*void LRInstance::InitBorderCoordinates() {
	  hborder_coords_.push_back(TOP_LINE);
	  hborder_coords_.push_back(TOOLBAR_LLINE);
	  hborder_coords_.push_back(HEADER_LLINE);
	  view_->set_hborder_coords_(hborder_coords_);
	  vborder_coords_.push_back(LEFT_LINE);
	  vborder_coords_.push_back(INIT_SCALE_WIDTH);
	  view_->set_vborder_coords_(vborder_coords_);
  }*/

  /*void LRInstance::OpenFile() {
	    file_io_ = new pp::FileIO(this);
		file_io_->Open(*file_ref_,
                 PP_FILEOPENFLAG_READ | PP_FILEOPENFLAG_WRITE |
                 PP_FILEOPENFLAG_CREATE,
                 pp::CompletionCallback(AsyncCallbacks::FileOpenCallback,
                                        this));
  }*/

  /*void LRInstance::SaveTempFile() {
	  if (file_io_ == NULL) {
            PostMessage(pp::Var("Could not read the file"));
            return;
        };
	  //const char* lorem = "Lorem Ipsum";
	  //PostMessage(pp::Var((int32_t)message_contents_.length()));
	  int32_t rv = file_io_->Write((int64_t)0, &message_contents_[0], (int32_t)message_contents_.length(), 
							pp::CompletionCallback(AsyncCallbacks::WriteCallback, this));
	  if (rv != PP_OK_COMPLETIONPENDING) {
		PostMessage(pp::Var("ERROR: Could not write to a file."));
		}
	}*/

  /*void LRInstance::ReadFromFile() {
      //file_io_ = new pp::FileIO(this);
	  //PostMessage(pp::Var(file_ref_->GetPath())); //fileref still seems ok
	  //char* read_here;
      int32_t rv = file_io_->Read((int64_t)0, &bytes_buffer_[0], (int32_t)11, 
			pp::CompletionCallback(AsyncCallbacks::ReadCallback, this));
	  if (rv != PP_OK_COMPLETIONPENDING) {
		PostMessage(pp::Var("ERROR: Could not read a file."));
	  }
	  //test_message_ = (std::string)read_here;
  }*/

   /*void LRInstance::SendFile() {
        //
        if (file_io_ == NULL) {
            //
            PostMessage(pp::Var("Could not read the file"));
            return;
        };
		PostMessage(pp::Var("We've got to the SendFile()"));
        PostMessage(pp::Var(this->bytes_buffer_));
   }*/

   //void LRInstance::FileProcessing() {}

   void LRInstance::ParseLas() {
   
	   //data::las::las_t las_;

	   char const marker = 0;

	   //read file
	   int read_ = read(fd_, &bytes_buffer_[0], mes_len_);
	   
	   if (read_ <=0)
			PostMessage(pp::Var("smths wrong with reading op"));

	   //PostMessage(pp::Var(bytes_buffer_));

	   char* s = read_las(las_, bytes_buffer_, &marker);
	   PostMessage(pp::Var(std::string(s)));

	   DisplayLas();
		   
   }

   void LRInstance::DisplayLas(){
	   //first: 
	   //pp::Rect* position_ = view_.GetRect();
	   //pp::Rect* clip_ = view_.GetClipRect();

	   //view_->UpdateView(&position_, &clip_);
	   
	   //pp::Rect testRect = pp::Rect::Rect(0, 31, 80, 50);
	   //view_->UpdateSmallView(testRect, this);
	   //view_->DrawLas();
	   pp::Size size_ = view_->GetSize();
	   
	   veholder_ = new VEHolder(/*widths*/scale_width_, columns_width_, (int32_t)size_.width(),
								/*heights*/header_height_, (int32_t)size_.height());

	   PostMessage(pp::Var(size_.height()));
	   std::string gotit = veholder_->getImageDataLas(&las_); 

	   PostMessage(pp::Var(gotit));

	   data::las::logs_section_t& logs = las_.logs_section();
	   data::las::logs_section_t::logs_list_t logs_list_ = logs.logs_list();

	   /*for (unsigned int ii=0; ii<logs_list_[0].curve.size(); ++ii)
	   {
		   char stri[40];
		   if(sprintf(stri, "index: %d, value: %f", logs_list_[0].curve[ii].index, logs_list_[0].curve[ii].value))
			   PostMessage(pp::Var((std::string)stri));
		   else PostMessage(pp::Var("Error in sprintf"));
	   }*/

	   //PostMessage(pp::Var((int32_t)logs_list_.size()));

	   //PostMessage(pp::Var((int32_t)veholder_->column_list_.size()));

	   int result = view_->DrawLas(veholder_);
	   
	   PostMessage(pp::Var((int32_t)result));

	   //PostMessage(pp::Var((int32_t)size_.width()));
   }

   void LRInstance::UnzipLVZFile() { 

		//pass the fd_ or file_name_ of opened file and pointer to proxy_ to OpenZip
	    proxy_->close(fd_);
		HZIP hz = OpenZip((void*)&file_name_[0], proxy_, 0, ZIP_FILENAME); //why is hz == 0?
		ZIPENTRY entry;
		ZRESULT res = GetZipItem(hz, -1, &entry);
		if (ZR_OK != res) PostMessage(pp::Var("GetZipItem(-1) not ZR_OK"));

		int n_entries = entry.index;
		for (int i=0; i<n_entries; i++)
		{
			if (ZR_OK == GetZipItem(hz, i, &entry)) 
			{
				if (ZR_OK == UnzipItem(hz, 0, (void*)entry.name, 0, ZIP_FILENAME)) 
				{
					int fd = open(entry.name, O_RDWR);
					std::string readbuf;
					readbuf.resize(entry.unc_size);
					if (0 != lseek(fd, 0, 0)) PostMessage(pp::Var("lseek != 0"));
					if (0 < read(fd, &readbuf[0], entry.unc_size))
					{
						//everything's ok, we can proceed
					}
				} //UnzipItem
				else PostMessage(pp::Var("UnzipItem's not OK"));
			}//GetZipItem i
			else PostMessage(pp::Var("GetZipItem's not OK"));
		} //for

   }
		//for error check
		/*switch(res){
			case ZR_OK: PostMessage(pp::Var("ZR_OK"));
				break;
			case ZR_RECENT: PostMessage(pp::Var("ZR_RECENT"));
				break;
			case ZR_GENMASK: PostMessage(pp::Var("ZR_GENMASK"));
				break;
			case ZR_NODUPH: PostMessage(pp::Var("ZR_NODUPH"));
				break;
			case ZR_NOFILE: PostMessage(pp::Var("ZR_NOFILE"));
				break;
			case ZR_NOALLOC: PostMessage(pp::Var("ZR_NOALLOC"));
				break;
			case ZR_WRITE: PostMessage(pp::Var("ZR_WRITE"));
				break;
			case ZR_NOTFOUND: PostMessage(pp::Var("ZR_NOTFOUND"));
				break;
			case ZR_MORE: PostMessage(pp::Var("ZR_MORE"));
				break;
			case ZR_CORRUPT: PostMessage(pp::Var("ZR_CORRUPT"));
				break;
			case ZR_READ: PostMessage(pp::Var("ZR_READ"));
				break;
			case ZR_CALLERMASK: PostMessage(pp::Var("ZR_CALLERMASK"));
				break;
			case ZR_ARGS: PostMessage(pp::Var("ZR_ARGS"));
				break;
			case ZR_NOTMMAP: PostMessage(pp::Var("ZR_NOTMMAP"));
				break;
			case ZR_MEMSIZE: PostMessage(pp::Var("ZR_MEMSIZE"));
				break;
			case ZR_FAILED: PostMessage(pp::Var("ZR_FAILED"));
				break;
			case ZR_ENDED: PostMessage(pp::Var("ZR_ENDED"));
				break;
			case ZR_MISSIZE: PostMessage(pp::Var("ZR_MISSIZE"));
				break;
			case ZR_PARTIALUNZ: PostMessage(pp::Var("ZR_PARTIALUNZ"));
				break;
			case ZR_ZMODE: PostMessage(pp::Var("ZR_ZMODE"));
				break;
			case ZR_BUGMASK: PostMessage(pp::Var("ZR_BUGMASK"));
				break;
			case ZR_NOTINITED: PostMessage(pp::Var("ZR_NOTINITED"));
				break;
			case ZR_SEEK: PostMessage(pp::Var("ZR_SEEK"));
				break;
			case ZR_NOCHANGE: PostMessage(pp::Var("ZR_NOCHANGE"));
				break;
			case ZR_FLATE: PostMessage(pp::Var("ZR_FLATE"));
				break;
			default: PostMessage(pp::Var("What is that?"));
				break;
		}*/ //eo switch

   char* LRInstance::read_las(data::las::las_t& las, std::string is, char const* marker = 0) {

       readers::LasReader rdr(las);

       char* state = !marker ? rdr.read(is/*, &vbuf_*/) : rdr.read(is, /*&vbuf_,*/ marker);
       return state;
   }

  //

 
class LogRunModule : public pp::Module {
 public:
  LogRunModule() : pp::Module() {}
  virtual ~LogRunModule() {}

  /// Create and return a HelloTutorialInstance object.
  /// @param[in] instance The browser-side instance.
  /// @return the plugin-side instance.
  virtual pp::Instance* CreateInstance(PP_Instance instance) {
	  return new LogRun::LRInstance(instance);
  }
};
} //namespace LogRun

namespace pp {
/// Factory function called by the browser when the module is first loaded.
/// The browser keeps a singleton of this module.  It calls the
/// CreateInstance() method on the object you return to make instances.  There
/// is one instance per <embed> tag on the page.  This is the main binding
/// point for your NaCl module with the browser.
Module* CreateModule() {
	return new LogRun::LogRunModule();
}
}  // namespace pp
