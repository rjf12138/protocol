#include "http.h"

namespace my_utils {

HttpPtl::HttpPtl(void)
{

}

HttpPtl::HttpPtl(ByteBuffer &data)
{

}

HttpPtl::~HttpPtl(void)
{

}

int 
HttpPtl::parser(ByteBuffer &data)
{
    data.find();
}

int 
HttpPtl::generate(ByteBuffer &data)
{

}

int 
HttpPtl::set_content(ByteBuffer &data)
{

}

int 
HttpPtl::set_header(const string &method, int status)
{

}

int 
HttpPtl::set_header_option(const string &key, const string &value)
{

}

int 
HttpPtl::get_status(void)
{

}

string 
HttpPtl::get_method(void)
{

}

string 
HttpPtl::get_header_option(const string &key)
{

}

int 
HttpPtl::get_content(ByteBuffer &data)
{

}

string 
HttpPtl::get_status_phrase(int status)
{

}

}