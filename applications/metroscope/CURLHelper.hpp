/*
 * CURLHelper.hpp
 *
 *  Created on: Feb 18, 2014
 *      Author: lprisan
 */

#ifndef CURLHELPER_HPP_
#define CURLHELPER_HPP_

#include <string>
#include <curl/curl.h>

class CURLHelper {
public:
	CURLHelper();
    std::string getData () { return curlBuffer; }
    void makeRequest(std::string pUrl);
    std::string urlEncode (std::string pString);
    void cleanup();

    static int curlWriter(char *data, size_t size, size_t nmemb, std::string *buffer) {
          int result = 0;
          if (buffer != NULL) {
            buffer->append(data, size * nmemb);
            result = size * nmemb;
          }
          return result;
    }

  protected:
    CURL * curl;
    std::string curlBuffer;

};

#endif /* CURLHELPER_HPP_ */
