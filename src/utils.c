#include <utils.h>

int check_valid_ip(const char* ip)
{
    int dotsCount = 0; // number of dots in the address
    bool is_first_symbol_is_dot = true; // flag to check if the first symbol is a dot (for cases like .125.201///)
    int lastPart = 0; // value of the last detected octet in the address (to check if it exceeds 255)

    while (*ip)
    {
        char ch = *ip++; // increment the pointer along the IP address
        if (ch == '.')
        {
            if (is_first_symbol_is_dot == true)
            {
                return false; // if the IP address starts with a dot, consider it invalid
            }
            dotsCount++;
            if (lastPart > 255)
            {
                return false; // if the last octet exceeds 255, consider the address invalid
            }
            lastPart = 0;
        }
        else if (atoi(&ch)) // if a number is detected
        {
            // add it to our octet
            lastPart *= 10;
            lastPart += atoi(&ch);
        }
        else
        {
            return false;
        }
        is_first_symbol_is_dot = false; // reset the flag as we move to the second symbol
    }
    if (lastPart <= 256 && dotsCount == 3) // final check for compliance with conditions
    {
        return true;
    }
    return false;
}