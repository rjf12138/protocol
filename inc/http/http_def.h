#ifndef __HTTP_DEF_H__
#define __HTTP_DEF_H__

#define HTTP_VERSION    "HTTP/1.1"

// http 请求方法
#define HTTP_METHOD_UNKNOWN "NONE"
#define HTTP_METHOD_GET     "GET"
#define HTTP_METHOD_POST    "POST"
#define HTTP_METHOD_PUT     "PUT"
#define HTTP_METHOD_DELETE  "DELETE"
#define HTTP_METHOD_HEAD    "HEAD"
#define HTTP_METHOD_OPTION  "OPTION"

// http 返回状态
// 状态码 2XX： 表明请求被正常处理了
#define HTTP_STATUS_OK              200 //正常请求
#define HTTP_STATUS_NoContent       204 //处理成功，但是没有资源返回
#define HTTP_STATUS_PartialContent  206 //客户端进行范围请求，服务器成功执行了，Content-Range 指定范围的实体内容
// 状态码 3XX： 表明浏览器需要执行某些特殊的处理以正确处理请求
#define HTTP_STATUS_MovedPermanently 301 // 永久性重定向。该状态码表示请求的资源已被分配了新的 URI
#define HTTP_STATUS_Found            302 // 临时性重定向。该状态码表示请求的资源已被分配了新的 URI,希望用户(本次)能使用新的 URI 访问 
#define HTTP_STATUS_SeeOther         303 // 该状态码表示由于请求对应的资源存在着另一个 URI,应使用 GET方法定向获取请求的资源。
#define HTTP_STATUS_NotModified      304 // 该状态码表示客户端发送附带条件的请求 2 时,服务器端允许请求访问资源,但未满足条件的情况
#define HTTP_STATUS_TemporaryRedirect 307 // 临时重定向。该状态码与 302 Found 有着相同的含义。
// 状态吗 4XX： 表明客户端是发生错误的原因所在。
#define HTTP_STATUS_BadRequest        400 // 该状态码表示请求报文中存在语法错误
#define HTTP_STATUS_Unauthorized      401 // 该状态码表示发送的请求需要有通过 HTTP 认证(BASIC 认证、DIGEST 认证)的认证信息。另外若之前已进行过 1 次请求,则表示用 户认证失败。
#define HTTP_STATUS_Forbidden         403 // 该状态码表明对请求资源的访问被服务器拒绝了。
#define HTTP_STATUS_NotFount          404 // 该状态码表明服务器上无法找到请求的资源。

#define HTTP_HEADER_OPTION_

#endif