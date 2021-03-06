/**===========================================================================*\
 * Name        : plodder.cpp
 * Author      : Brett Lynnes
 * Copyright   : 2016 Palo Alto Innovation, all rights reserved
 * Description : Main file to execute and control the rest of the software
\**===========================================================================*/

#include <iostream>
#include <fcntl.h>
#include <glib-2.0/glib.h>
#include <stdint.h>
#include "GPIOManip.h"
#include "mic.h"
#include <nghttp2/asio_http2_client.h>

using namespace std;

using boost::asio::ip::tcp;

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::client;


static void doAmazonStuff() {
    string baseAmazon = "https://avs-alexa-na.amazon.com";
}


static gboolean onButtonEvent(GIOChannel *channel, GIOCondition condition, gpointer user_data) {
    micHandle *mic_handle = (micHandle *) user_data;
    char buf[10];
    int buf_sz = 10;
    GError *error = 0;
    gsize bytes_read = 0;

    cerr << "onButtonEvent" << endl;

    g_io_channel_seek_position(channel, 0, G_SEEK_SET, 0);
    GIOStatus rc = g_io_channel_read_chars(channel, buf, buf_sz - 1, &bytes_read, &error);
    cerr << "rc:" << rc << "  data:" << buf << " with bytes: " << bytes_read << endl;

    int err;
    if (buf[0] == '1') {
        cout << "Reading from mic!!" << endl;
        for (int i = 0; i < 10; ++i) {
            cout << "Attempting to read " << mic_handle->buffer_frames << " frames into a buffer" << endl;
            err = snd_pcm_readi(mic_handle->capture_handle, mic_handle->buffer, mic_handle->buffer_frames);
            if (err != ((int) mic_handle->buffer_frames)) {
                if (err < 0) {
                    cerr << "read from audio interface failed " << err << " (" << snd_strerror(err) << ")" << endl;
                    err = snd_pcm_recover(mic_handle->capture_handle, err, 0);
                    if (err < 0) {
                        cerr << "Error fix failed " << err << " (" << snd_strerror(err) << ")" << endl;
                        exit(1);
                    }
                } else {
                    cerr << "Short read from mic: " << err << endl;
                }
            }
            cout << "read " << i << " done" << endl;
        }
        doAmazonStuff();
    }

    return 1;
}

//typedef struct
//{
//    char RIFF_marker[4];
//    uint32_t file_size;
//    char filetype_header[4];
//    char format_marker[4];
//    uint32_t data_header_length;
//    uint16_t format_type;
//    uint16_t number_of_channels;
//    uint32_t sample_rate;
//    uint32_t bytes_per_second;
//    uint16_t bytes_per_frame;
//    uint16_t bits_per_sample;
//} WaveHeader;


//WaveHeader *genericWAVHeader(uint32_t sample_rate, uint16_t bit_depth, uint16_t channels)
//{
//    WaveHeader *hdr;
//    hdr = malloc(sizeof(*hdr));
//    if (!hdr) return NULL;
//
//    memcpy(&hdr->RIFF_marker, "RIFF", 4);
//    memcpy(&hdr->filetype_header, "WAVE", 4);
//    memcpy(&hdr->format_marker, "fmt ", 4);
//    hdr->data_header_length = 16;
//    hdr->format_type = 1;
//    hdr->number_of_channels = channels;
//    hdr->sample_rate = sample_rate;
//    hdr->bytes_per_second = sample_rate * channels * bit_depth / 8;
//    hdr->bytes_per_frame = channels * bit_depth / 8;
//    hdr->bits_per_sample = bit_depth;
//
//    return hdr;
//}

