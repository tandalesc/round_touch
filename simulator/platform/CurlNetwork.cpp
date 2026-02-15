#include "platform/CurlNetwork.h"

#include <curl/curl.h>
#include <cstdio>
#include <string>

static size_t writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
  std::string *response = static_cast<std::string *>(userdata);
  response->append(ptr, size * nmemb);
  return size * nmemb;
}

void CurlNetwork::init() {
  curl_global_init(CURL_GLOBAL_DEFAULT);
  printf("[CurlNetwork] Initialized (desktop HTTP via libcurl)\n");
}

bool CurlNetwork::isConnected() {
  return true;
}

HttpResponse CurlNetwork::get(const char *url, const char *authHeader) {
  HttpResponse response;
  CURL *curl = curl_easy_init();
  if (!curl) return response;

  std::string responseBody;
  struct curl_slist *headers = nullptr;
  if (authHeader) {
    std::string hdr = std::string("Authorization: ") + authHeader;
    headers = curl_slist_append(headers, hdr.c_str());
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
  if (headers) {
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  }

  CURLcode res = curl_easy_perform(curl);
  if (res == CURLE_OK) {
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    response.statusCode = (int)httpCode;
    response.body = String(responseBody.c_str());
  } else {
    printf("[CurlNetwork] GET failed: %s\n", curl_easy_strerror(res));
  }

  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  return response;
}

HttpResponse CurlNetwork::post(const char *url, const char *body,
                                const char *contentType,
                                const char *authHeader) {
  HttpResponse response;
  CURL *curl = curl_easy_init();
  if (!curl) return response;

  std::string responseBody;
  struct curl_slist *headers = nullptr;

  std::string ctHdr = std::string("Content-Type: ") + contentType;
  headers = curl_slist_append(headers, ctHdr.c_str());
  if (authHeader) {
    std::string authHdr = std::string("Authorization: ") + authHeader;
    headers = curl_slist_append(headers, authHdr.c_str());
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

  CURLcode res = curl_easy_perform(curl);
  if (res == CURLE_OK) {
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    response.statusCode = (int)httpCode;
    response.body = String(responseBody.c_str());
  } else {
    printf("[CurlNetwork] POST failed: %s\n", curl_easy_strerror(res));
  }

  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  return response;
}
