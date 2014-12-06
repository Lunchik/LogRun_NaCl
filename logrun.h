 /**
  * @file logrun.h
  * @author garipova.nailya@tgtoil.com
  * @author Copyright(C) 2012 TGT Oil & Gas Services. All right reserved.
  */

#include <string>
//#include <iostream>
//#include <sstream>
//#include <iterator>
#include <vector>

#include "ppapi/c/pp_file_info.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/image_data.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/size.h"

//#include "view_files/veholder.h"
#include "Las.h"
#include "view1.h"
#include "LasReader.h"

#include <nacl-mounts/base/KernelProxy.h>

/*namespace pp {
class FileIO;
class FileRef;
class FileSystem;
class Rect;
}  */// namespace pp


namespace LogRun {

	class View;
	class VEHolder;

    class LRInstance : public pp::Instance {
        public:
            explicit LRInstance(PP_Instance instance);
            virtual ~LRInstance();

            //Open the file
            virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);

            virtual void DidChangeView(const pp::Rect& position, const pp::Rect& clip);

            //to handle the post message
            virtual void HandleMessage(const pp::Var& var_message);

			//to handle the input events
			virtual bool HandleInputEvent(const pp::InputEvent& event);

			void GotMouseEvent(const pp::MouseInputEvent& mouse_event,
                     const std::string& kind);
			void GotWheelEvent(const pp::WheelInputEvent& wheel_event);
			void GotTouchEvent(const pp::TouchInputEvent& touch_event,
                     const std::string& kind);
			
			//void InitBorderCoordinates();
			//void OpenFile();
			//void SaveTempFile();
            //void ReadFromFile();
            //void FileProcessing();
			void UnzipLVZFile();
			//void SendFile();
			void ParseLas();
			void DisplayLas();
            //void ReadFromBuffer();

			static void* thread_static(void* param);
			void* thread();

            PP_FileInfo file_info_;
            int32_t bytes_to_read_;
            int64_t offset_;
			KernelProxy* proxy_;
			//MainThreadRunner* runner_;
            //pp::FileIO* file_io_;
            //pp::FileRef* file_ref_;
            pp::FileSystem* file_system_;
			std::string bytes_buffer_;
			std::string message_contents_;
			int32_t i_test;
			std::string test_message_;
			std::string file_name_;
			int data_format_;
			int mes_len_;

			//bool thread_started_;
			int fd_;
			//pthread_t main_thread_;
			ssize_t written_;

			//std::vector<int32_t> hborder_coords_;
			//std::vector<int32_t> vborder_coords_;

			data::las::las_t las_;
			VEHolder* veholder_;
			pp::MouseCursor *mouse_cur_;


        private:
            LRInstance(const LRInstance&); //disallow copy

			char* read_las(data::las::las_t& las, std::string is, char const* marker);
			View* view_;
			int32_t scale_width_;
			int32_t columns_width_;
			int32_t header_height_;
			bool app_resizing_;
			int32_t initial_xCoord_;
			int32_t last_xPosition_;
			int col_no;
			
			//int32_t columns_height_;

			/*class JSDirectoryReader: public DirectoryReader {
			public:
				pp::Instance* instance_;
				pp::CompletionCallback cc_;
				std::set<std::string>* entries_;

				JSDirectoryReader(pp::Instance* instance) : instance_(instance) {}

				int ReadDirectory(const std::string& path, std::set<std::string>* entries, const pp::CompletionCallback& cc) {
					cc_ = cc;
					entries_ = entries;
					std::string message = "[\"ReadDirectory\",\"" + path + "\"]";
					instance_->PostMessage(message);
					return 0;
				}

				void HandleResponse(const std::string& response) {
					fprintf(stderr, "response: %s\n", response.c_str());
					std::string::const_iterator ind = response.begin();
					std::string::const_iterator next = response.begin();
					while (ind != response.end() && next != response.end()) {
						if (*next == '\n' && ind != next) {
							if (*ind == '\n') {
							++ind;
							}
							if (ind != next) {
								entries_->insert(std::string(ind, next));
							}
							ind = next;
						}
						++next;
					}
					if (ind != next) {
						std::string last(ind, next-1);
						if (!last.empty()) {
							entries_->insert(last);
						}
					}
					cc_.Run(PP_OK);
				}
			};

			JSDirectoryReader directory_reader_;*/
    };
}