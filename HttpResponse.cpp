#include "HttpResponse.h"

HttpResponse::HttpResponse(int code, String status)
{
  this->code = code;
  this->status = status;
}

HttpResponse::HttpResponse(String data)
{
  this->data = data;
}

HttpResponse::HttpResponse()
{
}

String HttpResponse::toString()
{
  return protocol + " " + String(code) + " " + status + "\r\n" +
    "Content-Type: " + contentType +  "\r\n\r\n" + data;
}

HttpResponse HttpResponse::BadRequest()
{
  return HttpResponse(400, "Bad Request");
}

HttpResponse HttpResponse::NotFound()
{
  return HttpResponse(404, "Not Found");
}

HttpResponse HttpResponse::Unacceptable()
{
  return HttpResponse(406, "Not Acceptable");
}
