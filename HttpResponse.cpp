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

HttpResponse::HttpResponse(int code, String status, String data)
{
  this->code = code;
  this->status = status;
  this->data = data;
}

HttpResponse::HttpResponse()
{
}

String HttpResponse::toString()
{
  return 
    protocol + " " + String(code) + " " + 
    status + "\r\n" +
    "Content-Type: " + contentType + "\r\n" +
    "Access-Control-Allow-Origin: *\r\n" +
    "HSA-Version: " + String(HTTP_SERVER_ADVANCED_VERSION) + "\r\n" +
    "\r\n" + data + "\r\n";
}

HttpResponse HttpResponse::BadRequest(String data)
{
  return HttpResponse(400, "Bad Request", data);
}

HttpResponse HttpResponse::NotFound(String data)
{
  return HttpResponse(404, "Not Found", data);
}

HttpResponse HttpResponse::Unacceptable(String data)
{
  return HttpResponse(406, "Not Acceptable", data);
}

HttpResponse HttpResponse::InternalError(String data)
{
  return HttpResponse(500, "Internal Server Error", data);
}
