#pragma once

#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <error_code.hpp>

namespace mangle {

  typedef std::vector<unsigned char> bytes;

  /**
   * Class: Zlib Adapter
   *
   * Rudimentary zlib wrapper
   */
  class zlib_adapter {
  public:

    static const int chunk_size = 1024;

    static bool compress(support::error_code& err,
                         bytes& out,
                         const std::string& in);

    static bool uncompress(support::error_code& err,
                           std::string& out,
                           const bytes& in);
  };

  /**
   * Compress
   */
  inline bool
  zlib_adapter::
  compress(support::error_code& err,
           bytes& out,
           const std::string& in) {

    /// local vars, buffers..
    int ret = 0;
    z_stream strm;
    unsigned char input[chunk_size] = {0};
    unsigned char output[chunk_size] = {0};

    /// init zlib structure
    strm.zalloc = Z_NULL;
    strm.zfree  = Z_NULL;
    strm.opaque = Z_NULL;

    /// init zlib deflater
    ret = deflateInit(&strm, Z_BEST_COMPRESSION);
    if (ret != Z_OK) {
      std::ostringstream oss;
      oss << "Failed to initialize zlib for compression: " << ret;
      support::error_code::attach_or_create(err, -1, oss.str());
      return false;
    }
    /// set input to supplied string, we compress all in one shot
    strm.avail_in = in.size();
    strm.next_in  = (unsigned char *) in.c_str();

    /// allocate some memory to save some repeated allocations
    out.reserve(chunk_size);

    do {

      /// supply the point where the compressed data goes
      strm.avail_out = chunk_size;
      strm.next_out  = output;

      /// and attempt to compress in one go..
      ret = deflate(&strm, Z_FINISH);
      if (ret == Z_STREAM_ERROR) {
        std::ostringstream oss;
        oss << "Failed to deflate chunk: " << ret;
        support::error_code::attach_or_create(err, -1, oss.str());

        /// cleanup or leak..
        deflateEnd(&strm);
        return false;
      }
      /// append the compressed blob onto the output vector of bytes
      int count = chunk_size - strm.avail_out;
      out.insert(out.end(), &output[0], &output[count]);
    }
    /// continues until zlib does not provide any more compressed output
    while (strm.avail_out == 0);

    /// cleanup or leak..
    deflateEnd(&strm);
    return true;
  }

  inline bool
  zlib_adapter::
  uncompress(support::error_code& err,
             std::string& out,
             const bytes& in) {

    /// local vars, buffers..
    int ret = 0;
    z_stream strm;
    unsigned char output[chunk_size] = {0};

    /// init zlib structure
    strm.zalloc = Z_NULL;
    strm.zfree  = Z_NULL;
    strm.opaque = Z_NULL;
 
    /// set input to supplied bytes, we compress all in one shot
    strm.avail_in = in.size();
    strm.next_in  = (unsigned char *) &in[0];

    /// init zlib deflater
    ret = inflateInit(&strm);
    if (ret != Z_OK) {
      std::ostringstream oss;
      oss << "Failed to initialize zlib for decompression: " << ret;
      support::error_code::attach_or_create(err, -1, oss.str());
      return false;
    }

    do {

      /// supply the point where the compressed data goes
      strm.avail_out = chunk_size;
      strm.next_out  = output;

      /// and attempt to uncompress in one go..
      ret = inflate(&strm, Z_FINISH);

      /// these appear to be the errors - are there any others
      switch (ret) {
        case Z_STREAM_ERROR:
        case Z_NEED_DICT:
        case Z_DATA_ERROR:
        case Z_MEM_ERROR: {
          std::ostringstream oss;
          oss << "Failed to inflate chunk: " << ret;
          support::error_code::attach_or_create(err, -1, oss.str());

          /// cleanup or leak
          inflateEnd(&strm);
          return false;
        }
      }
      /// append uncompressed data onto output string
      int count = chunk_size - strm.avail_out;
      out.append((const char *) output, count);
    }
    /// keep going until zlib tells us we are done
    while (ret != Z_STREAM_END);

    /// cleanup or leak
    inflateEnd(&strm);
    return true;
  }

}