//int writeWAVHeader(int fd, WaveHeader *hdr)
//{
//    if (!hdr)
//        return -1;
//
//    write(fd, &hdr->RIFF_marker, 4);
//    write(fd, &hdr->file_size, 4);
//    write(fd, &hdr->filetype_header, 4);
//    write(fd, &hdr->format_marker, 4);
//    write(fd, &hdr->data_header_length, 4);
//    write(fd, &hdr->format_type, 2);
//    write(fd, &hdr->number_of_channels, 2);
//    write(fd, &hdr->sample_rate, 4);
//    write(fd, &hdr->bytes_per_second, 4);
//    write(fd, &hdr->bytes_per_frame, 2);
//    write(fd, &hdr->bits_per_sample, 2);
//    write(fd, "data", 4);
//
//    uint32_t data_size = hdr->file_size - 36;
//    write(fd, &data_size, 4);
//
//    return 0;
//}

void doHTTPStuff() {
try{
    boost::system::error_code ec;
    boost::asio::io_service io_service;

    std::string uri = "https://google.com";
    std::string scheme, host, service;

    if (host_service_from_uri(ec, scheme, host, service, uri)) {
      std::cerr << "error: bad URI: " << ec.message() << std::endl;
      exit(1);
    }

    boost::asio::ssl::context tls_ctx(boost::asio::ssl::context::sslv23);
    tls_ctx.set_default_verify_paths();
    // disabled to make development easier...
    // tls_ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    configure_tls_context(ec, tls_ctx);

    auto sess = scheme == "https" ? session(io_service, tls_ctx, host, service)
                                  : session(io_service, host, service);

    sess.on_connect([&sess, &uri](tcp::resolver::iterator endpoint_it) {
      boost::system::error_code ec;

      auto req = sess.submit(ec, "GET", uri);

      if (ec) {
        std::cerr << "error: " << ec.message() << std::endl;
        return;
      }

      req->on_response([&sess](const response &res) {
        std::cerr << "HTTP/2 " << res.status_code() << std::endl;
        for (auto &kv : res.header()) {
          std::cerr << kv.first << ": " << kv.second.value << "\n";
        }
        std::cerr << std::endl;

        res.on_data([&sess](const uint8_t *data, std::size_t len) {
          std::cerr.write(reinterpret_cast<const char *>(data), len);
          std::cerr << std::endl;
        });
      });

      req->on_close([&sess](uint32_t error_code) { sess.shutdown(); });
    });

    sess.on_error([](const boost::system::error_code &ec) {
      std::cerr << "error: " << ec.message() << std::endl;
    });

    io_service.run();
  } catch (std::exception &e) {
    std::cerr << "exception: " << e.what() << "\n";
  }

}

int main() {


    doHTTPStuff();

    sleep(10);
    cerr << "FIN!" << endl;
    exit(0);

	char *buffer;
    string state;

	paiMic *mic = new paiMic();
	GPIOManip* gpio415 = new GPIOManip("415");

	micHandle user_data;

	gpio415->export_gpio();
	cout << "Exported GPIO" << endl;

	gpio415->setdir_gpio("in");
	gpio415->setedge_gpio("both");

	gpio415->getval_gpio(state);
	cout << "Current state: " << state << endl;

	GMainLoop* loop = g_main_loop_new( 0, 0 );

	int fd = open( "/sys/class/gpio/gpio415/value", O_RDONLY | O_NONBLOCK );
	GIOChannel* channel = g_io_channel_unix_new( fd );
	GIOCondition cond = GIOCondition( G_IO_PRI );

	// TODO: fix buffer sizing
	buffer = new char[2*mic->getApproxBufferSize()];
    user_data.buffer = buffer;
    user_data.buffer_frames = mic->getBufferFrames();
    user_data.capture_handle = mic->getCaptureHandle();

	//guint id = g_io_add_watch( channel, cond, onButtonEvent, &user_data );
	g_io_add_watch( channel, cond, onButtonEvent, &user_data );

	g_main_loop_run( loop );

	gpio415->unexport_gpio();


	delete [] buffer;

	cout << "buffer freed" << endl;

	mic->shutdown();

	return 0;
}
