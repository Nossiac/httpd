/**********************************************************************
 * Distributed under the terms of the GNU GPL, version 2
 * Author: Hua Shao <nossiac@163.com>
 **********************************************************************/

#include <iostream>
#include "response.hpp"

using namespace std;


http_status _rsp_[] =
{
    {100, "100", "Continue"},
    /* This means that the server has received the request headers, and that the client should proceed to send the request body (in the case of a request for which a body needs to be sent; for example, a POST request). If the request body is large, sending it to a server when a request has already been rejected based upon inappropriate headers is inefficient. To have a server check if the request could be accepted based on the request's headers alone, a client must send Expect: 100-continue as a header in its initial request and check if a 100 Continue status code is received in response before continuing (or receive 417 Expectation Failed and not continue). */
    {101, "101", "Switching Protocols"},
    /* This means the requester has asked the server to switch protocols and the server is acknowledging that it will do so. */
    {102, "102", "Processing (WebDAV; RFC 2518)"},
    /* As a WebDAV request may contain many sub-requests involving file operations, it may take a long time to complete the request. This code indicates that the server has received and is processing the request, but no response is available yet. This prevents the client from timing out and assuming the request was lost. */
    {200, "200", "OK"},
    /* Standard response for successful HTTP requests. The actual response will depend on the request method used. In a GET request, the response will contain an entity corresponding to the requested resource. In a POST request the response will contain an entity describing or containing the result of the action. */
    {201, "201", "Created"},
    /* The request has been fulfilled and resulted in a new resource being created. */
    {202, "202", "Accepted"},
    /* The request has been accepted for processing, but the processing has not been completed. The request might or might not eventually be acted upon, as it might be disallowed when processing actually takes place. */
    {203, "203", "Non-Authoritative Information (since HTTP/1.1)"},
    /* The server successfully processed the request, but is returning information that may be from another source. */
    {204, "204", "No Content"},
    /* The server successfully processed the request, but is not returning any content. */
    {205, "205", "Reset Content"},
    /* The server successfully processed the request, but is not returning any content. Unlike a 204 response, this response requires that the requester reset the document view. */
    {206, "206", "Partial Content"},
    /* The server is delivering only part of the resource due to a range header sent by the client. The range header is used by tools like wget to enable resuming of interrupted downloads, or split a download into multiple simultaneous streams. */
    {207, "207", "Multi-Status (WebDAV; RFC 4918)"},
    /* The message body that follows is an XML message and can contain a number of separate response codes, depending on how many sub-requests were made. */
    {208, "208", "Already Reported (WebDAV; RFC 5842)"},
    /* The members of a DAV binding have already been enumerated in a previous reply to this request, and are not being included again. */
    {226, "226", "IM Used (RFC 3229)"},
    /* The server has fulfilled a GET request for the resource, and the response is a representation of the result of one or more instance-manipulations applied to the current instance.  */
    {300, "300", "Multiple Choices"},
    /* Indicates multiple options for the resource that the client may follow. It, for instance, could be used to present different format options for video, list files with different extensions, or word sense disambiguation. */
    {301, "301", "Moved Permanently"},
    /* This and all future requests should be directed to the given URI. */
    {302, "302", "Found"},
    /* This is an example of industry practice contradicting the standard. The HTTP/1.0 specification (RFC 1945) required the client to perform a temporary redirect (the original describing phrase was \"Moved Temporarily\"), but popular browsers implemented 302 with the functionality of a 303 See Other. Therefore, HTTP/1.1 added status codes 303 and 307 to distinguish between the two behaviours. However, some Web applications and frameworks use the 302 status code as if it were the 303.[citation needed] */
    {303, "303", "See Other (since HTTP/1.1)"},
    /* The response to the request can be found under another URI using a GET method. When received in response to a POST (or PUT/DELETE), it should be assumed that the server has received the data and the redirect should be issued with a separate GET message. */
    {304, "304", "Not Modified"},
    /* Indicates the resource has not been modified since last requested. Typically, the HTTP client provides a header like the If-Modified-Since header to provide a time against which to compare. Using this saves bandwidth and reprocessing on both the server and client, as only the header data must be sent and received in comparison to the entirety of the page being re-processed by the server, then sent again using more bandwidth of the server and client. */
    {305, "305", "Use Proxy (since HTTP/1.1)"},
    /* Many HTTP clients (such as Mozilla and Internet Explorer) do not correctly handle responses with this status code, primarily for security reasons. */
    {306, "306", "Switch Proxy"},
    /* No longer used. Originally meant \"Subsequent requests should use the specified proxy.\" */
    {307, "307", "Temporary Redirect (since HTTP/1.1)"},
    /* In this case, the request should be repeated with another URI; however, future requests can still use the original URI. In contrast to 302, the request method should not be changed when reissuing the original request. For instance, a POST request must be repeated using another POST request. */
    {308, "308", "Permanent Redirect (experimental Internet-Draft)"},
    /* The request, and all future requests should be repeated using another URI. 307 and 308 (as proposed) parallel the behaviours of 302 and 301, but do not require the HTTP method to change. So, for example, submitting a form to a permanently redirected resource may continue smoothly. */
    {400, "400", "Bad Request"},
    /* The request cannot be fulfilled due to bad syntax. */
    {401, "401", "Unauthorized"},
    /* Similar to 403 Forbidden, but specifically for use when authentication is possible but has failed or not yet been provided. The response must include a WWW-Authenticate header field containing a challenge applicable to the requested resource. See Basic access authentication and Digest access authentication. */
    {402, "402", "Payment Required"},
    /* Reserved for future use. The original intention was that this code might be used as part of some form of digital cash or micropayment scheme, but that has not happened, and this code is not usually used. As an example of its use, however, Apple's MobileMe service generates a 402 error (\"httpStatusCode:402\" in the Mac OS X Console log) if the MobileMe account is delinquent.[citation needed] */
    {403, "403", "Forbidden"},
    /* The request was a legal request, but the server is refusing to respond to it. Unlike a 401 Unauthorized response, authenticating will make no difference. */
    {404, "404", "Not Found"},
    /* The requested resource could not be found but may be available again in the future. Subsequent requests by the client are permissible. */
    {405, "405", "Method Not Allowed"},
    /* A request was made of a resource using a request method not supported by that resource; for example, using GET on a form which requires data to be presented via POST, or using PUT on a read-only resource. */
    {406, "406", "Not Acceptable"},
    /* The requested resource is only capable of generating content not acceptable according to the Accept headers sent in the request. */
    {407, "407", "Proxy Authentication Required"},
    /* The client must first authenticate itself with the proxy. */
    {408, "408", "Request Timeout"},
    /* The server timed out waiting for the request. According to W3 HTTP specifications: \"The client did not produce a request within the time that the server was prepared to wait. The client MAY repeat the request without modifications at any later time.\" */
    {409, "409", "Conflict"},
    /* Indicates that the request could not be processed because of conflict in the request, such as an edit conflict. */
    {410, "410", "Gone"},
    /* Indicates that the resource requested is no longer available and will not be available again. This should be used when a resource has been intentionally removed and the resource should be purged. Upon receiving a 410 status code, the client should not request the resource again in the future. Clients such as search engines should remove the resource from their indices. Most use cases do not require clients and search engines to purge the resource, and a \"404 Not Found\" may be used instead. */
    {411, "411", "Length Required"},
    /* The request did not specify the length of its content, which is required by the requested resource. */
    {412, "412", "Precondition Failed"},
    /* The server does not meet one of the preconditions that the requester put on the request. */
    {413, "413", "Request Entity Too Large"},
    /* The request is larger than the server is willing or able to process. */
    {414, "414", "Request-URI Too Long"},
    /* The URI provided was too long for the server to process. */
    {415, "415", "Unsupported Media Type"},
    /* The request entity has a media type which the server or resource does not support. For example, the client uploads an image as image/svg+xml, but the server requires that images use a different format. */
    {416, "416", "Requested Range Not Satisfiable"},
    /* The client has asked for a portion of the file, but the server cannot supply that portion. For example, if the client asked for a part of the file that lies beyond the end of the file. */
    {417, "417", "Expectation Failed"},
    /* The server cannot meet the requirements of the Expect request-header field. */
    {418, "418", "I'm a teapot (RFC 2324)"},
    /* This code was defined in 1998 as one of the traditional IETF April Fools' jokes, in RFC 2324, Hyper Text Coffee Pot Control Protocol, and is not expected to be implemented by actual HTTP servers. However, known implementations do exist. */
    {420, "420", "Enhance Your Calm (Twitter)"},
    /* Returned by the Twitter Search and Trends API when the client is being rate limited. Likely a reference to this number's association with marijuana. Other services may wish to implement the 429 Too Many Requests response code instead. The phrase \"Enhance Your Calm\" is a reference to Demolition Man (film). In the film, Sylvester Stallone's character John Spartan is a hot-head in a generally more subdued future, and is regularly told to \"Enhance your calm\" rather than a more common phrase like \"calm down\". */
    {422, "422", "Unprocessable Entity (WebDAV; RFC 4918)"},
    /* The request was well-formed but was unable to be followed due to semantic errors. */
    {423, "423", "Locked (WebDAV; RFC 4918)"},
    /* The resource that is being accessed is locked. */
    {424, "424", "Failed Dependency (WebDAV; RFC 4918)"},
    /* The request failed due to failure of a previous request (e.g. a PROPPATCH). */
    {424, "424", "Method Failure (WebDAV)"},
    /* Indicates the method was not executed on a particular resource within its scope because some part of the method's execution failed causing the entire method to be aborted. */
    {425, "425", "Unordered Collection (Internet draft)"},
    /* Defined in drafts of \"WebDAV Advanced Collections Protocol\", but not present in \"Web Distributed Authoring and Versioning (WebDAV) Ordered Collections Protocol\". */
    {426, "426", "Upgrade Required (RFC 2817)"},
    /* The client should switch to a different protocol such as TLS/1.0. */
    {428, "428", "Precondition Required (RFC 6585)"},
    /* The origin server requires the request to be conditional. Intended to prevent \"the \'lost update\' problem, where a client GETs a resource's state, modifies it, and PUTs it back to the server, when meanwhile a third party has modified the state on the server, leading to a conflict.\" */
    {429, "429", "Too Many Requests (RFC 6585)"},
    /* The user has sent too many requests in a given amount of time. Intended for use with rate limiting schemes. */
    {431, "431", "Request Header Fields Too Large (RFC 6585)"},
    /* The server is unwilling to process the request because either an individual header field, or all the header fields collectively, are too large. */
    {444, "444", "No Response (Nginx)"},
    /* Used in Nginx logs to indicate that the server has returned no information to the client and closed the connection (useful as a deterrent for malware). */
    {449, "449", "Retry With (Microsoft)"},
    /* A Microsoft extension. The request should be retried after performing the appropriate action.
       Often search-engines or custom applications will ignore required parameters. Where no default action is appropriate, the Aviongoo website sends a \"HTTP/1.1 449 Retry with valid parameters: param1, param2, . . .\" response. The applications may choose to learn, or not. */
    {450, "450", "Blocked by Windows Parental Controls (Microsoft)"},
    /* A Microsoft extension. This error is given when Windows Parental Controls are turned on and are blocking access to the given webpage. */
    {451, "451", "Unavailable For Legal Reasons (Internet draft)"},
    /* Defined in the internet draft \"A New HTTP Status Code for Legally-restricted Resources\". Intended to be used when resource access is denied for legal reasons, e.g. censorship or government-mandated blocked access. Likely a reference to the 1953 dystopian novel Fahrenheit 451, where books are outlawed. */
    {499, "499", "Client Closed Request (Nginx)"},
    /* Used in Nginx logs to indicate when the connection has been closed by client while the server is still processing its request, making server unable to send a status code back. */
    {500, "500", "Internal Server Error"},
    /* A generic error message, given when no more specific message is suitable. */
    {501, "501", "Not Implemented"},
    /* The server either does not recognise the request method, or it lacks the ability to fulfill the request. */
    {502, "502", "Bad Gateway"},
    /* The server was acting as a gateway or proxy and received an invalid response from the upstream server. */
    {503, "503", "Service Unavailable"},
    /* The server is currently unavailable (because it is overloaded or down for maintenance). Generally, this is a temporary state. */
    {504, "504", "Gateway Timeout"},
    /* The server was acting as a gateway or proxy and did not receive a timely response from the upstream server. */
    {505, "505", "HTTP Version Not Supported"},
    /* The server does not support the HTTP protocol version used in the request. */
    {506, "506", "Variant Also Negotiates (RFC 2295)"},
    /* Transparent content negotiation for the request results in a circular reference. */
    {507, "507", "Insufficient Storage (WebDAV; RFC 4918)"},
    /* The server is unable to store the representation needed to complete the request. */
    {508, "508", "Loop Detected (WebDAV; RFC 5842)"},
    /* The server detected an infinite loop while processing the request (sent in lieu of 208). */
    {509, "509", "Bandwidth Limit Exceeded (Apache bw/limited extension)"},
    /* This status code, while used by many servers, is not specified in any RFCs. */
    {510, "510", "Not Extended (RFC 2774)"},
    /* Further extensions to the request are required for the server to fulfill it. */
    {511, "511", "Network Authentication Required (RFC 6585)"},
    /* The client needs to authenticate to gain network access. Intended for use by intercepting proxies used to control access to the network (e.g. \"captive portals\" used to require agreement to Terms of Service before granting full Internet access via a Wi-Fi hotspot). */
    {598, "598", "Network read timeout error (Unknown)"},
    /* This status code is not specified in any RFCs, but is used by Microsoft Corp. HTTP proxies to signal a network read timeout behind the proxy to a client in front of the proxy. */
    {599, "599", "Network connect timeout error (Unknown)"},
    /* This status code is not specified in any RFCs, but is used by Microsoft Corp. HTTP proxies to signal a network connect timeout behind the proxy to a client in front of the proxy. */
};



Response::Response()
{
    this->code = 0;
    this->setOption("Server", SERVER_INFO);
    this->setOption("Author", AUTHOR_INFO);
    this->version = "HTTP/1.1";
}


Response::~Response()
{
}


http_status * Response::get_http_status(int code)
{
    size_t i;
    for(i=0; i<sizeof(_rsp_)/sizeof(http_status); i++)
    {
        if(_rsp_[i].i_code == code)
            return &(_rsp_[i]);
    }
    return NULL;
}


string Response::getOption(const char * key)
{
    if (this->options.count(key))
        return this->options[key];
    return string();
}


void Response::setOption(const char * key, size_t value)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%zu", value);
    this->options.insert(pair<string, string>(key, buf));
}


void Response::setOption(const char * key, const char * value)
{
    this->options.insert(pair<string, string>(key, value));
}


void Response::setVersion(string httpver)
{
    this->version = httpver;
}

void Response::setCode(int code)
{
    http_status * stt = this->get_http_status(code);
    if (stt)
    {
        this->code = code;
        this->code_str = stt->s_code;
        this->code_expl = stt->s_expl;
    }
    else
    {
    }
}

#ifdef __DEBUG__
void Response::dump()
{
    DEBUG("rsp code: %d", this->code);
}
#endif


