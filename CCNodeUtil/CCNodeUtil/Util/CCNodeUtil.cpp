/*
 *   FILE: Util.cpp
 * AUTHOR: Michael David Plotz
 *   DATE: Fri Mar 15 19:21:37 PDT 2013
 */

#include "CCNodeUtil.h"

std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> elements;
    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, delim))
    {
        elements.push_back(token);
    }
    return elements;
}

